files = main.cc shared/context.cc gentime/erosion.cc gentime/gentime.cc gentime/perlin.cc runtime/platform.cc runtime/runtime.cc runtime/runtime_manager.cc runtime/shader.cc runtime/terrain.cc runtime/floating_camera.cc shared/debug.c
flags = -I/usr/include/SDL2 -lSDL2 -lGL -Wall -Wpedantic -DGL_GLEXT_PROTOTYPES -o3
build = build/build.out

target:
	g++ $(flags) $(files) -o $(build)