#include "global.h"

void run(int i, void (*fn)(void))
{
    std::cout << "=== Test " << i << ": begin ===\n\n";
    (*fn)();
    std::cout << "\n=== Test " << i << ": end ===\n\n";
}
