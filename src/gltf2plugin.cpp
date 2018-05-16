/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gltf2plugin.hpp"

#include <cstring>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

using namespace godot;

void Gltf2Plugin::_register_methods()
{
	register_method("load_gltf2_file", &Gltf2Plugin::load_gltf2_file);
}

void Gltf2Plugin::_init()
{
	Godot::print("Gltf2Plugin says hello!");
}

Ref<ArrayMesh> Gltf2Plugin::load_gltf2_file(String file_path)
{
	Godot::print(String("loading file: ") + file_path);
	
	tinygltf::Model model;
	
	std::string err;
	
	std::string filename = file_path.utf8().get_data();
	
	bool ret = loader.LoadASCIIFromFile(&model, &err, filename);
	
	if (!err.empty()) {
		Godot::print(String(err.c_str()));
		return Ref<ArrayMesh>();
	}
	
	if (!ret) {
		Godot::print("Could not parse glTF file");
		return Ref<ArrayMesh>();
	}
	
	if (model.meshes.size() < 1) {
		Godot::print("No mesh found in glTF file");
		return Ref<ArrayMesh>();
	}
	
	Ref<ArrayMesh> mesh = new ArrayMesh;
	
	tinygltf::Mesh &gltf_mesh = model.meshes[0];
	
	for (auto surface : gltf_mesh.primitives) {
		
		int primitive = 0;
		
		switch (surface.mode) {
		case TINYGLTF_MODE_TRIANGLES:
			primitive = ArrayMesh::PRIMITIVE_TRIANGLES;
			break;
		case TINYGLTF_MODE_TRIANGLE_FAN:
			primitive = ArrayMesh::PRIMITIVE_TRIANGLE_FAN;
			break;
		case TINYGLTF_MODE_LINE:
			primitive = ArrayMesh::PRIMITIVE_LINES;
			break;
		default:
			Godot::print("unimplemented surface mode");
			continue;
		}
		
		Array arrays;
		
		arrays.resize(ArrayMesh::ARRAY_MAX);
		
		// indices
		{
			PoolIntArray indices;
			
			int buffer_view_idx = model.accessors[surface.indices].bufferView;
			
			auto buffer_view = model.bufferViews[buffer_view_idx];
			
			std::vector<unsigned char> data = model.buffers[buffer_view.buffer].data;
			
			// TODO do actual copying
			
			arrays[ArrayMesh::ARRAY_INDEX] = indices;
		}
		
		// vertices
		if (surface.attributes.count("POSITION")) {
			PoolVector3Array vertices;
			
			int buffer_view_idx = model.accessors[surface.attributes["POSITION"]].bufferView;
			
			auto buffer_view = model.bufferViews[buffer_view_idx];
			
			std::vector<unsigned char> data = model.buffers[buffer_view.buffer].data;
			
			// TODO do actual copying
			
			arrays[ArrayMesh::ARRAY_VERTEX] = vertices;
		}
		
		if (surface.attributes.count("NORMAL")) {
			// TODO
		}
		
		if (surface.attributes.count("TEXCOORD_0")) {
			// TODO
		}
		
		mesh->add_surface_from_arrays(primitive, arrays);
	}
	
	return mesh;
}
