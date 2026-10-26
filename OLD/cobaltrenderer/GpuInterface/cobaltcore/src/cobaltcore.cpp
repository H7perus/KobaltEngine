#pragma once


#include "cobaltcore.h"
#include "renderertypes/VertexInfo.h"

#include "DevUI.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <map>

#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"


#include "renderertypes/RenderGroup.h"
#include "cobaltrenderer.h"
#include "KRay.h"
#include "renderertypes/UniformCommons.h"
#include "renderertypes/Material.h"

KEngineCore::KEngineCore()
{
	Init("CobaltFPSTest");
	//Renderer.Init(Window);
}

int KEngineCore::Init(std::string WindowTitle)
{
	SDL_Init(SDL_INIT_VIDEO);
	Window = SDL_CreateWindow(
		WindowTitle.c_str(),
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_OPENGL
	);

	Renderer.Init(Window);
	Renderer.Setup();

	clock.Update();
	lastFrame.timeNS = clock.time;

	DevUI MainUI(Window, &Renderer.Context);
	MainUI.InitOpenGL();

	return 1;
}


void KEngineCore::RunFrame()
{
	float frametime = clock.Update();
	ImGui_ImplSDL3_NewFrame();
	gamestate.inputState = input_manager.GetInput(Window);

	//This will probably be threaded away

	gamestate.yaw += gamestate.inputState.mousedelta.x * 0.022 * 1;
	gamestate.pitch -= gamestate.inputState.mousedelta.y * 0.022 * 1;

	gamestate.yaw = fmod(gamestate.yaw + 180.f, 360.f) - 180.f;
	gamestate.yaw = fmod(gamestate.yaw - 180.f, 360.f) + 180.f;
	gamestate.pitch = glm::clamp(gamestate.pitch, -89.f, 89.f);
	//std::cout << "Mouse movement: " << gamestate.yaw << " " << gamestate.pitch << std::endl;


	int numticks = (clock.time - lastFrame.timeNS) / (1000000000 / gamestate.tickrate);

	for(numticks; numticks > 0; numticks--)
	{
		gamestate.StepAhead(1 / gamestate.tickrate);

		lastFrame.timeNS += 1000000000 / gamestate.tickrate;
	}
	
	if (clock.time - lastFrame.timeNS > 1000000000 / gamestate.tickrate * 4)
	{
		lastFrame.timeNS = clock.time;
	}

	
	UniformCommons Uniforms;

	float fraction = uint64_t(clock.time - lastFrame.timeNS) / (1000000000 / gamestate.tickrate);
	
	glm::vec3 position = glm::mix(gamestate.oldpos, gamestate.pos, fraction);

	glm::vec3 viewdir = glm::vec3(sin(glm::radians(gamestate.yaw)) * cos(glm::radians(gamestate.pitch)), cos(glm::radians(gamestate.yaw)) * cos(glm::radians(gamestate.pitch)), sin(glm::radians(gamestate.pitch)));

	KRay ray;
	ray.origin = position;
	ray.direction = viewdir;

	glm::vec3 collidepoint = glm::vec3(std::numeric_limits<float>::infinity());
	
	
	/*for (int i = 0; i < gamestate.vertindices.size() / 3; i++)
	{
		KCollideTriangle tri;
		tri.a = gamestate.verts[gamestate.vertindices[3 * i]];
		tri.b = gamestate.verts[gamestate.vertindices[3 * i + 1]];
		tri.c = gamestate.verts[gamestate.vertindices[3 * i + 2]];
		float t;
		glm::vec3 collidepointtemp = glm::vec3(std::numeric_limits<float>::infinity());


		ray.intersectTri(tri, collidepointtemp, collidepointtemp, t);

		if (glm::distance(collidepointtemp, position) < glm::distance(collidepoint, position))
		{
			collidepoint = collidepointtemp;
		}
	}*/
	



	glm::mat4 translate = glm::translate(glm::mat4(1.0), -position);
	glm::mat4 perspective = glm::perspective(glm::radians(70.f), SCREEN_WIDTH / float(SCREEN_HEIGHT), 0.01f, 1000.f);
	glm::mat4 pos = perspective * glm::lookAt(glm::vec3(0), viewdir, glm::vec3(0, 0, 1)) * translate;

	Uniforms.Transform = pos;

	Renderer.StartMainPass();

	Renderer.UBO.Data(sizeof(Uniforms), &Uniforms);


	Renderer.ModelMatrixBuffer.Data(sizeof(glm::mat4) * Renderer.CPUModelMatrixBuffer.size(), Renderer.CPUModelMatrixBuffer.data());
	Renderer.ModelMatrixBuffer.BindToBaseSSBO(3);

	Renderer.Draw();
	Renderer.DrawWireframes(Renderer.BBGroups[4].first, Renderer.BBGroups[4].second);
	ImGui_ImplOpenGL3_NewFrame();


	ImGui::NewFrame();
	ImGui::Begin("Demo window");

	ImGui::Checkbox("debugging bool", &test);
	ImGui::Checkbox("noclip", &gamestate.noclip);

	ImGui::Text((std::string("on ground: ") + std::to_string(gamestate.isonground)).c_str());

	ImGui::Text(std::to_string(gamestate.pos.z).c_str());


	if (ImGui::Button("Hotreload shaders"))
		Renderer.shader.reload();

	ImGui::SliderFloat("normalmix", &normalmix, 0.0, 1.0);
	ImGui::SliderFloat("tickrate", &gamestate.tickrate, 1, 128);

	if (ImGui::Button("save position"))
	{
		gamestate.savedPos = gamestate.pos;
		gamestate.savedyaw = gamestate.yaw;
		gamestate.savedpitch = gamestate.pitch;
	}
		
	if (ImGui::Button("apply position"))
	{
		gamestate.pos = gamestate.savedPos;
		gamestate.yaw = gamestate.savedyaw;
		gamestate.pitch = gamestate.savedpitch;
	}
		

	ImGui::Text(std::to_string(glm::distance(position, collidepoint)).c_str());

	ImGui::SliderInt("Texture Index", &texindex, 0, Renderer.Textures.size() - 1);


	ImGui::Image((ImTextureID)(intptr_t)Renderer.Textures[texindex]->ID, ImVec2(256, 256));
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	Renderer.shader.setBool("debugbool", test);
	Renderer.shader.setFloat("normalmix", normalmix);

	//MainUI.DrawUI();
	SDL_GL_SwapWindow(Renderer.Window);
}


