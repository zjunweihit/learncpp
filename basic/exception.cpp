#include "global.h"
#include "math.h"

/*
 * === Test 1: basic exception: throw, try, catch ===
 *
 * try: observing exceptions thrown by a statement
 * throw: signal an exception
 * catch: handle exceptions
 *
 * If a statement throw an exception in try block, it will skip later statements
 * and go through the catch block for any type of exception.
 *   If no match for thrown exception type, go up layer until the main function,
 *   aborting the program.
 *   If it's match in catch block, program will go on after catch block
 */
namespace Test1
{
    void ex_static(void)
    {
        try
        {
            throw 4.5; // throw exception of type double
            std::cout << "This never prints\n";
        }
        catch(double x) // handle exception of type double
        {
            std::cerr << "We caught a double of value: " << x << '\n';
        }
        std::cout << "The program will go on\n";
    }

    // NOTE:
    //   it works for 'a', resulting 0
    void ex_sqart(void)
    {
        std::cout << "Enter a number: ";
        double x;
        std::cin >> x;

        try // Look for exceptions that occur within try block and route to attached catch block(s)
        {
            // If the user entered a negative number, this is an error condition
            if (x < 0.0)
                throw "Can not take sqrt of negative number"; // throw exception of type const char*

            // Otherwise, print the answer
            std::cout << "The sqrt of " << x << " is " << sqrt(x) << '\n';
        }
        catch (const char* exception) // catch exceptions of type const char*
        {
            std::cerr << "Error: " << exception << '\n';
        }
    }

    void fn(void)
    {
        ex_static();
        //ex_sqart();
    }
};

int main()
{
    run(1, &(Test1::fn)); // basic exception: throw, try, catch
}
