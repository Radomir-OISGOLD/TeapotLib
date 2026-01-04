
#include "Teapot/image.hpp"
#include "Teapot/rendering.hpp"

namespace Teapot
{

    Image::Image(VkImage handle, VkImageView view) :
		handle(handle), view(view)
    {
        
    }

    Image::~Image()
    {

    }

}

Teapot::LoadedImage::LoadedImage(const char* path)
{
	LoadedImage img{};

	stbi_uc* data = stbi_load(path, &img.width, &img.height, &img.channels, STBI_rgb_alpha);
	if (!data)
		throw std::runtime_error("Failed to load image");

	img.channels = 4;
	img.pixels.resize(img.width * img.height * 4);
	memcpy(img.pixels.data(), data, img.pixels.size());

	stbi_image_free(data);
	return img;
}
