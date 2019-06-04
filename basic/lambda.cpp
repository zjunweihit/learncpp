#include "global.h"

/*
 * Lambda expression
 * Constructs an unnamed function object
 * 
 * Syntax
 * [ captures ] ( params ) -> ret { body }
 * [ captures ] ( params ) { body }
 * [ captures ] { body }
 * 
 * captures:
 *   []         // capture nothing
 *   [=]        // get the value of out varibles, copy to local varibles
 *   [&]        // reference the out varibles, don't copy to local varibles
 *   [x, &y]    // copy x value and reference y(do not copy)
 *   [=, &z]    // copy all out varibles to local varibles, except z referred in function body
 *   [&, x]     // reference all out varibles, except x copied to local varible
 *   [this]     // reference the current object
 *   [*this]    // copy the current object
 */

/*
 * === Test 1: lambda capture ===
 */
namespace Test1
{
    void lambda_capture(void)
    {
        int base = 10;
        std::cout << "base is:" << base << std::endl;

        std::cout << "return 6: " << std::endl;
        std::cout << [] () { return 6; } << std::endl;

        std::cout << "return input x(5): " << std::endl;
        std::cout << [] (int x) { return x; } (5) << std::endl;

        std::cout << "return out base(10) + input x(5): " << std::endl;
        std::cout << [base] (int x) { return x + base; } (5) << std::endl;

        std::cout << "return get all out varibles base(10) + input x(5): " << std::endl;
        std::cout << [=] (int x) { return x + base; } (5) << std::endl;

        std::cout << "return reference all out varibles base(10) += input x(5): " << std::endl;
        std::cout << [&] (int x) { return base += x; } (5) << std::endl;

        std::cout << "now base is:" << std::endl;
        std::cout << base << std::endl;
    }

    void fn(void)
    {
        std::cout << "<<< lambda_capture >>>\n";
        lambda_capture();
    }
}

int main()
{
    run(1, &(Test1::fn)); // lambda capture
}
