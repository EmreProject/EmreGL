# EmreGL

C++20 / CMake starter project with bundled GLFW and GLEW.

## Layout

- `Renderer/include`, `Renderer/src`: public engine headers and implementation (`Engine`).
- `Sandbox`: executable linked to the engine.
- `Math/include`: existing math header; the module is an interface placeholder.
- `GUI`: interface placeholder for future GUI sources.
- `Assets`: runtime assets.
- `External`: bundled GLFW and GLEW sources, including their licenses.

The math header is incomplete: `ogl_Vector.h`, `ogl_Projection.h`, Assimp headers,
and the declared math implementations have not been added. It is preserved but
is not included by the starter executable. The printed wood texture path is an
example; the texture has not been added either.

## Build on Windows

Install Visual Studio 2022 with Desktop development with C++ and a Windows SDK.
From an x64 Native Tools Command Prompt (or Visual Studio's CMake integration):

```powershell
cmake --preset x64-debug
cmake --build out/build/x64-debug
.\out\build\x64-debug\Sandbox\Sandbox.exe
```

The Ninja presets require Ninja on PATH and the matching MSVC developer environment.
Alternatively, from a normal terminal:

```powershell
cmake -S . -B out/build/vs -G "Visual Studio 17 2022" -A x64
cmake --build out/build/vs --config Debug
.\out\build\vs\Sandbox\Debug\Sandbox.exe
```

Add new implementation files explicitly to the appropriate module's CMakeLists.txt.
Keep public headers in `include/` and implementation files in `src/`.

## Git

The local repository uses `main`. Build output and local IDE state are ignored.
Bundled dependencies are tracked as source, rather than requiring a network fetch.
No remote is configured. To connect an existing empty remote repository:

```powershell
git remote add origin <repository-url>
git push -u origin main
```

