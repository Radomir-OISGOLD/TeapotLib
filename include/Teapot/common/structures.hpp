
#pragma once

#include "Teapot/common/cap.hpp"
#include "Teapot/platform/window.hpp"
#include "Teapot/core/instance.hpp"
#include "Teapot/core/device.hpp"
#include "Teapot/core/swapchain.hpp"
#include "Teapot/core/queue.hpp"
#include "Teapot/pipeline/commandpool.hpp"
#include "Teapot/pipeline/framebuffer.hpp"
#include "Teapot/pipeline/renderpass.hpp"
#include "Teapot/pipeline/descriptors.hpp"
#include "Teapot/core/dispatch.hpp"


namespace Teapot
{

	struct Init
	{
		Window* p_window;
		Instance* p_instance;
		vkb::InstanceDispatchTable p_inst_disp;
		Surface* p_surface;
		PhysDevice* p_phys_device;
		Device* p_device;
		DispatchTable* p_disp;
		Swapchain* p_swapchain;
	};

	struct RenderData
	{
		Device* p_device;
		Queue* p_graphics_queue;
		Queue* p_present_queue;

		Swapchain* p_swapchain;
		Framebuffer* p_framebuffers;
		RenderPass* p_render_pass;
		Pipeline* p_graphics_pipeline;
		CommandPool* p_command_pool;

		DescriptorSetLayout* p_descriptor_set_layout;
		DescriptorPool* p_descriptor_pool;

		vec<VkCommandBuffer> command_buffers;

		vec<VkSemaphore> available_semaphores;
		vec<VkSemaphore> finished_semaphore;
		vec<VkFence> in_flight_fences;
		vec<VkFence> image_in_flight;

		size_t current_frame = 0;
	};

}
