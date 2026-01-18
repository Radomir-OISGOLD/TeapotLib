
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
#include <functional>

#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vkb/VkBootstrap.h>
#include <glm/vec2.hpp>
#include <vulkan/vk_enum_string_helper.h>


#define TEAPOT_DOUBLE_BUFFERING 2

namespace Teapot
{
	// Forward declarations
	class Init;
	class RenderData;

	class Device;
	class PhysDevice;
	class Instance;
	class Queue;
	class Swapchain;
	class DispatchTable;
	class InstanceDispatchTable;
	class Image;
	class Texture;
	class LoadedImage;

	class Shader;
	class CommandPool;
	class Pipeline;
	class RenderPass;
	class Framebuffer;
	class DescriptorSetLayout;
	class DescriptorPool;
	class DescriptorSet;

	class Window;
	class Surface;
	class InputManager;
	class MouseState;

	enum class ButtonState;
	class ButtonTextures;
	class Button;
	class UIVertex;
	class Projection;
	class UIQuad;
	class UIRenderer;

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

