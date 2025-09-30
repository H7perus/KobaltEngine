#pragma once


#include "cobaltcore.h"
#include "renderertypes/VertexInfo.h"

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <map>

#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"


CobaltEngineCore::CobaltEngineCore()
{
	Init("CobaltFPSTest");
	Renderer.Init(Window);
}

int CobaltEngineCore::Init(std::string WindowTitle)
{
	SDL_Init(SDL_INIT_VIDEO);
	Window = SDL_CreateWindow(
		WindowTitle.c_str(),
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_OPENGL
	);
	return 1;
}

void CobaltEngineCore::LoadModel(PBRMat* Material)
{
	VertexData<VertexSimple> vData;
	std::vector<uint32_t> iData;


	tinygltf::Model model;

	std::string err;
	std::string warn;


	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, "../assets/testscenes/greybox.gltf");
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "ERR: " << err << std::endl;
	}


	if (!res)
		std::cout << "Failed to load glTF: " << "../assets/testscenes/greybox.gltf" << std::endl;
	else
		std::cout << "Loaded glTF: " << "../assets/testscenes/greybox.gltf" << std::endl;

	for (const auto& attribute : model.meshes[0].primitives[0].attributes) {
		std::string attributeName = attribute.first; // e.g., "POSITION", "TEXCOORD_0", etc.
		std::cout << "Attribute available: " << attributeName << std::endl;
	}


	
	DrawInfoGroup DrawInfoLocal;
	for (auto& mesh : model.meshes)
	{
		for (tinygltf::Primitive& primitive : mesh.primitives)
		{
			int float_offset = 0;
			int float_size = 0;
			//index into list of possible attributes, we don't want to check the same value twice!
			int attribute_index = 0;
			std::vector < std::pair<std::string, uint32_t> > available_attributes;

			//See what attributes the mesh has(position, normal, uv, ...)
			for (std::pair<std::string, uint32_t>& option : DefaultVBOElements)
			{
				if (!primitive.attributes.count(option.first)) //do find instead and use that iterator?
				{
					continue;
				}
				if (option.first == "TEXCOORD_1") //do find instead and use that iterator?
				{
					continue;
				}
				available_attributes.push_back(option);
				float_size += option.second;
			}
			//Write that data into a buffer
			for (std::pair<std::string, uint32_t>& attribute : available_attributes)
			{

				int accessor_index = primitive.attributes[attribute.first];
				tinygltf::Accessor acc = model.accessors[accessor_index];
				tinygltf::BufferView bufferView = model.bufferViews[acc.bufferView];
				tinygltf::Buffer buffer = model.buffers[bufferView.buffer];//I am tired, boss

				if (float_offset == 0)
				{
					vData.vdata.resize(acc.count);
				}

				for (int i = 0; i < acc.count; i++)
					memcpy((float*)(vData.vdata.data() + i) + float_offset, buffer.data.data() + bufferView.byteOffset + i * attribute.second * sizeof(float), attribute.second * sizeof(float));

				float_offset += attribute.second;
			}



			//GET INDEX BUFFER
			tinygltf::Accessor indicesAccessor = model.accessors[primitive.indices];
			const auto& indexBufferView = model.bufferViews[indicesAccessor.bufferView];
			const auto& indexBuffer = model.buffers[indexBufferView.buffer];

			iData.resize(indexBufferView.byteLength / 2);

			for (int i = 0; i < indexBufferView.byteLength / 2; i++)
			{
				uint32_t index = *(uint16_t*)(indexBuffer.data.data() + indexBufferView.byteOffset + i * 2); // 18, 1, 325
				iData[i] = index;
			}

			PBRMat PerPrimMat = Renderer.GetMaterial(model, primitive.material);
			RenderObjectRequest RQ;

			PrimitiveDataBlock PrimitiveData;

			PrimitiveData.indexcount = iData.size();
			PrimitiveData.indexdata = iData.data();
			PrimitiveData.vertcount = vData.vdata.size() * 32; //TODO: Is the 32 a hardcoded per vert size? I think so?
			PrimitiveData.vertdata = vData.vdata.data();




			/*RQ.vertex_data = vData.vdata.data();
			RQ.vertex_size = vData.vdata.size() * 32;
			RQ.index_data = iData.data();
			RQ.num_indices = iData.size();
			RQ.data_pointer = &PerPrimMat;
			RQ.data_type = typeid(PerPrimMat);*/




			//std::shared_ptr<RenderObject> RO = Renderer.GetRenderObject(RQ);

			/*SingleDraw primitivedraw;


			primitivedraw.VertLocator.first_vert = RO->VBOfirst;

			primitivedraw.VertLocator.index_offset = RO->IndexFirst;
			primitivedraw.VertLocator.num_verts = RO->IndexCount;
			primitivedraw.materialindex = RO->data_index;
			primitivedraw.target_group = RO->DrawInfo.target_group;*/

			//DrawInfoLocal.DrawActions.push_back(primitivedraw);
		}
	}
		

	//RObjects.push_back(DrawInfoLocal);

	std::cout << "greybox has " << model.meshes[0].primitives[0].attributes["0"] << " floats of verts" << std::endl;
		

}

