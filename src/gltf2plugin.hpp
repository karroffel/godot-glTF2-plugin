/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef GLTF2PLUGIN_HPP
#define GLTF2PLUGIN_HPP

#include <Godot.hpp>

#include <Reference.hpp>
#include <ArrayMesh.hpp>

#include "tiny_gltf.h"

class Gltf2Plugin : public godot::GodotScript<godot::Reference> {
	GODOT_CLASS(Gltf2Plugin)
public:
	
	static void _register_methods();
	
	void _init();
	
	godot::Dictionary load_file(godot::String file_path);
};

#endif // GLTF2PLUGIN_HPP
