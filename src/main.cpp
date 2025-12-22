#include "application.hpp"

extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

int main()
{
    MyApp app;
    app.run();
    return 0;
}