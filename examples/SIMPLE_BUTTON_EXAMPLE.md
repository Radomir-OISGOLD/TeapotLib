# Simple Button Example - Complete Initialization Guide

This example demonstrates the **correct** initialization flow for creating a window with a clickable button using TeapotLib's factory methods.

## Overview

The example creates:
- An 800x600 window titled "Click the Button!"
- A single clickable button at position (300, 250) to (500, 350)
- When clicked, prints "Button clicked! Hello from TeapotLib!" to console
- Full Vulkan rendering with proper synchronization

## Prerequisites

Before running this example, you need:

1. **TeapotLib built and installed**
2. **Button texture**: Place a PNG image at `textures/button.png`
   - Format: PNG with alpha channel (RGBA)
   - Recommended size: 256x128 pixels or similar
3. **Shaders**: Compiled SPIR-V shaders at:
   - `shaders/ui_button.vert.spv`
   - `shaders/ui_button.frag.spv`

## Correct API Usage

**CRITICAL**: Most constructors in TeapotLib are **private**. You MUST use factory methods to create objects.

### Factory Methods (Private Constructors)
- `Instance::createWindow()` → creates Window
- `Instance::createPhysDevice()` → creates PhysDevice
- `PhysDevice::createLogicalDevice()` → creates Device
- `Device::createSwapchain()` → creates Swapchain
- `Device::createQueue()` → creates Queue
- `Window::addSurface()` → creates Surface
- `Window::addButton()` → creates Button

### Public Constructors
- `Instance(app_name)`
- `RenderPass(device, swapchain)`
- `Framebuffer(device, render_pass, swapchain, image)`
- `CommandPool(device, queue_type)`
- `DescriptorSetLayout(device)`
- `DescriptorPool(device, max_sets)`
- `Shader(device, file_path)`
- `LoadedImage(path)`
- `Texture(device, command_pool, queue, loaded_image)`
- `UIRenderer(device, render_pass, swapchain)`

## Initialization Flow (16 Steps)

### Step 1: Create Instance
```cpp
Instance instance("Simple Button Example");
```
- **Public constructor**
- Initializes Vulkan and GLFW

### Step 2: Create Window and Surface
```cpp
Window* window = instance.createWindow("Click the Button!", {800, 600});
Surface* surface = window->addSurface();
window->plugInput();
```
- **Factory method**: `createWindow()` - Window constructor is PRIVATE
- Returns raw pointer (Instance owns the Window)
- Add surface and input manager via factory methods

### Step 3: Create Physical Device
```cpp
PhysDevice* phys_device = instance.createPhysDevice(surface);
```
- **Factory method**: `createPhysDevice()` - PhysDevice constructor is PRIVATE
- Selects GPU based on surface support
- Returns raw pointer (Instance owns the PhysDevice)

### Step 4: Create Logical Device
```cpp
std::unique_ptr<Device> device = phys_device->createLogicalDevice();
```
- **Factory method**: `createLogicalDevice()` - Device constructor is PRIVATE
- Returns `unique_ptr<Device>` (caller owns the Device)

### Step 5: Create Swapchain
```cpp
Swapchain* swapchain = device->createSwapchain();
```
- **Factory method**: `createSwapchain()` - Swapchain constructor is PRIVATE
- Returns raw pointer (Device owns the Swapchain)
- Automatically creates sync objects (semaphores, fences)

### Step 6: Create Queues
```cpp
Queue* graphics_queue = device->createQueue(vkb::QueueType::graphics);
Queue* present_queue = device->createQueue(vkb::QueueType::present);
```
- **Factory method**: `createQueue()` - Queue constructor is PRIVATE
- Returns raw pointer (Device owns the Queue)

### Step 7: Create Render Pass
```cpp
RenderPass render_pass(device.get(), swapchain);
```
- **Public constructor**
- Takes raw pointers to dependencies

### Step 8: Create Framebuffers
```cpp
vec<Framebuffer> framebuffers;
framebuffers.reserve(swapchain->images.size());
for (auto& image : swapchain->images)
{
    framebuffers.emplace_back(device.get(), &render_pass, swapchain, image);
}
```
- **Public constructor**
- One framebuffer per swapchain image
- Images are created automatically by Swapchain

### Step 9: Create Command Pool
```cpp
CommandPool command_pool(device.get(), vkb::QueueType::graphics);
command_pool.allocateCommandBuffers(swapchain->images.size());
```
- **Public constructor**
- Allocate command buffers after creation

### Step 10: Create Descriptor Resources
```cpp
DescriptorSetLayout desc_layout(device.get());
DescriptorPool desc_pool(device.get(), 10);  // Max 10 descriptor sets
```
- **Public constructors**
- Needed for texture binding in UI rendering

### Step 11: Create UI Renderer
```cpp
UIRenderer ui_renderer(device.get(), &render_pass, swapchain);
```
- **Public constructor**
- Handles batched rendering of UI elements

### Step 12: Load Shaders
```cpp
Shader ui_vert(device.get(), "shaders/ui_button.vert.spv");
Shader ui_frag(device.get(), "shaders/ui_button.frag.spv");

ui_renderer.createPipeline(ui_vert, ui_frag, desc_layout);
ui_renderer.createBuffers();
```
- **Public constructor**
- Shaders must be pre-compiled SPIR-V binaries
- Create pipeline and buffers after loading shaders

### Step 13: Load Texture
```cpp
LoadedImage button_img("textures/button.png");
Texture button_texture(device.get(), &command_pool, graphics_queue, button_img);
```
- **Public constructors**
- LoadedImage loads PNG from disk
- Texture uploads to GPU and creates sampler

