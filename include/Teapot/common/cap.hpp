
#pragma once

/*
 * The topmost header of Vulkan Teapot Library.
 */

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <array>
#include <cstdint>

#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <vulkan/vk_enum_string_helper.h>


#define TEAPOT_DOUBLE_BUFFERING 2

namespace Teapot
{
	// Forward declarations
	// common
	struct Init;
	struct RenderData;

	// core
	struct Device;
	struct PhysDevice;
	struct Instance;
	struct Queue;
	struct Swapchain;
	struct DispatchTable;
	struct InstanceDispatchTable;
	struct Image;
	struct Texture;
	struct LoadedImage;

	// pipeline
	struct Shader;
	struct CommandPool;
	struct Pipeline;
	struct RenderPass;
	struct Framebuffer;
	
	// platform
	struct Window;
	struct Surface;

	template <typename T>
	using vec = std::vector<T>;

	template <typename T, std::size_t L>
	using arr = std::array<T, L>;

	template <typename T, typename L>
	using map = std::map<T, L>;

	using str = std::string;
	using u16t = uint16_t;
	using u32t = uint32_t;
	using u64t = uint64_t;


	void inline err(std::string message)
	{
		throw std::runtime_error(message);
	}

	void inline isVkOk(VkResult res, std::string message)
	{
		if (res != VK_SUCCESS)
		{
			std::cout << message << " VkResult code: " << string_VkResult(res) << "\n";
			err("Vulkan function thrown an error.");
		}
	}
	void inline isVkOk(vk::Result res, std::string message)
	{
		if (res != vk::Result::eSuccess)
		{
			std::cout << message << " VkResult code." << "\n";
			err("Vulkan function thrown an error.");
		}
	}

} // namespace Teapot