void CobaltEngineCore::LoadGLTFasScene(std::string path)
{
	std::vector<int> alreadyLoadedMeshes; //gotta see if we already loaded the mesh so we don't do double loading. Obvious improvement for instances of objects
	VertexData<VertexSimple> vData;
	std::vector<uint32_t> iData;

	std::filesystem::path fspath(path);
	std::unique_ptr<tinygltf::Model> modeldata = std::unique_ptr<tinygltf::Model>(new tinygltf::Model);

	tinygltf::Model &model = *modeldata;



	std::string err;
	std::string warn;

	if (fspath.extension() == ".gltf")
	{
		loader.LoadASCIIFromFile(modeldata.get(), &err, &warn, fspath.string());
	}
	else if (fspath.extension() == ".glb")
	{
		loader.LoadBinaryFromFile(modeldata.get(), &err, &warn, fspath.string());
	}
	else
		throw std::invalid_argument("not a valid file extension for GLTF files");

	if (!warn.empty()) {
		std::cout << "GLTF load warning: " << warn << std::endl;
	}
	if (!err.empty()) {
		std::cout << "GLTF load error: " << err << std::endl;
	}
	//model.nodes.resize(5);
	for (tinygltf::Node& node : model.nodes)
	{
		std::cout << node.name << std::endl;
		bool is_static = true;

		if (node.extras.Size())
		{
			if (node.extras.Has("TYPE")) // && node.extras.Get("TYPE") == tinygltf::Value("SHOOTABLETARGET")
			{
				is_static = false;
			}
		}
		DrawInfoGroup DrawInfoLocal;


		glm::mat4 transform(1);
		glm::dvec3 translation = glm::dvec3(0);
		glm::dquat rotation = glm::dquat();
		glm::dvec3 scale = glm::dvec3(0);

		int matbufferindex = Renderer.CPUModelMatrixBuffer.size();


		
		

		


		if (node.scale.size() == 3)
		{
			memcpy(&scale, node.scale.data(), 3 * 8);
			transform = glm::scale(transform, glm::vec3(scale.x, scale.z, scale.y));
		}


		

		if (node.rotation.size() == 4)
		{
			memcpy(&rotation, node.rotation.data(), 4 * 8);
			transform = glm::toMat4( glm::quat(-rotation.w, rotation.x, rotation.z, rotation.y)) * transform;
			glm::vec3 euler = glm::eulerAngles(glm::quat(rotation));
		}

		
		if (node.translation.size() == 3)
		{
			memcpy(&translation, node.translation.data(), 3 * 8);
			double intermediate = translation.z;
			translation.z = translation.y;
			translation.y = intermediate;


			transform = glm::translate(glm::mat4(1), glm::vec3(translation)) * transform;
		}

		Renderer.CPUModelMatrixBuffer.push_back(transform);


		if(node.mesh >= 0)
		for (tinygltf::Primitive& primitive : model.meshes[node.mesh].primitives)
		{
			int float_offset = 0;
			int float_size = 0;
			int attribute_index = 0;
			std::vector < std::pair<std::string, uint32_t> > available_attributes;

			//See what attributes the mesh has(position, normal, uv, ...)
			for (std::pair<std::string, uint32_t>& option : DefaultVBOElements)
			{
				if (!primitive.attributes.count(option.first)) //do find instead and use that iterator?
				{
					continue;
				}
				if (option.first == "TEXCOORD_1") //do find instead and use that iterator?
				{
					continue;
				}
				available_attributes.push_back(option);
				float_size += option.second;
			}
			//Write that data into a buffer
			for (std::pair<std::string, uint32_t>& attribute : available_attributes)
			{

				int accessor_index = primitive.attributes[attribute.first];
				tinygltf::Accessor acc = model.accessors[accessor_index];
				tinygltf::BufferView bufferView = model.bufferViews[acc.bufferView];
				tinygltf::Buffer buffer = model.buffers[bufferView.buffer];//I am tired, boss

				if (float_offset == 0)
				{
					vData.vdata.resize(acc.count);
				}


				//std::vector<float> testout(100);

				//memcpy(testout.data(), buffer.data.data() + bufferView.byteOffset + acc.byteOffset, 400);

				
					
				if(attribute.first == "POSITION" || attribute.first == "NORMAL" || attribute.first == "TANGENT")
					for (int i = 0; i < acc.count; i++)
					{
						memcpy((float*)(vData.vdata.data() + i) + float_offset, buffer.data.data() + bufferView.byteOffset + acc.byteOffset + i * attribute.second * sizeof(float), sizeof(float));

						memcpy((float*)(vData.vdata.data() + i) + float_offset + 1, buffer.data.data() + bufferView.byteOffset + acc.byteOffset + i * attribute.second * sizeof(float) + 8, sizeof(float));

						memcpy((float*)(vData.vdata.data() + i) + float_offset + 2, buffer.data.data() + bufferView.byteOffset + acc.byteOffset + i * attribute.second * sizeof(float) + 4, sizeof(float));
					}
				else
					for (int i = 0; i < acc.count; i++)
					{
						void* dest = (float*)(vData.vdata.data() + i) + float_offset;

						void* src = buffer.data.data() + bufferView.byteOffset + acc.byteOffset + i * attribute.second * sizeof(float);

						int size = attribute.second * sizeof(float);

						memcpy(dest, src, size);
					}

				float_offset += attribute.second;
			}



			//GET INDEX BUFFER
			tinygltf::Accessor indicesAccessor = model.accessors[primitive.indices];
			const auto& indexBufferView = model.bufferViews[indicesAccessor.bufferView];
			const auto& indexBuffer = model.buffers[indexBufferView.buffer];

			iData.resize(indicesAccessor.count);

			for (int i = 0; i < indicesAccessor.count; i++)
			{
				uint32_t indexindex = i;

				if (i % 3)
					indexindex = (i - i % 3) + 3 - i % 3;

				uint32_t index;
				if(indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
					index = *(uint16_t*)(indexBuffer.data.data() + indexBufferView.byteOffset + indicesAccessor.byteOffset + indexindex * 2); // 18, 1, 325
				if (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
					index = *(uint32_t*)(indexBuffer.data.data() + indexBufferView.byteOffset + indicesAccessor.byteOffset + indexindex * 4); // 18, 1, 325
				iData[i] = index;
			}

			tinygltf::Material& gltfMat = model.materials[primitive.material];


			
			

			//RenderObjectRequest RQ;

			PrimitiveDataBlock PrimitiveData;
			PBRMat PerPrimMat = Renderer.GetMaterial(model, primitive.material);
			PrimitiveData.materialindex = Renderer.SubmitMaterial(PerPrimMat);


			PrimitiveData.indexcount = iData.size();
			PrimitiveData.indexdata = iData.data();
			PrimitiveData.vertcount = vData.vdata.size(); //TODO: Is the 32 a hardcoded per vert size? I think so?
			PrimitiveData.vertdata = vData.vdata.data();

			PrimitiveData.vertformat = DefaultVBOElements;
			SingleDraw primitivedraw;
			primitivedraw.materialindex = PrimitiveData.materialindex;
			primitivedraw.VertLocator = Renderer.SubmitPrimitive(PrimitiveData);

			primitivedraw.modelmatindex = matbufferindex;

			DrawInfoLocal.DrawActions.push_back(primitivedraw);


			/*RQ.vertex_data = vData.vdata.data();
			RQ.vertex_size = vData.vdata.size() * 32;
			RQ.index_data = iData.data();
			RQ.num_indices = iData.size();
			RQ.data_pointer = &PerPrimMat;
			RQ.data_type = typeid(PerPrimMat);*/

			//std::shared_ptr<RenderObject> RO = Renderer.GetRenderObject(RQ);

			//SingleDraw primitivedraw;
			//

			//primitivedraw.first_vert = RO->VBOfirst;

			//primitivedraw.index_offset = RO->IndexFirst;
			//primitivedraw.num_verts = RO->IndexCount;
			//primitivedraw.materialindex = RO->data_index;
			//primitivedraw.modelmatindex = matbufferindex; //size is already updated here, think about something
			//primitivedraw.target_group = RO->location;

			//DrawInfoLocal.DrawActions.push_back(primitivedraw);
		}
		Renderer.DrawInfoGroups.push_back(std::make_shared<DrawInfoGroup>(DrawInfoLocal));

		//RObjects.push_back(DrawInfoLocal);
	}
	

}