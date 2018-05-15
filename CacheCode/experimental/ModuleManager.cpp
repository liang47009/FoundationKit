
#include "ModuleManager.hpp"

class TestModule
{
public:
    TestModule()
    {
        value = 100;
    }

private:
    int value;
};




IMPLEMENT_MODULE(TestModule, TestModule)







