#include "application.hpp"

#ifdef _WIN32
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#endif

int main()
{
    MyApp app;
    app.run();
    return 0;
}