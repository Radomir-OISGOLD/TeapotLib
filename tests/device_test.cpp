#include <gtest/gtest.h>

#include <Teapot/common/structures.hpp>


namespace Teapot
{
	void err(const std::string& message)
	{
		throw std::runtime_error(message);
	}
}

class DeviceTest : public ::testing::Test 
{
protected:
	void SetUp() override {
		try {
			instance = std::make_unique<Teapot::Instance>("TestApp");
			window = &instance->createWindow("Test", 800, 600);

			init = std::make_unique<Teapot::Init>();
			init->p_instance = instance.get();
			init->p_window = window;

			window->createSurface(init.get());
			init->p_surface = window->surface.get();
			ASSERT_NE(init->p_surface, nullptr) << "Surface was not created.";
			ASSERT_NE(init->p_surface->handle, VK_NULL_HANDLE) << "Surface handle is null.";

			physDevice = &instance->createPhysicalDevice(init.get());
			init->p_phys_device = physDevice;
			ASSERT_NE(init->p_phys_device, nullptr) << "Physical device was not created.";
			ASSERT_NE(init->p_phys_device->handle.physical_device, VK_NULL_HANDLE) << "Physical device handle is null.";
		}
		catch (const std::exception& e) {
			FAIL() << "Setup failed with exception: " << e.what();
		}
	}

	void TearDown() override
	{
		// instance unique_ptr will handle cleanup of all resources
	}

	std::unique_ptr<Teapot::Instance> instance;
	Teapot::Window* window = nullptr;
	Teapot::PhysDevice* physDevice = nullptr;
	std::unique_ptr<Teapot::Init> init;
};

// --- PhysDevice Tests ---

TEST_F(DeviceTest, PhysDeviceConstructor)
{
	ASSERT_NE(physDevice, nullptr);
	EXPECT_NE(physDevice->handle.physical_device, VK_NULL_HANDLE);
	EXPECT_EQ(physDevice->p_instance, instance.get());
}

TEST_F(DeviceTest, PhysDeviceCreateLogicalDevice)
{
	ASSERT_NE(physDevice, nullptr);
	ASSERT_NE(init, nullptr);

	std::unique_ptr<Teapot::Device> device;
	ASSERT_NO_THROW({
		device = physDevice->createLogicalDevice(init.get());
	});
	
	ASSERT_NE(device, nullptr);
	EXPECT_NE(device->handle.device, VK_NULL_HANDLE);
	EXPECT_EQ(device->p_phys_device, physDevice);
	EXPECT_NE(device->table, nullptr);
}

// --- Device Tests ---

TEST_F(DeviceTest, DeviceConstructor)
{
	ASSERT_NE(init, nullptr);
	
	std::unique_ptr<Teapot::Device> device;
	ASSERT_NO_THROW({
		device = std::make_unique<Teapot::Device>(init.get());
	});

	ASSERT_NE(device, nullptr);
	EXPECT_NE(device->handle.device, VK_NULL_HANDLE);
	EXPECT_EQ(device->p_phys_device, physDevice);
	ASSERT_NE(device->table, nullptr);
}

TEST_F(DeviceTest, DeviceDestructor) {
	ASSERT_NO_THROW({
		Teapot::Device device(init.get());
		// Destructor called at end of scope. If it doesn't crash, it passes.
	});
	SUCCEED(); // Explicitly mark as success
}
