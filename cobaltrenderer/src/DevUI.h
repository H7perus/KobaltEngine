#pragma once


#include "SDL2/SDl.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
class DevUI
{
public:
	SDL_Window *window;
	SDL_GLContext* sdl_context;

	DevUI(SDL_Window* window, SDL_GLContext* sdl_context) : window(window), sdl_context(sdl_context) {}

	void InitOpenGL()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		ImGui_ImplSDL2_InitForOpenGL(window, sdl_context);
		ImGui_ImplOpenGL3_Init("#version 130");
		ImGui::StyleColorsDark();

		/*ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();*/
	}

	void DrawUI()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Demo window");
		bool test = true;
		ImGui::Checkbox("debugging bool", &test);
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	}

};

