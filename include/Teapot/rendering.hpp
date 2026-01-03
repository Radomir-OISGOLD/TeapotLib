
#pragma once

#include "Teapot/cap.hpp"

namespace Teapot
{

	struct Device;
	struct Instance;
	struct Image;
	struct Pipeline;
	struct RenderPass;
	struct DispatchTable;
	struct Framebuffer;

	struct Queue
	{
		Queue(Device& device, vkb::QueueType type);

		~Queue();

		Device* p_device;

		vk::Queue handle;
	};


	struct Swapchain
	{
		Swapchain(Device& device);

		~Swapchain();

		void initSemaphores(DispatchTable& table);

		vec<VkSemaphore> available_semaphores;
        vec<VkSemaphore> finished_semaphore;
        vec<VkFence> in_flight_fences;
        vec<VkFence> image_in_flight;

		vkb::Swapchain handle;
		vec<Image*> swapchain_images;
		
		Device* p_device;
	};

	struct DispatchTable
	{
		DispatchTable(Device& device);

		~DispatchTable();

		Device* p_device;
		vec<Framebuffer*> framebuffers;
		RenderPass* p_rend_pass;

		vkb::DispatchTable handle;
	};


	struct Shader
	{

		Shader(DispatchTable& table, const char* file_path);
		Shader(DispatchTable& table, std::string inline_shader);	

		~Shader();

		vec<char> code;
		DispatchTable* p_table;

		VkShaderModule handle;

	};

	struct CommandPool
	{
		/*
		 * Creates command pool for a specified dispatch table and its parent
		 * device and queue family index corresponding to the type of queue specified.
		 */
		CommandPool(DispatchTable& table, vkb::QueueType queue_type);

		~CommandPool();

		void allocBuffers(Pipeline& pipeline);

		DispatchTable* p_table;
		vec<VkCommandBuffer> buffers;

		VkCommandPool handle;
	};
	
	// struct CommandBuffer
	// {
	// 	friend struct CommandPool;

	// 	private:	
	// 	CommandPool* p_pool;

	// 	VkCommandBuffer handle;
	// };

	struct Pipeline
	{
		Pipeline(Swapchain& swapchain, Shader& sh_vert, Shader& sh_frag);
		~Pipeline();

		Device* p_device;
		DispatchTable* p_table;

		VkPipeline handle;
		VkPipelineLayout* p_layout;
		VkRenderPass* p_render_pass;

		VkViewport viewport;
		VkRect2D scissor;
	};

	struct RenderPass
	{
		RenderPass(Swapchain& swapchain, DispatchTable& table);
		~RenderPass();

		VkRenderPass handle;
	};

	struct Framebuffer
	{
		Framebuffer(DispatchTable& table, Swapchain& swapchain, RenderPass& render_pass);
		~Framebuffer();

		DispatchTable* p_table;
		VkFramebuffer handle;
	};


	int drawFrame(size_t* p_frame, DispatchTable& table, Swapchain& chain, CommandPool& pool, Queue& graphics_queue, Queue& present_queue);
}
