
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