### Step 14: Setup Button Textures
```cpp
ButtonTextures btn_textures;
btn_textures.idle = &button_texture;
btn_textures.hover = &button_texture;
btn_textures.press = &button_texture;
btn_textures.off = &button_texture;
```
- Can use same texture for all states or different textures

### Step 15: Add Button
```cpp
window->addButton(
    {300.0f, 250.0f},  // Bottom-left corner
    {500.0f, 350.0f},  // Top-right corner
    btn_textures,
    []() {
        std::cout << "Button clicked! Hello from TeapotLib!" << std::endl;
    }
);
```
- **Factory method**: `addButton()` - Button constructor is public but used via Window
- Callback is invoked when button is clicked

### Step 16: Main Render Loop
```cpp
while (!glfwWindowShouldClose(window->handle))
{
    glfwPollEvents();
    window->input->update();
    window->updateButtons();

    // Wait for fence
    device->device.waitForFences({swapchain->in_flight_fences[current_frame]}, true, UINT64_MAX);

    // Acquire image
    auto result = device->device.acquireNextImageKHR(
        swapchain->swapchain,
        UINT64_MAX,
        swapchain->available_semaphores[current_frame],
        nullptr
    );

    // Reset fence
    device->device.resetFences({swapchain->in_flight_fences[current_frame]});

    // Prepare and record
    ui_renderer.prepareFrame(window->getButtons(), *window, desc_pool, desc_layout);
    command_pool.recordUICommands(image_index, render_pass, framebuffers.data(), ui_renderer, *window);

    // Submit
    graphics_queue->handle.submit({submit_info}, swapchain->in_flight_fences[current_frame]);

    // Present
    present_queue->handle.presentKHR(present_info);

    current_frame = (current_frame + 1) % MAX_FRAMES;
}
```

## Key Concepts

### Ownership Model
- **Instance** owns: Windows, PhysicalDevices
- **Device** owns: Swapchain, Queues
- **Window** owns: Surface, InputManager, Buttons
- **Application** owns: Device (via unique_ptr)
- **Application stack**: RenderPass, Framebuffers, CommandPool, etc.

### Memory Management
- Factory methods return raw pointers - parent owns the object
- `createLogicalDevice()` returns `unique_ptr` - caller owns
- All Vulkan resources use RAII (auto cleanup in destructors)
- VMA handles all GPU memory allocations

### Synchronization Pattern
1. **Wait** for fence (CPU waits for GPU to finish previous frame)
2. **Acquire** next image with semaphore (GPU-GPU sync)
3. **Reset** fence for this frame
4. **Record** rendering commands
5. **Submit** with semaphores (wait for acquire, signal when done)
6. **Present** with semaphore (wait for rendering to finish)

## Common Mistakes

### ❌ WRONG: Direct construction of Window
```cpp
Window window("Title", {800, 600}, &instance);  // ERROR: Constructor is private!
```

### ✅ CORRECT: Use factory method
```cpp
Window* window = instance.createWindow("Title", {800, 600});
```

### ❌ WRONG: Direct construction of Queue
```cpp
Queue queue(&device, vkb::QueueType::graphics);  // ERROR: Constructor is private!
```

### ✅ CORRECT: Use factory method
```cpp
Queue* queue = device->createQueue(vkb::QueueType::graphics);
```

## Building

Add to your CMakeLists.txt:
```cmake
add_executable(SimpleButtonExample examples/simple_button_example.cpp)
target_link_libraries(SimpleButtonExample PRIVATE TeapotLib)
```

Build:
```bash
cmake --preset=x64-debug
cmake --build out/build/x64-debug
```

## Running

```bash
./out/build/x64-debug/SimpleButtonExample.exe
```

Ensure working directory contains:
- `textures/button.png`
- `shaders/ui_button.vert.spv`
- `shaders/ui_button.frag.spv`

## Customization

### Multiple Buttons
```cpp
window->addButton({100, 100}, {200, 150}, btn_textures, []() { /* callback 1 */ });
window->addButton({250, 100}, {350, 150}, btn_textures, []() { /* callback 2 */ });
window->addButton({400, 100}, {500, 150}, btn_textures, []() { /* callback 3 */ });
```

### Different Textures Per State
```cpp
LoadedImage idle_img("textures/button_idle.png");
LoadedImage hover_img("textures/button_hover.png");
LoadedImage press_img("textures/button_press.png");

Texture idle_tex(device.get(), &command_pool, graphics_queue, idle_img);
Texture hover_tex(device.get(), &command_pool, graphics_queue, hover_img);
Texture press_tex(device.get(), &command_pool, graphics_queue, press_img);

ButtonTextures btn_textures;
btn_textures.idle = &idle_tex;
btn_textures.hover = &hover_tex;
btn_textures.press = &press_tex;
btn_textures.off = &idle_tex;
```

## Troubleshooting

### Compilation Error: "Constructor is private"
- You're trying to directly construct an object with a private constructor
- Use the appropriate factory method instead (see "Correct API Usage" above)

### "Failed to load texture"
- Verify `textures/button.png` exists
- Check file format is PNG with RGBA channels

### "Failed to load shader"
- Verify `.spv` files exist in `shaders/` directory
- Shaders must be compiled to SPIR-V (not GLSL source)

### Button not responding
- Ensure `window->input->update()` is called before `window->updateButtons()`
- Check button coordinates are within window bounds
