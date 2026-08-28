#include "Application.h"

#include <Windows.h>

int APIENTRY wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    Application::Application app;

    if (!app.Initialize())
        return 1;

    app.Run();
    app.Shutdown();

    return 0;
}