void KEngineCore::LoadModel(PBRMat* Material)
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

void KEngineCore::LoadGLTFasPhysics(std::string path)
{
	std::vector<int> alreadyLoadedMeshes; //gotta see if we already loaded the mesh so we don't do double loading. Obvious improvement for instances of objects
	VertexData<VertexSimple> vData;
	std::vector<uint32_t> iData;

	std::filesystem::path fspath(path);
	std::unique_ptr<tinygltf::Model> modeldata = std::unique_ptr<tinygltf::Model>(new tinygltf::Model);

	tinygltf::Model& model = *modeldata;



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
		//std::cout << node.name << std::endl;
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
			transform = glm::toMat4(glm::quat(-rotation.w, rotation.x, rotation.z, rotation.y)) * transform;
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

		if (node.mesh >= 0)
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

				int vertexoffsetforindices = gamestate.verts.size();
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

					if (attribute.first == "POSITION")
						for (int i = 0; i < acc.count; i++)
						{
							memcpy((float*)(vData.vdata.data() + i) + float_offset, buffer.data.data() + bufferView.byteOffset + acc.byteOffset + i * attribute.second * sizeof(float), sizeof(float));
							memcpy((float*)(vData.vdata.data() + i) + float_offset + 1, buffer.data.data() + bufferView.byteOffset + acc.byteOffset + i * attribute.second * sizeof(float) + 8, sizeof(float));
							memcpy((float*)(vData.vdata.data() + i) + float_offset + 2, buffer.data.data() + bufferView.byteOffset + acc.byteOffset + i * attribute.second * sizeof(float) + 4, sizeof(float));

							if (attribute.first == "POSITION")
								gamestate.verts.push_back(glm::vec3(transform * glm::vec4(vData.vdata[i].position, 1.0)));
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
					if (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
						index = *(uint16_t*)(indexBuffer.data.data() + indexBufferView.byteOffset + indicesAccessor.byteOffset + indexindex * 2); // 18, 1, 325
					if (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
						index = *(uint32_t*)(indexBuffer.data.data() + indexBufferView.byteOffset + indicesAccessor.byteOffset + indexindex * 4); // 18, 1, 325
					iData[i] = index;
					gamestate.vertindices.push_back(index + vertexoffsetforindices);
				}
			}

	}
	if (gamestate.tris.size() == 0)
		for (int i = 0; i < gamestate.vertindices.size() / 3; i++)
		{
			KCollideTriangle tri;
			tri.a = gamestate.verts[gamestate.vertindices[3 * i]];
			tri.b = gamestate.verts[gamestate.vertindices[3 * i + 1]];
			tri.c = gamestate.verts[gamestate.vertindices[3 * i + 2]];

			tri.AABBA = glm::min(tri.a, tri.b, tri.c);
			tri.AABBB = glm::max(tri.a, tri.b, tri.c);

			tri.normal = normalize(cross(tri.b - tri.a, tri.c - tri.a));
			gamestate.tris.push_back(tri);
		}
}

