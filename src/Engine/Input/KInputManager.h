//#pragma once
//#include <queue>
//#include <mutex>
//#include "KInputSet.h"
//#include "imgui.h"
//#include "imgui_impl_sdl3.h"
//#include "SDL3/SDL.h"
//
//
////This will collect input and categorize it for further use.
//class KInputManager
//{
//public:
//	//std::lock_guard<std::mutex> queue_lock;
//    KInputSet persistentInputs;
//	std::queue<KInputSet> inputs;
//	
//	KInputSet GetInput(SDL_Window* Window)
//	{
//        //SDL_SetWindowRelativeMouseMode(Window, true);
//
//        const bool *keypresses;
//
//        KInputSet return_set;
//        SDL_Event e;
//        while (SDL_PollEvent(&e)) {
//            /* user has pressed a key? */
//            ImGui_ImplSDL3_ProcessEvent(&e);
//            /*if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
//                should_run = 0;
//                break;
//            }*/
//            //if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP) {
//            //    //return_set.buttonstates.set(e.key.scancode, e.key.down);
//            //}
//            //if (e.type == SDL_EVENT_MOUSE_MOTION)
//            //{
//            //    return_set.mousedelta.x += e.motion.xrel;
//            //    return_set.mousedelta.y += e.motion.yrel;
//            //}
//        }
//
//        float x, y;
//
//        SDL_GetRelativeMouseState(&x, &y);
//
//        return_set.mousedelta = glm::ivec2(x, y);
//
//        keypresses = SDL_GetKeyboardState(nullptr);
//
//        for (int i = 0; i < SDL_SCANCODE_COUNT; i++)
//        {
//            return_set.buttonstates.set(i, keypresses[i]);
//        }
//
//        return return_set;
//	}
//
//};
//
