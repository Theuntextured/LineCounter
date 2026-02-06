#include "App/line_counter_app.h"
#include "AppFramework/app_settings.h"
#include <windows.h>
#include <shellapi.h>
#include <string>
#include <vector>


int main(int argc, char* argv[])
{
    g_settings = app_settings(argc, argv);
    line_counter_app app;
    return 0;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    int argc = 0;
    LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);

    std::vector<std::string> argv;
    argv.reserve(argc);

    // Convert from wide (UTF-16) to UTF-8
    for (int i = 0; i < argc; ++i)
    {
        int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, nullptr, 0, nullptr, nullptr);
        std::string arg(sizeNeeded - 1, '\0');
        WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, arg.data(), sizeNeeded - 1, nullptr, nullptr);
        argv.push_back(std::move(arg));
    }

    LocalFree(argvW);

    // Build a char* array to pass to app_settings, matching main()’s signature
    std::vector<char*> argvC;
    argvC.reserve(argc);
    for (auto& s : argv)
        argvC.push_back(s.data());

    return main(argc, argvC.data());
}
