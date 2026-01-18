#include <Teapot/teapot.hpp>
#include <Teapot/core/instance.hpp>
#include <Teapot/core/device.hpp>
#include <Teapot/core/queue.hpp>
#include <Teapot/core/image.hpp>
#include <Teapot/platform/window.hpp>
#include <Teapot/pipeline/shader.hpp>
#include <Teapot/pipeline/renderpass.hpp>
#include <Teapot/pipeline/framebuffer.hpp>
#include <Teapot/pipeline/commandpool.hpp>
#include <Teapot/pipeline/descriptors.hpp>
#include <Teapot/ui/ui_renderer.hpp>
#include <Teapot/ui/button.hpp>
#include <iostream>

using namespace Teapot;

int main()
{
    try
    {
        // =============================================================================
        // 1. CREATE CORE OBJECTS
        // =============================================================================

        Instance instance("Button Example");
        Window& window = instance.createWindow("Textured Button", 800, 600);

        // =============================================================================
        // 2. INITIALIZE VULKAN OBJECTS
        // =============================================================================

        Init init = {};
        init.p_instance = &instance;
        init.p_window = &window;

        window.createSurface(&init);
        init.p_surface = window.surface.get();

        PhysDevice& phys_device = instance.createPhysicalDevice(&init);
        init.p_phys_device = &phys_device;

        // Device automatically creates and owns Swapchain and DispatchTable
        auto device_ptr = phys_device.createLogicalDevice(&init);
        Device& device = *device_ptr;
        init.p_device = &device;
        init.p_disp = device.table.get();
        init.p_swapchain = device.swapchain.get();

        // Initialize input handling
        window.initInput(&init);

        // =============================================================================
        // 3. CREATE RENDERING COMPONENTS
        // =============================================================================

        Queue graphics_queue(&init, vkb::QueueType::graphics);
        Queue present_queue(&init, vkb::QueueType::present);

        RenderPass render_pass(&init);

        // Create framebuffers for each swapchain image
        vec<std::unique_ptr<Framebuffer>> framebuffers;
        framebuffers.reserve(device.swapchain->images.size());

        RenderData render_data = {};
        render_data.p_device = &device;
        render_data.p_graphics_queue = &graphics_queue;
        render_data.p_present_queue = &present_queue;
        render_data.p_swapchain = device.swapchain.get();
        render_data.p_render_pass = &render_pass;

        for (auto& image : device.swapchain->images)
        {
            framebuffers.push_back(
                std::make_unique<Framebuffer>(&render_data, image)
            );
        }

        CommandPool command_pool(&init, vkb::QueueType::graphics);
        command_pool.allocateCommandBuffers(
            static_cast<uint32_t>(device.swapchain->images.size())
        );

        // =============================================================================
        // 4. SETUP UI RENDERING
        // =============================================================================

        DescriptorSetLayout desc_layout(&init);
        DescriptorPool desc_pool(&init, 10);

        render_data.p_descriptor_set_layout = &desc_layout;
        render_data.p_descriptor_pool = &desc_pool;

        UIRenderer ui_renderer(&render_data);

        Shader ui_vert(&init, "shaders/ui_button.vert.spv");
        Shader ui_frag(&init, "shaders/ui_button.frag.spv");

        ui_renderer.createPipeline(ui_vert, ui_frag, desc_layout);
        ui_renderer.createBuffers();

        // =============================================================================
        // 5. CREATE BUTTON WITH TEXTURE
        // =============================================================================

        // Load button texture
        LoadedImage button_img("path/to/button.png");
        Texture button_texture(&init, &render_data, button_img);

        // Setup button textures (using same texture for all states)
        ButtonTextures btn_textures = {};
        btn_textures.normal = &button_texture;
        btn_textures.hovered = &button_texture;
        btn_textures.pressed = &button_texture;
        btn_textures.disabled = &button_texture;

        // Create button in center of window (200x100 pixels)
        window.newButton(
            vec2(300.0f, 250.0f),  // Bottom-left corner
            vec2(500.0f, 350.0f),  // Top-right corner
            btn_textures,
            []() {
                std::cout << "Button clicked!" << std::endl;
            }
        );

        // =============================================================================
        // 6. MAIN RENDER LOOP
        // =============================================================================

        size_t current_frame = 0;
        const size_t MAX_FRAMES = 2; // Double buffering

        std::cout << "Starting render loop..." << std::endl;

        while (!glfwWindowShouldClose(window.handle))
        {
            glfwPollEvents();

            // Update input state
            window.input->update();

            // Update button interactions
            window.updateButtons();

            // === ACQUIRE NEXT IMAGE ===

            device.table->handle.waitForFences(
                1,
                &device.swapchain->in_flight_fences[current_frame],
                VK_TRUE,
                UINT64_MAX
            );

            uint32_t image_index;
            VkResult result = device.table->handle.acquireNextImageKHR(
                device.swapchain->handle,
                UINT64_MAX,
                device.swapchain->available_semaphores[current_frame],
                VK_NULL_HANDLE,
                &image_index
            );

            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
            {
                // Swapchain needs recreation (window resized)
                continue;
            }

            device.table->handle.resetFences(
                1,
                &device.swapchain->in_flight_fences[current_frame]
            );

            // === PREPARE AND RECORD COMMANDS ===

            ui_renderer.prepareFrame(
                window.getButtons(),
                window,
                desc_pool,
                desc_layout
            );

            command_pool.recordUICommands(
                image_index,
                render_pass,
                framebuffers[image_index].get(),
                ui_renderer,
                window
            );

            // === SUBMIT TO QUEUE ===

            VkSubmitInfo submit_info = {};
            submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            VkSemaphore wait_semaphores[] = {
                device.swapchain->available_semaphores[current_frame]
            };
            VkPipelineStageFlags wait_stages[] = {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            };
            submit_info.waitSemaphoreCount = 1;
            submit_info.pWaitSemaphores = wait_semaphores;
            submit_info.pWaitDstStageMask = wait_stages;
            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = &command_pool.buffers[image_index];

            VkSemaphore signal_semaphores[] = {
                device.swapchain->finished_semaphores[current_frame]
            };
            submit_info.signalSemaphoreCount = 1;
            submit_info.pSignalSemaphores = signal_semaphores;

            VkResult submit_result = graphics_queue.handle.submit(
                1,
                &submit_info,
                device.swapchain->in_flight_fences[current_frame]
            );
            isVkOk(submit_result, "Failed to submit draw command buffer");

            // === PRESENT ===

            VkPresentInfoKHR present_info = {};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = signal_semaphores;

            VkSwapchainKHR swapchains[] = {device.swapchain->handle};
            present_info.swapchainCount = 1;
            present_info.pSwapchains = swapchains;
            present_info.pImageIndices = &image_index;

            present_queue.handle.presentKHR(&present_info);

            current_frame = (current_frame + 1) % MAX_FRAMES;
        }

        // Wait for device to finish all operations
        device.table->handle.deviceWaitIdle();

        std::cout << "Application closed successfully" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
