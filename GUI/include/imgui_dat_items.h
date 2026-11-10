#ifndef imgui_items_h
#define imgui_items_h



#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <list>
#include <functional>
#include <algorithm>

using namespace std;

namespace ImguiItems {



	template <typename T>
	concept Number = std::integral<T> || std::floating_point<T>;


	class Item {
		
	public:
		string name;
		Item(){}
		Item(string name_):name(name_){}

		virtual void Render() = 0;

	};

	
	class CheckboxItem :public Item {

	private:


	public:

		bool* value;
		function<void()> onChangeFunc;

		CheckboxItem(string name_, bool* value_) :Item(name_), value(value_) {
		
			onChangeFunc = []() {};

		}


		CheckboxItem& onChange(function<void()> onChangeFunc_) {

			this->onChangeFunc = onChangeFunc_;

			return *this;
		}

		void Render() {

			bool changedThisFrame = ImGui::Checkbox(name.c_str(), value);

			if (changedThisFrame) {
				onChangeFunc();
			}

		}
	};



	class DropdownItem :public Item {

	private:


	public:

		vector<const char*> options;
		int currentIndex = 0;
		function<void(string)> onChangeFunc;

		DropdownItem(string name_, vector<const char*> options_) :Item(name_),
			options(options_){
		
			onChangeFunc = [](string choosenOption) {};

		}


		string getChoosenValue() {

			return string(options[currentIndex]);

		}

		DropdownItem& onChange(function<void(string)> onChangeFunc_) {

			onChangeFunc = onChangeFunc_; 

			return *this;
		}

		void Render() {


			bool changedThisFrame = ImGui::Combo(name.c_str(), &currentIndex, &options[0], static_cast<int>(options.size()));
		
			if (changedThisFrame) {

				string choosen(options[currentIndex]);

				onChangeFunc(choosen);
			}

		}
	};


	
	class SliderItem : public Item{

	private:


	public:

		float* value;
		float min;
		float max;
		float digitAfterDot;

		function<void()> onChangeFunc;
		function<void()> onChangeFinishedFunc;


		SliderItem(string name_, float* value_, float min_, float max_,int digitAfterDot_=5) {

			this->name = name_;
			this->value = value_;
			this->min = min_;
			this->max = max_;
			this->digitAfterDot = digitAfterDot_;
		
			onChangeFunc = []() {};
			onChangeFinishedFunc = []() {};

		}


		SliderItem& onChange( function<void()> func) {

			onChangeFunc = func;
			return *this;
			
		}

		SliderItem& onChangeFinished(function<void()> func) {

			onChangeFinishedFunc = func;
			return *this;

		}

		void Render() {

			ostringstream oss;
			oss << "%." << digitAfterDot << "f";
			bool changedThisFrame = ImGui::SliderFloat(this->name.c_str(), (float*)(value),
				(float)(min), (float)(max), oss.str().c_str());
			bool finishedEditing = ImGui::IsItemDeactivatedAfterEdit();


			if (changedThisFrame) {
				onChangeFunc();
			}

			if (finishedEditing) {
				onChangeFinishedFunc();
			}
		}


	};



	
	class FolderItem :public Item {

	private:

		list<Item*> items;

	public:

		FolderItem(string name_) :Item(name_) {}
		~FolderItem() {
			for_each(items.begin(), items.end(), [](Item* item) {delete item; });
		}

	
		ImguiItems::SliderItem& add(string name, float* number, float min, float max,int digitAfterDot=5) {

			ImguiItems::SliderItem* item = new ImguiItems::SliderItem(name, number, min, max, digitAfterDot);
			items.insert(items.end(), item);

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


		void Render() {
			

			ImGui::PushID(this); // Unique scope for this FolderItem object


			if(ImGui::CollapsingHeader(name.c_str())){
			
				for_each(items.begin(), items.end(), [](Item* item) {
					item->Render(); 
					});
			}

			ImGui::PopID();
		}
	};



}


#endif
