

#include "Teapot/instance.hpp"

namespace Teapot
{

	Instance::Instance(const char* app_name)
	{
		vkb::InstanceBuilder builder;

		auto inst_ret = builder.set_app_name(app_name).request_validation_layers().use_default_debug_messenger().build();

		if (!inst_ret) {
			std::cerr << "Failed to create instance. Error: " << inst_ret.error().message() << "\n";
			delete this;
			return;
		}

		handle = inst_ret.value();
	}

	Instance::~Instance()
	{
		vkb::destroy_instance(handle);
	}

}

