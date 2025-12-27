
#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{

	struct Device;
	struct Instance;

	struct Queue
	{
		Queue(Device device, vkb::QueueType type);

		~Queue();

		Device* p_device;

		vk::Queue handle;
	};


	struct Swapchain
	{
		Swapchain(Device device);

		~Swapchain();

		vkb::Swapchain handle;

		Device* p_device;
	};

	struct DispatchTable
	{
		DispatchTable(Device& device);

		~DispatchTable();

		vkb::DispatchTable handle;

		Device* p_device;
	};


	struct Shader
	{

		Shader(DispatchTable& table, const char* file_path);
		Shader(DispatchTable& table, std::vector<char> inline_shader);	

		~Shader();

		vec<char> code;
		DispatchTable* p_table;

		VkShaderModule vk_handle;

	};

	struct CommandPool
	{
		/*
		 * Creates command pool for a specified dispatch table and its parent
		 * device and queue family index corresponding to the type of queue specified.
		 */
		CommandPool(DispatchTable& table, vkb::QueueType queue_type);

		~CommandPool();

		DispatchTable* p_table;

		VkCommandPool vk_handle;
	};

	struct Pipeline
	{
		
		Pipeline(Swapchain& swapchain, Shader& sh_vert, Shader& shader_frag);
		~Pipeline();

		VkPipeline* vk_handle;
		VkPipelineLayout* p_layout;
		VkRenderPass* p_render_pass;

		VkViewport viewport;
		VkRect2D scissor;
	};

	int drawFrame();
}
