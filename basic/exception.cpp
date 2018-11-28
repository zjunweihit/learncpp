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
 *   This process is called unwinding the stack.
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

/*
 * === Test 2: unwind stack ===
 *
 * If an exception is thrown, it will unwind stack to caller and find if it's
 * in a catch block, if so, check exception type, no match either, go on unwinding
 * stack to caller's caller and go on, until the main funtion and abort the program.
 * at last.
 *
 * If any catch block matches the exception, it will go on the caller function.
 * OS will handle the uncatched exception with OS specific actions, error msg, or crash.
 *
 * Catch all exceptions:
 *      catch (...) // catch-all handler
 *   We can use it to save the state if the main function throw an exception.
 *
 * (Not recommended)
 * Exception specifier: rarely used, may be not supported by compilers
 *   a mechanism that allows us to use a function declaration to specify
 *   whether a function may or will not throw exceptions.
 *   e.g.
 *   int doSomething() throw(); // does not throw exceptions
 *   int doSomething() throw(double); // may throw a double
 *   int doSomething() throw(...); // may throw anything
 */
namespace Test2
{
    void last() // called by third()
    {
        std::cout << "Start last\n";
        std::cout << "--> last throwing int exception\n";
        throw -1;
        std::cout << "End last\n";

    }

    void third() // called by second()
    {
        std::cout << "Start third\n";
        last();
        std::cout << "End third\n";
    }

    void second() // called by first()
    {
        std::cout << "Start second\n";
        try
        {
            third();
        }
        catch(double)
        {
             std::cerr << "second caught double exception\n";
        }
        std::cout << "End second\n";
    }

    void first() // called by main()
    {
        std::cout << "Start first\n";
        try
        {
            second();
        }
        catch (int)
        {
             std::cerr << "first caught int exception\n";
        }
        catch (double)
        {
             std::cerr << "first caught double exception\n";
        }
        std::cout << "End first\n";
    }

    void ex_unwind(void)
    {
        std::cout << "Start main\n";
        try
        {
            first();
        }
        catch (int)
        {
             std::cerr << "main caught int exception\n";
        }
        std::cout << "End main\n";
    }

    void ex_catch_all(void)
    {
        try
        {
            throw 5; // throw an int exception
        }
        catch (double x)
        {
            std::cout << "We caught an exception of type double: " << x << '\n';
        }
        catch (...) // catch-all handler
        {
            std::cout << "We caught an exception of an undetermined type\n";
        }
        //save_state();
    }

    void fn(void)
    {
        std::cout << "<<< unwind stack >>>\n";
        ex_unwind();

        std::cout << "\n<<< catch all >>>\n";
        ex_catch_all();
    }
}

/*
 * === Test 3: rethrowing exceptions ===
 * Catch an exception, but just log error and pass it to the caller to handle.
 *
 * It's allowed to throw a new exception in catch block, although it looks wired.
 * Throwing a new exception or throw the catched exception is not always a good
 * way. Especially the excetpion is a class object(Base, Derived, copy initializtion
 * and slicing may happen)
 *
 * When rethrowing the same exception, use the throw keyword by itself.
 * It just rethrows the same exception without copies or slicing.
 */
namespace Test3
{
    class Base
    {
    public:
        Base() {}
        virtual void print() { std::cout << "Base"; }
    };

    class Derived: public Base
    {
    public:
        Derived() {}
        virtual void print() { std::cout << "Derived"; }
    };

    void ex_rethrow_exception_bad()
    {
        try
        {
            try
            {
                std::cout << "Throwing Derived\n";
                throw Derived();
            }
            catch (Base& b)
            {
                std::cout << "Caught Base b, which is actually a ";
                b.print();
                std::cout << "\n";
                throw b; // the Derived object gets sliced here
            }
        }
        catch (Base& b)
        {
            std::cout << "Caught Base b, which is actually a ";
            b.print();
            std::cout << "\n";
        }
    }

    void ex_rethrow_exception_good(void)
    {
        try
        {
            try
            {
                std::cout << "Throwing Derived\n";
                throw Derived();
            }
            catch (Base& b)
            {
                std::cout << "Caught Base b, which is actually a ";
                b.print();
                std::cout << "\n";
                throw; // rethrowing the object here
            }
        }
        catch (Base& b)
        {
            std::cout << "Caught Base b, which is actually a ";
            b.print();
            std::cout << "\n";
        }
    }

    void fn(void)
    {
        ex_rethrow_exception_bad();
        ex_rethrow_exception_good();
    }
}

int main()
{
    run(1, &(Test1::fn)); // basic exception: throw, try, catch
    run(2, &(Test2::fn)); // unwind stack
    run(3, &(Test3::fn)); // rethrow an exception
}
