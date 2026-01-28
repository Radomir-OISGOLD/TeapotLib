
#include <iostream>

#include <Teapot/common/cap.hpp>

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
#include <Teapot/core/swapchain.hpp>


using namespace Teapot;

int main()
{
    try
    {
        // 1. Create Vulkan instance
        Instance instance("Simple Button Example");

        // 2. Create window using factory method
        Window* window = instance.createWindow("Click the Button!", {800, 600});
        Surface* surface = window->addSurface();
        window->plugInput();

        // 3. Create physical device using factory method
        PhysDevice* phys_device = instance.createPhysDevice(surface);

        // 4. Create logical device using factory method
        std::unique_ptr<Device> device = phys_device->createLogicalDevice();

        // 5. Create swapchain using factory method
        Swapchain* swapchain = device->createSwapchain();

        // 6. Create queues using factory methods
        Queue* graphics_queue = device->createQueue(vkb::QueueType::graphics);
        Queue* present_queue = device->createQueue(vkb::QueueType::present);

        // 7. Create render pass (public constructor)
        RenderPass render_pass(device.get(), swapchain);

        // 8. Create framebuffers - one per swapchain image (public constructor)
        vec<Framebuffer> framebuffers;
        framebuffers.reserve(swapchain->images.size());
        for (auto& image : swapchain->images)
        {
            framebuffers.emplace_back(device.get(), &render_pass, swapchain, image);
        }

        // 9. Create command pool (public constructor)
        CommandPool command_pool(device.get(), vkb::QueueType::graphics);
        command_pool.allocateCommandBuffers(
            static_cast<uint32_t>(swapchain->images.size())
        );

        // 10. Create descriptor resources (public constructors)
        DescriptorSetLayout desc_layout(device.get());
        DescriptorPool desc_pool(device.get(), 10);

        // 11. Create UI renderer (public constructor)
        UIRenderer ui_renderer(device.get(), &render_pass, swapchain);

        // 12. Load shaders (public constructor)
        Shader ui_vert(device.get(), "shaders/ui_button.vert.spv");
        Shader ui_frag(device.get(), "shaders/ui_button.frag.spv");

        ui_renderer.createPipeline(ui_vert, ui_frag, desc_layout);
        ui_renderer.createBuffers();

        // 13. Load button texture (public constructors)
        LoadedImage button_img("textures/button.png");
        Texture button_texture(device.get(), &command_pool, graphics_queue, button_img);

        // 14. Set up button textures for different states
        ButtonTextures btn_textures;
        btn_textures.idle = &button_texture;   // Normal state
        btn_textures.hover = &button_texture;  // Mouse over
        btn_textures.press = &button_texture;  // Being clicked
        btn_textures.off = &button_texture;    // Disabled state

        // 15. Add button to window using factory method
        window->addButton(
            {300.0f, 250.0f},  // Bottom-left corner (X, Y)
            {500.0f, 350.0f},  // Top-right corner (X, Y)
            btn_textures,
            []() {
                std::cout << "Button clicked! Hello from TeapotLib!" << std::endl;
            }
        );

        // 16. Main render loop
        size_t current_frame = 0;
        const size_t MAX_FRAMES = 2; // Double buffering

        std::cout << "Window created. Click the button!" << std::endl;

        while (!glfwWindowShouldClose(window->handle))
        {
            // Poll window events
            glfwPollEvents();

            // Update input state and buttons
            window->input->update();
            window->updateButtons();

            // Wait for previous frame to finish
            device->device.waitForFences(
                {swapchain->in_flight_fences[current_frame]},
                true,
                UINT64_MAX
            );

            // Acquire next swapchain image
            uint32_t image_index;
            auto result = device->device.acquireNextImageKHR(
                swapchain->swapchain,
                UINT64_MAX,
                swapchain->available_semaphores[current_frame],
                nullptr
            );

            // Handle swapchain out of date (window resize, etc.)
            if (result.result == vk::Result::eErrorOutOfDateKHR ||
                result.result == vk::Result::eSuboptimalKHR)
            {
                continue;
            }
            image_index = result.value;

            // Reset fence for this frame
            device->device.resetFences({swapchain->in_flight_fences[current_frame]});

            // Prepare UI rendering (build draw list from buttons)
            ui_renderer.prepareFrame(
                window->getButtons(),
                *window,
                desc_pool,
                desc_layout
            );

            // Record rendering commands
            command_pool.recordUICommands(
                image_index,
                render_pass,
                framebuffers.data(),
                ui_renderer,
                *window
            );

            // Submit rendering commands
            vk::SubmitInfo submit_info;

            vk::Semaphore wait_semaphores[] = {
                swapchain->available_semaphores[current_frame]
            };
            vk::PipelineStageFlags wait_stages[] = {
                vk::PipelineStageFlagBits::eColorAttachmentOutput
            };
            submit_info.waitSemaphoreCount = 1;
            submit_info.pWaitSemaphores = wait_semaphores;
            submit_info.pWaitDstStageMask = wait_stages;
            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = &command_pool.buffers[image_index];

            vk::Semaphore signal_semaphores[] = {
                swapchain->finished_semaphores[current_frame]
            };
            submit_info.signalSemaphoreCount = 1;
            submit_info.pSignalSemaphores = signal_semaphores;

            graphics_queue->handle.submit(
                {submit_info},
                swapchain->in_flight_fences[current_frame]
            );

            // Present the rendered image
            vk::PresentInfoKHR present_info;
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = signal_semaphores;

            vk::SwapchainKHR swapchains[] = {swapchain->swapchain};
            present_info.swapchainCount = 1;
            present_info.pSwapchains = swapchains;
            present_info.pImageIndices = &image_index;

            present_queue->handle.presentKHR(present_info);

            // Move to next frame
            current_frame = (current_frame + 1) % MAX_FRAMES;
        }

        // Wait for all operations to complete before cleanup
        device->device.waitIdle();

        std::cout << "Application closed successfully" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
