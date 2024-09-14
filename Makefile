build:
	g++ -I/usr/include/SDL2 -lSDL2 -lGL -Wall -Wpedantic -DGL_GLEXT_PROTOTYPES main.cpp terrain.cpp noise.cpp -o a.out 