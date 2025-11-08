#include "App/line_counter_app.h"
#include "AppFramework/app_settings.h"

int main(int argc, char* argv[])
{
    g_settings = app_settings(argc, argv);
    line_counter_app app;
    return 0;
}
