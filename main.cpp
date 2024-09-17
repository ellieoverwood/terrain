#include <stdio.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdint.h>
#include "terrain.hpp"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "Terrain Render"

#define WINDOW_MIN_WIDTH 300
#define WINDOW_MIN_HEIGHT 300

#define OPENGL_VERSION_MAJOR 3
#define OPENGL_VERSION_MINOR 3

#define CAMERA_SPEED 0.05f
#define CAMERA_SPEED_SPRINT 0.3f

#define FOV_WALK 80
#define FOV_SPRINT 140

#define MOUSE_SENSITIVITY 0.1f

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

char* read_file(const char* src) {
	char * buffer = 0;
	long length;
	FILE * f = fopen (src, "rb");

	if (f)
	{
	  fseek (f, 0, SEEK_END);
	  length = ftell (f);
	  fseek (f, 0, SEEK_SET);
	  buffer = (char*)malloc (length + 1);
	  if (buffer)
	  {
	    fread (buffer, 1, length, f);
	  }
	  fclose (f);
	}

	buffer[length] = EOF;
	return buffer;
}

SDL_Window* window;
SDL_GLContext context;

#define SDL_ERR(msg) {\
	fprintf(stderr, msg": %s\n", SDL_GetError());\
}\

#define GL_ERR(msg) {\
	GLenum __error = glGetError();\
	if (__error) fprintf(stderr, msg": %d\n", __error);\
}\

void close() {
    SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();
	exit(0);
}

void init_sdl() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) SDL_ERR("failed to intialize video")

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_VERSION_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_VERSION_MINOR);

    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) SDL_ERR("failed to create window")

    SDL_SetWindowMinimumSize(window, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);

	context = SDL_GL_CreateContext(window);
	if (!context) SDL_ERR("failed to create OpenGL context")
	
	if (SDL_GL_SetSwapInterval(1) < 0) SDL_ERR("failed to set vsync")
	//SDL_ShowCursor(SDL_FALSE);
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

unsigned int program;
unsigned int VBO, VAO, EBO, nVBO;

glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

glm::vec3 camera_pos   = glm::vec3(0.0f, 50.0f,  3.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);

bool wireframe = false;
bool sprint = false;

float fov = FOV_WALK;
int width = WINDOW_WIDTH;
int height = WINDOW_HEIGHT;
float fov_target = FOV_WALK;

float lastx = WINDOW_WIDTH / 2;
float lasty = WINDOW_HEIGHT / 2;

float yaw = -90.0f;
float pitch =  0.0f;

bool first_mouse = true;

void resize() {
    glViewport(0, 0, width, height);
	projection = glm::perspective(glm::radians(fov), (float)width/(float)height, 0.1f, 1000.0f);
}

glm::vec3 face_normal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    // Uses p2 as a new origin for p1,p3
    glm::vec3 a = p3 - p2;
    glm::vec3 b = p1 - p2;
    // Compute the cross product a X b to get the face normal
    return glm::normalize(glm::cross(a, b));
}

	#define TRIANGLE_CT (((CHUNK_SIZE - 1) * (CHUNK_SIZE - 1)) * 2)
	#define VERTEX_CT (CHUNK_SIZE * CHUNK_SIZE)

void write_terrain() {
	terrain.triangulate();

	float* normals;
	normals = (float*)malloc(VERTEX_CT * sizeof(float) * 3);

	float* vertices;
	vertices = (float*)malloc(VERTEX_CT * sizeof(float) * 3);

	unsigned int* indices;
	indices = (unsigned int*)malloc(TRIANGLE_CT * 3 * sizeof(unsigned int));

	terrain.write_triangles(indices);
	terrain.write_vertices(vertices);
	terrain.write_normals(normals);

 	glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_CT * sizeof(float) * 3, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TRIANGLE_CT * 3 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &nVBO);
    glBindBuffer(GL_ARRAY_BUFFER, nVBO);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_CT * sizeof(float) * 3, normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
}

