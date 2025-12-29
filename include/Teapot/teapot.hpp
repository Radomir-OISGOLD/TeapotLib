
#pragma once

#include "Teapot/device.hpp"
#include "Teapot/image.hpp"
#include "Teapot/instance.hpp"
#include "Teapot/rendering.hpp"
#include "Teapot/window.hpp"
#include "Teapot/structures.hpp"
#include "VkBootstrap.h"


namespace Teapot
{

    void inline oneFuncTest(const char* app_name, const char* window_title, unsigned int w, unsigned int h, std::string window_name, std::string inline_vert, std::string inline_frag)
    {
        std::vector<VkImage> swapchain_images;
        std::vector<VkImageView> swapchain_image_views;
    
        std::vector<VkSemaphore> available_semaphores;
        std::vector<VkSemaphore> finished_semaphore;
        std::vector<VkFence> in_flight_fences;
        std::vector<VkFence> image_in_flight;

        size_t current_frame = 0;

        Instance inst(app_name);
        
        Window window(inst, window_title, w, h);
        Surface surf(window);

        PhysDevice phys(surf);
        Device dev(phys);

        DispatchTable table(dev);

        Shader sh_vert(table, inline_vert);
        Shader sh_frag(table, inline_frag);

        Swapchain swagchain(dev);
        RenderPass rpass(swagchain, table);
        Framebuffer fbuf(table, swagchain, rpass);

        Queue qu_graphics(dev, vkb::QueueType::graphics);
        Queue qu_present(dev, vkb::QueueType::present);
        // Queues must be created before creating pools, this must be prohibited implicitly. TODO
        CommandPool pool(table, vkb::QueueType::graphics);

        Pipeline pipeline(swagchain, sh_vert, sh_frag);

    }

}
