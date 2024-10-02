files = main.cc shared/context.cc gentime/erosion.cc gentime/gentime.cc gentime/perlin.cc runtime/platform.cc runtime/runtime.cc runtime/runtime_manager.cc runtime/shader.cc runtime/terrain.cc runtime/floating_camera.cc shared/debug.cc runtime/water.cc shared/cmd_args.cc shared/serialize.cc shared/fileutil.cc
flags = -I/usr/include/SDL2 -lSDL2 -lGL -Wall -Wpedantic -DGL_GLEXT_PROTOTYPES -g
build = build/build.out

target:
	g++ $(flags) $(files) -o $(build)
