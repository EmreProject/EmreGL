#include "imgui_dat.h"



namespace DAT {


	Gui::Gui() {

		ImGuiIO& io = ImGui::GetIO();
		// These are in *pixels*, matching your current framebuffer size.
		float screenWidth = io.DisplaySize.x;
		float screenHeight = io.DisplaySize.y;

		this->x = screenWidth - width;
		this->y = 0;


	}

	Gui::Gui(float width) {

		this->width = width;
		

	}

	Gui::Gui(float width, float x, float y) {


		this->width = width;
		this->x = x;
		this->y = y;
	}

	void Gui::setWindowSize(int winWidth, int winHeight) {

		this->windowWidth = winWidth;
		this->windowHeight = winHeight;

		if (this->x == -1000) {
			this->x = this->windowWidth - this->width;
		}
	}

	
	void Gui::GetSizeandPosition(float* x, float* y, float* width, float* height) {

		*x = this->x;
		*y = this->y;
		*width = this->width;
		*height = this->height;
	}

	void Gui::Render() {

		/*
		ImGuiIO& io = ImGui::GetIO();
		// These are in *pixels*, matching your current framebuffer size.
		float screenWidth = io.DisplaySize.x;
		float screenHeight = io.DisplaySize.y;
		

		ImGui::SetNextWindowPos(ImVec2(screenWidth - width, 0));
		ImGui::SetNextWindowSizeConstraints(
			ImVec2(300, -1),  // (min_width = 300, min_height = -1 => no minimum)
			ImVec2(300, screenHeight) // (max_width = 300, max_height is huge => effectively no maximum)
		);

		*/
		

		ImGui::SetNextWindowPos(ImVec2(this->x, this->y));
		ImGui::SetNextWindowSizeConstraints(
			ImVec2(this->width, -1),  // (min_width = 300, min_height = -1 => no minimum)
			ImVec2(this->width, this->windowHeight- this->y) // (max_width = 300, max_height is huge => effectively no maximum)
		);

		ImGui::Begin("Debug",nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

		ImVec2 imWindowPos = ImGui::GetWindowPos();
		ImVec2 imWindowSize = ImGui::GetWindowSize();

		this->x = imWindowPos.x;
		this->y = imWindowPos.y;
		this->width = imWindowSize.x;
		this->height = imWindowSize.y;
	

		for_each(this->items.begin(), items.end(), [](ImguiItems::Item* item) {

			item->Render();

			});


		ImGui::End();



	}


}