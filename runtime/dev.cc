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

	if (ImGui::CollapsingHeader("Performance")) {
		ImGui::LabelText("FPS", "%d", dev::fps);
		ImGui::LabelText("Triangles", "%d", dev::triangle_ct);
	}

	if (ImGui::CollapsingHeader("Cheats")) {
		if (ImGui::Button("Toggle Freecam")) {
			toggle_floating_camera();
		}
		if (is_freecam && ImGui::Button("Teleport Here")) {
			teleport_to_floating_camera();
		}
	}

	ImGui::End();
}
