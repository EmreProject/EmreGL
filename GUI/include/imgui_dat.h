#ifndef imgui_dat_h
#define imgui_dat_h


#include "imgui_dat_items.h"

using namespace std;


namespace DAT {
	
	template <typename T>
	concept Number = std::integral<T> || std::floating_point<T>;


	class Gui {
	private:

		float windowWidth, windowHeight;
		
		float width = 350;
		float height;
		float x = -1000;
		float y = 0;
		
		
		list<ImguiItems::Item*> items;

	public:

		Gui();
		Gui(float width);
		Gui(float width,float x,float y);

		template<Number t>
		ImguiItems::SliderItem<t>& add(string name,t * number,t  min,t max, int digitAfterDot=5) {

			ImguiItems::SliderItem<t>* item = new ImguiItems::SliderItem(name, number, min, max, digitAfterDot);
			items.insert(items.end(),item );

			return *item;

		}


		ImguiItems::CheckboxItem& add(string name, bool* value_) {

			ImguiItems::CheckboxItem* item = new ImguiItems::CheckboxItem(name, value_);
			items.insert(items.end(), item);

			return *item;
		}

		ImguiItems::DropdownItem& add(string name, vector<string> options_) {

			ImguiItems::DropdownItem* item = new ImguiItems::DropdownItem(name, options_);
			items.insert(items.end(), item);

			return *item;
		}


		ImguiItems::ButtonItem& add(string name,function<void()> clickFunc) {
			ImguiItems::ButtonItem* item = new ImguiItems::ButtonItem(name);
			item->clickFunction = clickFunc;
			items.insert(items.end(), item);
			return *item;
		}


		ImguiItems::FolderItem& addFolder(string name) {
			ImguiItems::FolderItem* item = new ImguiItems::FolderItem(name);
			items.insert(items.end(), item);
			return *item;
		}

		ImguiItems::SeperatorItem& addSeperator(string name) {
			ImguiItems::SeperatorItem* item = new ImguiItems::SeperatorItem(name);
			items.insert(items.end(), item);
			return *item;
		}

		void setWindowSize(int width, int height);
		void GetSizeandPosition(float* x, float* y, float* width, float* height);
		void Render();

	};



}


#endif
