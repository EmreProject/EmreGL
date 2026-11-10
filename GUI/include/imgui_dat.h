#ifndef imgui_dat_h
#define imgui_dat_h


#include "imgui_dat_items.h"

using namespace std;


namespace DAT {



	class Gui {
	private:

		float windowWidth, windowHeight;
		
		float width = 350;
		float height;
		float x=-1000, y;
		
		
		list<ImguiItems::Item*> items;

	public:

		Gui();
		Gui(float width);
		Gui(float width,float x,float y);

		
		ImguiItems::SliderItem& add(string name,float * number,float  min,float  max, int digitAfterDot=5) {

			ImguiItems::SliderItem* item = new ImguiItems::SliderItem(name, number, min, max, digitAfterDot);
			items.insert(items.end(),item );

			return *item;

		}


		ImguiItems::CheckboxItem& add(string name, bool* value_) {

			ImguiItems::CheckboxItem* item = new ImguiItems::CheckboxItem(name, value_);
			items.insert(items.end(), item);

			return *item;
		}

		ImguiItems::DropdownItem& add(string name, vector<const char*> options_) {

			ImguiItems::DropdownItem* item = new ImguiItems::DropdownItem(name, options_);
			items.insert(items.end(), item);

			return *item;
		}

		ImguiItems::FolderItem& addFolder(string name) {
			ImguiItems::FolderItem* item = new ImguiItems::FolderItem(name);
			items.insert(items.end(), item);
			return *item;
		}


		void setWindowSize(int width, int height);
		void GetSizeandPosition(float* x, float* y, float* width, float* height);
		void Render();

	};



}


#endif