
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

    void inline oneFuncTest()
    {
        const char* app_name = "Vulkan Teapot App";
        const char* window_title = "Vulkan Teapot Window";
        unsigned int w = 800;
        unsigned int h = 600;

        std::string inline_vert = R"(
        #version 450
        #extension GL_ARB_separate_shader_objects : enable

        layout (location = 0) out vec3 fragColor;

        vec2 positions[3] = vec2[](vec2 (0.0, -0.5), vec2 (0.5, 0.5), vec2 (-0.5, 0.5));

        vec3 colors[3] = vec3[](vec3 (1.0, 0.0, 0.0), vec3 (0.0, 1.0, 0.0), vec3 (0.0, 0.0, 1.0));

        void main ()
        {
            gl_Position = vec4 (positions[gl_VertexIndex], 0.0, 1.0);
            fragColor = colors[gl_VertexIndex];
        }
        )";

        std::string inline_frag = R"(
        #version 450
        #extension GL_ARB_separate_shader_objects : enable

        layout (location = 0) in vec3 fragColor;

        layout (location = 0) out vec4 outColor;

        void main () { outColor = vec4 (fragColor, 1.0); }
        )";


        vec<VkImage> swapchain_images;
        vec<VkImageView> swapchain_image_views;
    
        vec<VkSemaphore> available_semaphores;
        vec<VkSemaphore> finished_semaphore;
        vec<VkFence> in_flight_fences;
        vec<VkFence> image_in_flight;

        size_t current_frame = 0;

        Instance inst(app_name);
        
        Window window(inst, window_title, w, h);
        Surface surf(window);

        PhysDevice phys(surf);
        Device dev(phys);

        DispatchTable table(dev);

        Shader sh_vert(table, inline_vert);
        Shader sh_frag(table, inline_frag);

        Swapchain swapchain(dev);
        RenderPass rpass(swapchain, table);
        Framebuffer fbuf(table, swapchain, rpass);

        Queue qu_graphics(dev, vkb::QueueType::graphics);
        Queue qu_present(dev, vkb::QueueType::present);
        // Queues must be created before creating pools, this must be prohibited implicitly. TODO
        CommandPool pool(table, vkb::QueueType::graphics);

        Pipeline pipeline(swapchain, sh_vert, sh_frag);

        swapchain.initSemaphores(table);
        
        while (!glfwWindowShouldClose(window.handle))
        {
            glfwPollEvents();
            int res = drawFrame(&current_frame, table, swapchain, pool,  qu_graphics, qu_present);
            if (res != 0) 
            {
                err("Failed to draw frame.");
            }
        }

        table.handle.deviceWaitIdle();

    }

}
