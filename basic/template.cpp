#include "global.h"

/*
 * === Test 1: inheritance order ===
 *
 * 1. Derived class contents Base + Derived
 * 2. It also initialize in this order base, derived
 */
namespace Test1
{
    void fn(void)
    {
        std::cout << "Test1\n";
    }
}

int main()
{
    run(1, &(Test1::fn));
}
