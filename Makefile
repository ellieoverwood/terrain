files = main.cc shared/context.cc gentime/erosion.cc gentime/gentime.cc gentime/perlin.cc runtime/platform.cc runtime/runtime.cc runtime/runtime_manager.cc runtime/shader.cc runtime/terrain.cc runtime/floating_camera.cc shared/debug.cc runtime/water.cc shared/cmd_args.cc shared/serialize.cc shared/fileutil.cc runtime/dev.cc
imgui_files = include/imgui/imgui.cpp include/imgui/imgui_demo.cpp include/imgui/imgui_draw.cpp include/imgui/imgui_tables.cpp include/imgui/imgui_widgets.cpp include/imgui/backends/imgui_impl_sdl2.cpp include/imgui/backends/imgui_impl_opengl3.cpp
flags = -I/usr/include/SDL2 -lSDL2 -lGL -Wall -Wpedantic -DGL_GLEXT_PROTOTYPES -g -I include/imgui -I include/imgui/backends
build = build/build.out

target:
	g++ $(flags) $(files) $(imgui_files) -o $(build)
