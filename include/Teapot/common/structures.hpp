
#pragma once

#include "Teapot/cap.hpp"
#include "Teapot/window.hpp"
#include "Teapot/instance.hpp"
#include "Teapot/device.hpp"
#include "Teapot/rendering.hpp"

namespace Teapot
{

	struct Init
	{
		Window* p_window;
		Instance* p_instance;
		vkb::InstanceDispatchTable p_inst_disp;
		Surface* p_surface;
		Device* p_device;
		DispatchTable* p_disp;
		Swapchain* p_swapchain;
	};

	struct RenderData
	{
		Queue* p_graphics_queue;
		Queue* p_present_queue;

		Swapchain* p_swapchain;
		Framebuffer* p_framebuffers;
		RenderPass* p_render_pass;
		Pipeline* p_graphics_pipeline;
		CommandPool* p_command_pool;

		vec<VkCommandBuffer> command_buffers;

		vec<VkSemaphore> available_semaphores;
		vec<VkSemaphore> finished_semaphore;
		vec<VkFence> in_flight_fences;
		vec<VkFence> image_in_flight;
        
		size_t current_frame = 0;
	};

}
