
#pragma once

/*
 * The topmost header of Vulkan Teapot Library.
 */

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <cstdint>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <vulkan/vk_enum_string_helper.h>

/*
 * Concept of encapsulation got sacrificed
 * to save nerve system. Friends are good,
 * but in C++ they're just hell.
 */

#define TEAPOT_DOUBLE_BUFFERING 2

namespace Teapot
{


	template <typename T>
	using vec = std::vector<T>;

	template <typename T, std::size_t L>
	using arr = std::array<T, L>;

	template <typename T, typename L>
	using map = std::map<T, L>;

	template <typename T>
	using sptr = std::shared_ptr<T>;

	template <typename T>
	using uptr = std::unique_ptr<T>;

	using str = std::string;
	using u16t = uint16_t;
	using u32t = uint32_t;
	using u64t = uint64_t;


	void inline err(std::string message)
	{
		throw std::exception(&message[0]);
	}

	void inline isVkOk(VkResult res, std::string message)
	{
		if (res != VK_SUCCESS)
		{
			std::cout << message << " VkResult code: " << string_VkResult(res) << "\n";
			err("Vulkan function thrown an error.");
		}
	}

} // namespace Teapot

