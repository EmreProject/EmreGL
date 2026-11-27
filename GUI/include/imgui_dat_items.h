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
#include <concepts>
#include <utility>

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

	class ButtonItem :public Item {

	private:

	public:

		function<void()> clickFunction = []() {};

		ButtonItem(string name_):Item(name_){}

		void Render() {
			if (ImGui::Button(name.c_str())) {

				this->clickFunction();
			}
		}
	};

	class SeperatorItem :public Item {


	private:

	public:

		SeperatorItem(string name_):Item(name_){}

		void Render() {

			ImGui::SeparatorText(name.c_str());
		}

	};

	class DropdownItem :public Item {

	private:


	public:

		vector<string> options;
		int currentIndex = 0;
		function<void(string)> onChangeFunc;


		DropdownItem(std::string name_, std::vector<std::string> options_)
			: Item(name_), options(std::move(options_))
		{
			onChangeFunc = [](std::string chosenOption) {};
		}


		std::string getChoosenValue() const {
			if (currentIndex < 0 || static_cast<std::size_t>(currentIndex) >= options.size()) {
				return {};
			}
			return options[currentIndex];
		}

		DropdownItem& onChange(function<void(string)> onChangeFunc_) {

			onChangeFunc = onChangeFunc_; 

			return *this;
		}

		void Render() override {
			if (options.empty()) {
				return;
			}

			std::vector<const char*> labels;
			labels.reserve(options.size());

			for (const std::string& option : options) {
				labels.push_back(option.c_str());
			}

			if (ImGui::Combo(name.c_str(), &currentIndex,
				labels.data(), static_cast<int>(labels.size()))) {
				onChangeFunc(options[currentIndex]);
			}
		}
	};


	template<Number t>
	class SliderItem : public Item{

	private:

		function<bool()> RenderImguiSlider_;

		void setRenderImguiSliderFunc() {
			RenderImguiSlider_ = [this]() -> bool {
				if constexpr (std::same_as<t, int>) {
					return ImGui::SliderInt(name.c_str(), value, min, max);
				}
				else {
					static_assert(std::same_as<t, float> || std::same_as<t, double>,
						"SliderItem supports only int, float, and double");

					std::string format = "%." + std::to_string(digitAfterDot) + "f";

					if constexpr (std::same_as<t, float>) {
						return ImGui::SliderFloat(name.c_str(), value, min, max,
							format.c_str());
					}
					else {
						return ImGui::SliderScalar(name.c_str(), ImGuiDataType_Double,
							value, &min, &max, format.c_str());
					}
				}
				};
		}
	public:

		t* value;
		t min;
		t max;
		int digitAfterDot;

		function<void()> onChangeFunc;
		function<void()> onChangeFinishedFunc;


		SliderItem(string name_, t* value_, t min_, t max_,int digitAfterDot_=5) {

			this->name = name_;
			this->value = value_;
			this->min = min_;
			this->max = max_;
			this->digitAfterDot = digitAfterDot_;
		
			onChangeFunc = []() {};
			onChangeFinishedFunc = []() {};

			setRenderImguiSliderFunc();

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

			
			bool changedThisFrame = RenderImguiSlider_();
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

		template<Number t>
		ImguiItems::SliderItem<t>& add(string name, t* number, t min, t max,int digitAfterDot=5) {

			ImguiItems::SliderItem<t>* item = new ImguiItems::SliderItem<t>(name, number, min, max, digitAfterDot);
			items.insert(items.end(), item);

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
