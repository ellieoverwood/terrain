#include "dev.h"
#include <imgui.h>
#include <SDL_opengl.h>
#include <stdio.h>

bool wireframe_on = false;

void dev::update() {
	bool active;
	ImGui::Begin("Developer Menu", &active, ImGuiWindowFlags_None);

	if (ImGui::CollapsingHeader("Graphics")) {

		if (ImGui::Button("Toggle Wireframe")) {
			wireframe_on = !wireframe_on;
			glPolygonMode(GL_FRONT_AND_BACK, wireframe_on ? GL_LINE : GL_FILL);
		}
	}

	ImGui::End();
}
