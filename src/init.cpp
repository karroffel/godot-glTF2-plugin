/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Godot.hpp>

#include "gltf2plugin.hpp"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

extern "C" void GDN_EXPORT godot_gltf_plugin_gdnative_init(
                const godot_gdnative_init_options *options)
{
	godot::Godot::gdnative_init((godot_gdnative_init_options *) options);
}

extern "C" void GDN_EXPORT godot_gltf_plugin_gdnative_terminate(
                const godot_gdnative_terminate_options *options)
{
	godot::Godot::gdnative_terminate((godot_gdnative_terminate_options *) options);
}

extern "C" void GDN_EXPORT godot_gltf_plugin_nativescript_init(void *handle)
{
	godot::Godot::nativescript_init((void *) handle);
	
	godot::register_class<Gltf2Plugin>();
}
