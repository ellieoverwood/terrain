#include "platform.h"
#include "runtime_manager.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <vector>

#include "../shared/debug.h"

SDL_Window* window;
SDL_GLContext sdl_context;

namespace platform {

void (*keypress[128]) (double);
void (*keydown[128]) (double);

std::vector<char> keys_down;

void on_keypress(unsigned char key, void (*func) (double)) {
	keypress[key] = func;
}

void on_keydown(unsigned char key, void (*func) (double)) {
	keydown[key] = func;
}

#define SDL_ERR(msg) {\
	fprintf(stderr, msg": %s\n", SDL_GetError());\
}\

void terminate() {
    SDL_GL_DeleteContext(sdl_context);
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();
	exit(0);
}

void input(double delta_time) {
	SDL_PumpEvents();

	SDL_Event event;

    while(SDL_PollEvent(&event) != 0) {
    	switch(event.type) {
    		case SDL_QUIT: terminate();
    		case SDL_KEYDOWN: {
    			if (event.key.repeat) break;
			if (event.key.keysym.sym > 255) break;

    			void (*func)(double) = keypress[event.key.keysym.sym];
    			if (func) func(delta_time);

    			func = keydown[event.key.keysym.sym];
    			if (func) {
    				for (unsigned char i : keys_down) {
    					if ((int)i == (int)event.key.keysym.sym) goto break_out_of_switch;
    				}
    				keys_down.push_back((unsigned char)event.key.keysym.sym);
    			}
    			break_out_of_switch:
    			break;
    		}
	    	case SDL_KEYUP: {
			if (event.key.keysym.sym > 255) break;
	    		unsigned char key = event.key.keysym.sym;
	    		for (int i = 0; i < (int)keys_down.size(); i ++) {
	    			if (key == keys_down[i]) {
	    				keys_down.erase(keys_down.begin() + i);
	    				break;
	    			}
	    		}
	    		break;
	    	}
    		case SDL_WINDOWEVENT: {
    			if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
    				runtime::on_resize(event.window.data1, event.window.data2, delta_time);
    			}
    			break;
    		}
    	}
    }

    for (unsigned char c : keys_down) {
    	keydown[c](delta_time);
    }

    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    if (x || y) runtime::on_mouse(x, y, delta_time);
}

void init(int opengl_version_major, int opengl_version_minor, const char* window_title, int window_width, int window_height, int window_min_width, int window_min_height) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) SDL_ERR("failed to intialize video")

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, opengl_version_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, opengl_version_minor);

    window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) SDL_ERR("failed to create window")

    SDL_SetWindowMinimumSize(window, window_min_width, window_min_height);

	sdl_context = SDL_GL_CreateContext(window);
	if (!sdl_context) SDL_ERR("failed to create OpenGL sdl_context")
	
	if (SDL_GL_SetSwapInterval(1) < 0) SDL_ERR("failed to set vsync")
	//SDL_ShowCursor(SDL_FALSE);
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

long ticks() {
	return SDL_GetTicks();
}

char* read_file(char* src) {
	char * buffer = 0;
	long length;
	FILE * f = fopen (src, "rb");

	if (f)
	{
	  fseek (f, 0, SEEK_END);
	  length = ftell (f);
	  fseek (f, 0, SEEK_SET);
	  buffer = (char*)malloc(length + 1);
	  if (buffer)
	  {
	    fread (buffer, 1, length, f);
	  }
	  fclose (f);
	}

	buffer[length] = EOF;

	DEBUG_OK("loaded %s", src);
	return buffer;
}

void swap_buffers() {
	SDL_GL_SwapWindow(window);
}
}
