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
        Instance instance("Button Example");
        Window window("Textured Button", { 800, 600 }, &instance);
        window.addSurface();
        window.plugInput();

        instance.physical_devices.emplace_back(std::make_unique<PhysDevice>(&instance));

        auto& phys_device = instance.physical_devices.back();
        auto device_ptr = phys_device->createLogicalDevice();
        auto& device = *device_ptr;

        device.swapchain = std::make_unique<Swapchain>(&device);

        Queue graphics_queue(&device, vkb::QueueType::graphics);
        Queue present_queue(&device, vkb::QueueType::present);

        RenderPass render_pass(&device, device.swapchain.get());

        vec<Framebuffer> framebuffers;
        framebuffers.reserve(device.swapchain->images.size());

        for (auto& image : device.swapchain->images)
        {
            framebuffers.emplace_back(&device, &render_pass, device.swapchain.get(), image);
        }

        CommandPool command_pool(&device, vkb::QueueType::graphics);
        command_pool.allocateCommandBuffers(
            static_cast<uint32_t>(device.swapchain->images.size())
        );

        DescriptorSetLayout desc_layout(&device);
        DescriptorPool desc_pool(&device, 10);

        UIRenderer ui_renderer(&device, &render_pass, device.swapchain.get());

        Shader ui_vert(&device, "shaders/ui_button.vert.spv");
        Shader ui_frag(&device, "shaders/ui_button.frag.spv");

        ui_renderer.createPipeline(ui_vert, ui_frag, desc_layout);
        ui_renderer.createBuffers();

        LoadedImage button_img("path/to/button.png");
        Texture button_texture(&device, &command_pool, &graphics_queue, button_img);

        ButtonTextures btn_textures;
        btn_textures.idle = &button_texture;
        btn_textures.hover = &button_texture;
        btn_textures.press = &button_texture;
        btn_textures.off = &button_texture;

        window.addButton(
            {300.0f, 250.0f},
            {500.0f, 350.0f},
            btn_textures,
            []() {
                std::cout << "Button clicked!" << std::endl;
            }
        );

        size_t current_frame = 0;
        const size_t MAX_FRAMES = 2; // Double buffering

        std::cout << "Starting render loop..." << std::endl;

        while (!glfwWindowShouldClose(window.handle))
        {
            glfwPollEvents();

            window.input->update();
            window.updateButtons();

            device.device.waitForFences(
                { device.swapchain->in_flight_fences[current_frame] },
                true,
                UINT64_MAX
            );

            uint32_t image_index;
            auto result = device.device.acquireNextImageKHR(
                device.swapchain->swapchain,
                UINT64_MAX,
                device.swapchain->available_semaphores[current_frame],
                nullptr
            );

            if (result.result == vk::Result::eErrorOutOfDateKHR || result.result == vk::Result::eSuboptimalKHR)
            {
                continue;
            }
            image_index = result.value;

            device.device.resetFences({ device.swapchain->in_flight_fences[current_frame] });

            ui_renderer.prepareFrame(
                window.getButtons(),
                window,
                desc_pool,
                desc_layout
            );

            command_pool.recordUICommands(
                image_index,
                render_pass,
                framebuffers.data(),
                ui_renderer,
                window
            );

            vk::SubmitInfo submit_info;

            vk::Semaphore wait_semaphores[] = {
                device.swapchain->available_semaphores[current_frame]
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
                device.swapchain->finished_semaphores[current_frame]
            };
            submit_info.signalSemaphoreCount = 1;
            submit_info.pSignalSemaphores = signal_semaphores;

            graphics_queue.handle.submit(
                { submit_info },
                device.swapchain->in_flight_fences[current_frame]
            );

            vk::PresentInfoKHR present_info;
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = signal_semaphores;

            vk::SwapchainKHR swapchains[] = { device.swapchain->swapchain };
            present_info.swapchainCount = 1;
            present_info.pSwapchains = swapchains;
            present_info.pImageIndices = &image_index;

            present_queue.handle.presentKHR(present_info);

            current_frame = (current_frame + 1) % MAX_FRAMES;
        }

        device.device.waitIdle();

        std::cout << "Application closed successfully" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
