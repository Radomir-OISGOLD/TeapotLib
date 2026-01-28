
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

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <glm/vec2.hpp>
#include <vulkan/vk_enum_string_helper.h>

#define TEAPOT_DOUBLE_BUFFERING 2

namespace Teapot
{
	// Forward declarations
	struct Device;
	struct PhysDevice;
	struct Instance;
	struct Queue;
	struct Swapchain;
	struct DispatchTable;
	struct Image;
	struct Texture;
	struct LoadedImage;

	struct Shader;
	struct CommandPool;
	class Pipeline;
	struct RenderPass;
	struct Framebuffer;
	struct DescriptorSetLayout;
	struct DescriptorPool;
	struct DescriptorSet;

	class Window;
	class Surface;
	class InputManager;
	struct MouseState;

	enum class ButtonState;
	struct ButtonTextures;
	class Button;
	struct UIVertex;
	class Projection;
	struct UIQuad;
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

	void inline isVkOk(vk::Result res, std::string message)
	{
		if (res != vk::Result::eSuccess)
		{
			std::cout << message << " VkResult code." << "\n";
			err("Vulkan function thrown an error.");
		}
	}

} // namespace Teapot

