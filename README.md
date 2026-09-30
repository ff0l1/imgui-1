# imgui-1

Win32, Direct3D 11, Dear ImGui. Login, then a loader, then a floating overlay. Custom theme and glass panels, because stock ImGui looks like stock ImGui.

C++20.

![Menu](docs/menu.png)

## What you see

Login and sign-up with screen transitions, a loader, then an overlay with shared widgets: toggles, sliders, keybinds, color pickers. Particles drift behind the panels. Fonts and icons are baked in.

```bat
dist\imgui-menu.exe
dist\imgui-menu.exe --menu
```

`--menu` skips login.

### Login

![Login](docs/login.webp)

### Menu

![Menu](docs/menu.webp)

## Build

Windows 10 or 11, Visual Studio 2022, Windows SDK 10.

```bat
msbuild imgui-menu.sln /p:Configuration=Release /p:Platform=x64
```

Output: `build\x64\Release\imgui-menu.exe`. A copy also sits in `dist\`.

## Files

```
src/Application     entry and screen flow
src/Renderer        D3D11
src/Window          Win32 overlay
src/UI              auth, loader, menu, theme, particles
third_party         Dear ImGui, stb
dist/
docs/
```