void init_opengl() {
	glEnable(GL_DEPTH_TEST);

	resize();

	terrain.perlin(0, 0);

	write_terrain();

	char* vertex_shader_src = read_file("vertex.glsl");
	unsigned int vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
	glCompileShader(vertex_shader);

	{
		int  success;
		char log[512];
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex_shader, 512, NULL, log);
			fprintf(stderr, "vertex shader error: %s\n", log);
		}
	}



	char* fragment_shader_src = read_file("fragment.glsl");
	unsigned int fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
	glCompileShader(fragment_shader);

	{
		int  success;
		char log[512];
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment_shader, 512, NULL, log);
			fprintf(stderr, "fragment shader error: %s\n", log);
		}
	}



	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	{
		int  success;
		char log[512];
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if(!success) {
		    glGetProgramInfoLog(program, 512, NULL, log);
			fprintf(stderr, "shader program linkage error: %s\n", log);
		}
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	free(vertex_shader_src);
	free(fragment_shader_src);

	model = glm::mat4(1.0f);

	view = glm::mat4(1.0f);
	// note that we're translating the scene in the reverse direction of where we want to move
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 

	// if i have more, i'll have to unbind arrays
}

Terrain::Drop* drop;

void input(unsigned int delta_ms) {
    SDL_PumpEvents();

	SDL_Event event;

    while(SDL_PollEvent(&event) != 0) {
    	switch(event.type) {
    		case SDL_QUIT: close();
    		case SDL_KEYDOWN: {
    			switch (event.key.keysym.sym) {
    				case SDLK_ESCAPE: close();
    				case SDLK_v: {
    					wireframe = !wireframe;
    					glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    					break;
    				}
    				case SDLK_TAB: {
    					sprint = !sprint;
    					fov_target = sprint ? FOV_SPRINT : FOV_WALK;
    					break;
    				}
    			}
    			break;
    		}
    		case SDL_WINDOWEVENT: {
    			if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
    				width = event.window.data1;
    				height = event.window.data2;
    				resize();
    			}
    			break;
    		}
    	}
    }

    const uint8_t* keystate = SDL_GetKeyboardState(NULL);
    float camera_speed = (sprint ? CAMERA_SPEED_SPRINT : CAMERA_SPEED) * delta_ms;

    if (keystate[SDL_SCANCODE_W]) {
		camera_pos += camera_speed * camera_front;
    } if (keystate[SDL_SCANCODE_S]) {
		camera_pos -= camera_speed * camera_front;
    } if (keystate[SDL_SCANCODE_A]) {
		camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
    } if (keystate[SDL_SCANCODE_D]) {
		camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
    }

	if (keystate[SDL_SCANCODE_LSHIFT]) {
		for (int i = 0; i < 10000; i ++) {
			Terrain::Drop _drop = Terrain::Drop();
			for (int j = 0; j < 20; j ++) {
				_drop.erode();
			}
		}
		write_terrain();
	}

    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    if (x || y) {
	    x += lastx;
	    y += lasty;
	    float xoff = x - lastx;
	    float yoff = lasty - y;

    	if (first_mouse) // initially set to true
		{
		    first_mouse = false;
		    return;
		}

	    lastx = x;
	    lasty = y;

	    xoff *= MOUSE_SENSITIVITY;
	    yoff *= MOUSE_SENSITIVITY;

	    yaw += xoff;
	    pitch += yoff;

		if(pitch > 89.0f)
			pitch =  89.0f;
		if(pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		camera_front = glm::normalize(direction);
    }
}

unsigned int prior_ms = 0;

void render(unsigned int delta_ms) {

	glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (fov_target != fov) {
		if (abs(fov_target - fov) > 1) {
			fov += (fov_target - fov) * (0.01 * delta_ms);
			resize();
		} else {
			fov = fov_target;
		}
	}

	glUniform2f(glGetUniformLocation(program, "drop"), drop->pos.x, drop->pos.y);

	view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUseProgram(program);
	glDrawElements(GL_TRIANGLES, TRIANGLE_CT * 3, GL_UNSIGNED_INT, 0);
}

void swap() {
	SDL_GL_SwapWindow(window);
}

int main() {
	init_sdl();
	init_opengl();

	drop = new Terrain::Drop();

	for (;;) {
		unsigned int time = SDL_GetTicks();
		unsigned int delta_ms = time - prior_ms;
		prior_ms = time;

		input(delta_ms);
        render(delta_ms);
        swap();
	}
}