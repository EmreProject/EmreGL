# GUI module placeholder; add application GUI sources here when available.
add_library(EmreGUI STATIC
src/imgui_dat.cpp)

target_compile_features(EmreGUI PUBLIC cxx_std_20)

target_include_directories(EmreGUI PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(EmreGUI PUBLIC imgui)
