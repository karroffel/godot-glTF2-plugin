/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gltf2plugin.hpp"

#include <cstring>

using namespace godot;

void Gltf2Plugin::_register_methods()
{
	register_method("load_file", &Gltf2Plugin::load_file);
}

void Gltf2Plugin::_init()
{
}

Dictionary Gltf2Plugin::load_file(String file_path)
{
	tinygltf::Model model;
	
	std::string err;
	
	std::string filename = file_path.utf8().get_data();
	
	Dictionary gltf_file;
	
	gltf_file["filename"] = file_path;
	gltf_file["valid"] = false;

	bool ret = false;

	if (filename.substr(filename.size() - 5) == ".gltf") {
		ret = tinygltf::TinyGLTF().LoadASCIIFromFile(&model, &err, filename);

	} else if (filename.substr(filename.size() - 4) == ".glb") {
		ret = tinygltf::TinyGLTF().LoadBinaryFromFile(&model, &err, filename);
	} else {
		Godot::print_error("Can only load glb and gltf files!", "load_file", "gltf2plugin.cpp", __LINE__);
		return gltf_file;
	}
	
	if (!ret) {
		Godot::print("Could not parse glTF file: " + String(err.c_str()));
		return gltf_file;
	}
	
	if (model.meshes.size() < 1) {
		Godot::print("No mesh found in glTF file");
		return gltf_file;
	}
	
	gltf_file["valid"] = true;
	
	Dictionary meshes;
	
	for (auto gltf_mesh : model.meshes) {
	
		Ref<ArrayMesh> mesh = new ArrayMesh;
		
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
				
				auto accessor = model.accessors[surface.indices];
				
				auto buffer_view = model.bufferViews[accessor.bufferView];
				
				std::vector<unsigned char> data = model.buffers[buffer_view.buffer].data;
				
				size_t num_indicies = accessor.count;
				
				indices.resize(num_indicies);
				
				{
					auto write = indices.write();
					
					uint8_t *buffer = (uint8_t *) data.data() + buffer_view.byteOffset;
					
					switch (accessor.componentType) {
					
					case TINYGLTF_COMPONENT_TYPE_BYTE: {
						for (size_t i = 0; i < num_indicies; i++) {
							write[i] = *(int8_t *) &buffer[i * sizeof(int8_t)];
						}
					} break;
						
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
						for (size_t i = 0; i < num_indicies; i++) {
							write[i] = *(uint8_t *) &buffer[i * sizeof(uint8_t)];
						}
					} break;
						
					case TINYGLTF_COMPONENT_TYPE_SHORT: {
						for (size_t i = 0; i < num_indicies; i++) {
							write[i] = *(int16_t *) &buffer[i * sizeof(int16_t)];
						}
					} break;
						
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
						for (size_t i = 0; i < num_indicies; i++) {
							write[i] = *(uint16_t *) &buffer[i * sizeof(uint16_t)];
						}
					} break;
						
					case TINYGLTF_COMPONENT_TYPE_INT: {
						for (size_t i = 0; i < num_indicies; i++) {
							write[i] = *(int32_t *) &buffer[i * sizeof(int32_t)];
						}
					} break;
						
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
						for (size_t i = 0; i < num_indicies; i++) {
							write[i] = *(uint32_t *) &buffer[i * sizeof(uint32_t)];
						}
					} break;
					default:
						break;
					}
					
				}
				
				arrays[ArrayMesh::ARRAY_INDEX] = indices;
			}
			
			// vertices
			if (surface.attributes.count("POSITION")) {
				PoolVector3Array vertices;
				
				auto accessor = model.accessors[surface.attributes["POSITION"]];
				
				auto buffer_view = model.bufferViews[accessor.bufferView];
				
				std::vector<unsigned char> data = model.buffers[buffer_view.buffer].data;
				
				size_t num_vertices = accessor.count;
				
				size_t stride = buffer_view.byteStride;

				if (stride == 0) {
					stride = sizeof(Vector3);
				}

				vertices.resize(num_vertices);
				
				{
					auto write = vertices.write();
					
					uint8_t *buffer = (uint8_t *) data.data() + buffer_view.byteOffset;
					
					for (size_t i = 0; i < num_vertices; i++) {
						Vector3 position = *(Vector3 *) &buffer[i * stride];
						write[i] = position;
					}
				}
				
				arrays[ArrayMesh::ARRAY_VERTEX] = vertices;
			}
			
			if (surface.attributes.count("NORMAL")) {
				PoolVector3Array normals;
				
				auto accessor = model.accessors[surface.attributes["NORMAL"]];
				
				auto buffer_view = model.bufferViews[accessor.bufferView];
				
				std::vector<unsigned char> data = model.buffers[buffer_view.buffer].data;
				
				size_t num_normals = accessor.count;
				
				size_t stride = buffer_view.byteStride;

				if (stride == 0) {
					stride = sizeof(Vector3);
				}

				normals.resize(num_normals);
				
				{
					auto write = normals.write();
					
					uint8_t *buffer = (uint8_t *) data.data() + buffer_view.byteOffset;
					
					for (size_t i = 0; i < num_normals; i++) {
						Vector3 normal = *(Vector3 *) &buffer[i * stride];
						write[i] = -normal;
					}
				}
				
				arrays[ArrayMesh::ARRAY_NORMAL] = normals;
			}

			if (surface.attributes.count("TEXCOORD_0")) {
				PoolVector2Array uvs;
				
				auto accessor = model.accessors[surface.attributes["TEXCOORD_0"]];
				
				auto buffer_view = model.bufferViews[accessor.bufferView];
				
				std::vector<unsigned char> data = model.buffers[buffer_view.buffer].data;
				
				size_t num_uvs = accessor.count;
				
				size_t stride = buffer_view.byteStride;

				if (stride == 0) {
					stride = sizeof(Vector2);
				}

				uvs.resize(num_uvs);
				
				{
					auto write = uvs.write();
					
					uint8_t *buffer = (uint8_t *) data.data() + buffer_view.byteOffset;
					
					for (size_t i = 0; i < num_uvs; i++) {
						Vector2 uv = *(Vector2 *) &buffer[i * stride];
						write[i] = uv;
					}
				}
				
				arrays[ArrayMesh::ARRAY_TEX_UV] = uvs;
			}
			
			mesh->add_surface_from_arrays(primitive, arrays);
		}
		
		meshes[String(gltf_mesh.name.c_str())] = mesh;
	}
	
	gltf_file["meshes"] = meshes;
	
	return gltf_file;
}
