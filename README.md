# EmreGL

C++20 / CMake starter project with bundled GLFW and GLEW.

## Requirements

A C++20 compiler (MSVC, GCC, Clang or Apple Clang), CMake 3.22+ and Ninja.
Visual Studio's IDE is optional. GLFW and GLEW sources are included; no submodule
or dependency download is needed. OpenGL development libraries are required.

### Windows

Choose either:

- Install Visual Studio Build Tools with Desktop development with C++ and a
  Windows SDK. Run the build in an x64 Native Tools Command Prompt with Ninja
  on PATH. The Visual Studio IDE also supports opening this CMake project.
- Install MSYS2, open its UCRT64 terminal, and install:

```sh
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja
```

Use the UCRT64 terminal for MinGW builds so the compiler and runtime DLLs are
on PATH. Use separate build directories when changing compilers.

### Linux (Ubuntu / Debian)

```sh
sudo apt-get update
sudo apt-get install build-essential cmake ninja-build xorg-dev libgl1-mesa-dev
```

For Clang, also install `clang`. X11 is the default Linux backend. To add Wayland,
install `libwayland-dev libxkbcommon-dev wayland-protocols pkg-config` and configure
with `-DGLFW_BUILD_WAYLAND=ON`. Other distributions need equivalent development
packages; see the [GLFW compilation guide](https://www.glfw.org/docs/latest/compile.html).

### macOS

Install Apple command-line tools and CMake/Ninja (Homebrew shown):

```sh
xcode-select --install
brew install cmake ninja
```

Apple's SDK supplies OpenGL. OpenGL is deprecated on macOS; this project targets
desktop OpenGL, not Metal, mobile platforms, or browsers.

## Build and run

After installing your platform's requirements, clone the repository, enter its
root directory, and run the same commands on all three desktop platforms:

```sh
cmake --preset debug
cmake --build --preset debug --parallel
ctest --preset debug
```

Run `./out/build/debug/bin/Sandbox` on Linux/macOS or
`.\out\build\debug\bin\Sandbox.exe` on Windows. Use `release` in place of `debug`
for an optimized build. Binaries are always in the build directory's `bin/`.
The smoke test prints dependency versions and an example asset path; it does not
require a display or create an OpenGL context.

CMake detects the system compiler. On Linux/macOS you can select one explicitly
in a fresh build directory:

```sh
CC=clang CXX=clang++ cmake -S . -B out/build/clang -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build out/build/clang
ctest --test-dir out/build/clang --output-on-failure
```

Ninja is optional when using CMake directly with another generator. For example,
with MSVC Build Tools installed, from a normal Windows terminal:

```powershell
cmake -S . -B out/build/vs -G "Visual Studio 17 2022" -A x64
cmake --build out/build/vs --config Debug
ctest --test-dir out/build/vs -C Debug --output-on-failure
.\out\build\vs\bin\Sandbox.exe
```

The original `x64-debug`, `x64-release`, `x86-debug`, and `x86-release` presets
remain available for MSVC developer environments of the matching architecture.

## Layout and current scope

- `Renderer/include`, `Renderer/src`: public engine headers and implementation.
- `Sandbox`: executable linked to `EmreGL::Engine`.
- `Math/include`: existing incomplete math header; interface placeholder module.
- `GUI`: interface placeholder for future sources.
- `Assets`: runtime assets.
- `External`: bundled GLFW and GLEW sources and their licenses.

Add implementation files explicitly to each module's CMakeLists.txt. The math
header still needs `ogl_Vector.h`, `ogl_Projection.h`, Assimp and its declared
implementations. It is preserved but not included by the starter executable.
The example `textures/wood.png` asset is not included. Asset paths currently point
to the source checkout; this is a development build, not an installable package.

## Validation

The GitHub Actions workflow builds and runs the smoke test on Windows (MSVC and
MinGW), Linux (GCC and Clang), and macOS (Apple Clang). These jobs run after pushing
to GitHub; adding a workflow alone does not mean all platforms have passed.

## Git

Build output and IDE state are ignored. Bundled dependencies are tracked as
source. To connect an empty GitHub repository:

```sh
git remote add origin <repository-url>
git push -u origin main
```

Choose a license for your own code before advertising reuse rights. Bundled
dependency licenses are retained in `External`.


## ImGui

`External/imgui/CMakeLists.txt` defines the `imgui` library, including the demo and the
GLFW/OpenGL3 backends. `EmreGUI` links `imgui`, and `Renderer` links `EmreGUI`, so Sandbox can include
`imgui.h`, `imgui_impl_glfw.h`, and `imgui_impl_opengl3.h` without extra paths.
The startup output prints the linked ImGui version.

Displaying UI still requires a GLFW window with a current OpenGL context, ImGui
context/backend initialization, a frame/render loop, and shutdown. Follow the
bundled `External/imgui/examples/example_glfw_opengl3/main.cpp` for that lifecycle.
The current console Sandbox does not open a GUI window.