void KEngineCore::LoadGLTFasScene(std::string path)
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
		//std::cout << node.name << std::endl;
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

			int vertexoffsetforindices = gamestate.verts.size();
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

						if (attribute.first == "POSITION")
							gamestate.verts.push_back(glm::vec3(transform * glm::vec4(vData.vdata[i].position, 1.0)));
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
				gamestate.vertindices.push_back(index + vertexoffsetforindices);
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

	std::vector<LineSegment> locallines;

	if(gamestate.tris.size() == 0)
		for (int i = 0; i < gamestate.vertindices.size() / 3; i++)
		{
			KCollideTriangle tri;
			tri.a = gamestate.verts[gamestate.vertindices[3 * i]];
			tri.b = gamestate.verts[gamestate.vertindices[3 * i + 1]];
			tri.c = gamestate.verts[gamestate.vertindices[3 * i + 2]];
			tri.AABBA = glm::min(tri.a, tri.b, tri.c);
			tri.AABBB = glm::max(tri.a, tri.b, tri.c);

			KBvhNode node;

			node.bb.a = tri.AABBA;
			node.bb.b = tri.AABBB;

			gamestate.BvhNodes.push_back(node);

			std::vector<LineSegment> boy;

			boy = node.bb.GetWireframeLines(glm::vec3(0.f, 0.8f, 0.8f));

			locallines.insert(locallines.end(), boy.begin(), boy.end());

			tri.normal = normalize(cross(tri.b - tri.a, tri.c - tri.a));
			gamestate.tris.push_back(tri);
		}
	//Renderer.AddLineSet(locallines);


	gamestate.BuildCollideBvh();
	locallines.clear();
	for (std::pair<uint32_t, uint32_t> layer : gamestate.BvhLayers)
	{
		locallines.clear();
		for (int i = layer.first; i < layer.first + layer.second; i++)
		{

			std::vector<LineSegment> boy;

			boy = gamestate.BvhNodes[i].bb.GetWireframeLines(glm::vec3(0.0, 0.8, 0.8));
			locallines.insert(locallines.end(), boy.begin(), boy.end());
		}
		Renderer.AddLineSet(locallines);
	}

	Renderer.SetupLines(gamestate.verts, gamestate.vertindices);
}