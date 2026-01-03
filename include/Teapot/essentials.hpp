
#pragma once

#include <Teapot/window.hpp>
#include <Teapot/instance.hpp>
#include <Teapot/rendering.hpp>
#include <Teapot/device.hpp>

namespace Teapot
{
    
    struct Init
    {
        Window window;
        Instance instance;
        DispatchTable inst_table;
        Surface surface;
        Device device;
        DispatchTable table;
        Swapchain swapchain;

    };

    struct RenderData
    {
        Queue graphics_queue;
        Queue present_queue;

        vec<VkImage> swapchain_images;
        vec<VkImageView> swapchain_image_views;
        vec<Framebuffer> framebuffers;

        RenderPass render_pass;
        Pipeline pipeline;

        CommandPool command_pool;

        vec<VkSemaphore> available_semaphores;
        vec<VkSemaphore> finished_semaphore;
        vec<VkFence> in_flight_fences;
        vec<VkFence> image_in_flight;

        size_t current_frame = 0;
    };

    // TODO - MAKE METHODS FIT THIS STRUCTURE

}

