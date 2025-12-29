
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
		Window* window;
		Instance* instance;
		vkb::InstanceDispatchTable inst_disp;
		Surface* surface;
		Device* device;
		DispatchTable* disp;
		Swapchain* swapchain;
	};

	struct RenderData
	{
		Queue* graphics_queue;
		Queue* present_queue;

		Swapchain* swapchain;
		Framebuffer* framebuffers;
		RenderPass* render_pass;
		Pipeline* graphics_pipeline;
		CommandPool* command_pool;

		vec<VkCommandBuffer> command_buffers;

		vec<VkSemaphore> available_semaphores;
		vec<VkSemaphore> finished_semaphore;
		vec<VkFence> in_flight_fences;
		vec<VkFence> image_in_flight;
        
		size_t current_frame = 0;
	};

}
