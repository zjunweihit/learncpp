#include "global.h"
#include "math.h"
#include <exception> // for std::exception
#include <stdexcept> // for std::runtime_error

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

/*
 * === Test 4: exception class ===
 *
 * overloaded funtions may return different type of error code, since it requires
 * different type of parameter it can take and return. But they can throw the same
 * the same type of exception. e.g. array index(int)
 *
 * when constructor fails, its destructor will be never called, so it has to
 * do cleanup before throw an exception to indicate the object failed to create.
 *
 * basic data types(e.g. int) may not indicate the problem clearly, then a exception
 * class is required.
 * Note: to catch the exception class by reference, in case copy or clicing of
 *       class object.
 * If an exception class inherits from an exception class, it should catch the
 * derived class at first and then the base class.
 * The compiler may info that, ^^
 *   warning: exception of type 'Test4::Derived &' will be caught by earlier
 *
 * The good news is that all of these exception classes are derived from
 * a single class called std::exception.
 * its member function what() returns a C-style string description of the exception.
 */
namespace Test4
{
    class ArrayException1
    {
    private:
        std::string m_error;

    public:
        ArrayException1(std::string error)
            : m_error(error)
        {
        }

         const char* getError() { return m_error.c_str(); }
    };

    class IntArray1
    {
    private:

        int m_data[3]; // assume array is length 3 for simplicity
    public:
        IntArray1() {}

        int getLength() { return 3; }

        int& operator[](const int index)
        {
            if (index < 0 || index >= getLength())
                throw ArrayException1("Invalid index");

            return m_data[index];
        }

    };

    void ex_class(void)
    {
        IntArray1 array;

        try
        {
            int value = array[5];
        }
        // NOTE:
        // exception handlers should catch class exception objects by reference
        // instead of by value, avoiding copy and slicing a class object.
        // By pointer is also not a good idea.
        catch (ArrayException1 &exception)
        {
            std::cerr << "An array exception occurred (" << exception.getError() << ")\n";
        }
    }

    class Base
    {
    public:
        Base() {}
    };

    class Derived: public Base
    {
    public:
        Derived() {}
    };

    void ex_class_inheriant_bad(void)
    {
        try
        {
            throw Derived();
        }
        catch (Base &base)
        {
            std::cout << "uncomment me for the test, bad inheriant experience\n";
        }
        // NOET:
        // handlers for derived exception classes should be listed before those
        // for base classes
        // Compiler: (good boy^^)
        // warning: exception of type 'Test4::Derived &' will be caught by earlier
        //catch (Base &base)
        //{
        //    std::cout << "caught Base\n";
        //}
        //catch (Derived &derived)
        //{
        //    std::cout << "caught Derived\n";
        //}
    }
    void ex_class_inheriant_good(void)
    {
        try
        {
            throw Derived();
        }
        // NOET:
        // handlers for derived exception classes should be listed before those
        // for base classes
        catch (Derived &derived)
        {
            std::cout << "caught Derived\n";
        }
        catch (Base &base)
        {
            std::cout << "caught Base\n";
        }
    }

    void ex_class_std(void)
    {
        try
        {
            // Your code using standard library goes here
            // We'll trigger one of these exceptions intentionally for the sake of example
                    std::string s;
                    s.resize(-1); // will trigger a std::bad_alloc
        }

        // This handler will catch std::bad_alloc (and any exceptions derived from it) here
        // it's not called actually?? that should be called firstly, exact exception
        //catch (std::bad_alloc &exception)
        //{
        //        std::cerr << "You ran out of memory!" << '\n';
        //}

        // This handler will catch std::exception and all the derived exceptions too
        // NOTE:
        //   that this string is meant to be used for descriptive text only --
        //   do not use it for comparisons, as it is not guaranteed to be the
        //   same across compilers.
        catch (std::exception &exception)
        {
            std::cerr << "Standard exception: " << exception.what() << '\n';
        }
    }

    void ex_class_std_throw_std(void)
    {
        try
        {
            throw std::runtime_error("Bad things happened");
        }
        // This handler will catch std::exception and all the derived exceptions too
        catch (std::exception &exception)
        {
            std::cerr << "Standard exception: " << exception.what() << '\n';
        }
    }

    class ArrayException: public std::exception
    {
    private:
        std::string m_error;

    public:
        ArrayException(std::string error)
            : m_error(error)
        {
        }

        // override what() const member function
        // return the std::string as a const C-style string
        // const char* what() const { return m_error.c_str(); } // pre-C++11 version
        const char* what() const noexcept { return m_error.c_str(); } // C++11 version
    };

    class IntArray
    {
    private:

        int m_data[3]; // assume array is length 3 for simplicity
    public:
        IntArray() {}

        int getLength() { return 3; }

        int& operator[](const int index)
        {
            if (index < 0 || index >= getLength())
                throw ArrayException("Invalid index");

            return m_data[index];
        }

    };

    void ex_derived_class(void)
    {
        IntArray array;

        try
        {
            int value = array[5];
        }
        catch (ArrayException &exception) // derived catch blocks go first
        {
            std::cerr << "An array exception occurred (" << exception.what() << ")\n";
        }
        catch (std::exception &exception)
        {
            std::cerr << "Some other std::exception occurred (" << exception.what() << ")\n";
        }
    }

    void fn(void)
    {
        std::cout << "<<< exception class >>>\n";
        ex_class();

        std::cout << "\n<<< exception class inheriant >>>\n";
        std::cout << "bad pratice\n";
        ex_class_inheriant_bad();
        std::cout << "good pratice\n";
        ex_class_inheriant_good();

        std::cout << "\n<<< exception class standard library >>>\n";
        std::cout << "use std exception class to catch the exception\n";
        ex_class_std();
        std::cout << "throw std exception\n";
        ex_class_std_throw_std();
        ex_class_std_throw_std();

        std::cout << "\n<<< derived exception class from std::exception >>>\n";
        ex_derived_class();
    }
}

/*
 * === Test 5: function try block ===
 *
 * Catch the exception for an entire function.
 *
 * Note:
 *   unlike normal catch blocks, if don't explicitly throw a new exception, or
 *   rethrow the current exception, the exception will be implicitly rethrown up
 *   the stack.
 */
namespace Test5
{
    class A
    {
    private:
        int m_x;
    public:
        A(int x) : m_x(x)
        {
            if (x <= 0)
                throw 1;
        }
    };

    class B : public A
    {
    public:
        // Note:
        //   This try block includes initializer list util the end of the B function
        B(int x) try : A(x) // note addition of try keyword here
        {
            throw 2; // it will be catched by later catch too
        }
        // Note:
        //   Cannot define function between try and catch blocks.
        //   It expects catch block here by compiler.
        //   this is at same level of indentation as the function itself
        catch (int e)
        {
            // Exceptions from member initializer list or constructor body are caught here
            std::cerr << "Construction of A failed: " << e << "\n";
            // Even if an exception isn't explicitly thrown here,
            // the current exception will be implicitly rethrown to the caller.
        }
    };

    void fn(void)
    {
        try
        {
            B b(0);
        }
        // by default, function try block will rethrow the exception to the caller here.
        catch (int x)
        {
            std::cout << "Oops: " << x << "\n";
        }
    }
}

// NOTE: Downside of exceptions
//   1) in try block, print error message and DO clean up, like freeing resouces.
//      e.g. delete pointer(or using smart pointer), close fd
//   2) exceptions should never be thrown in destructors.
//      it will cause program terminated immediately, better to write a message
//      to a log file instead.
//   3) It may cause a small performance price to pay. unwinding stack to catch
//      an exception, or try check.
//      moden CPU supports zero-cost exceptions, which has no cost if no error,
//      but it causes more when an exception is thrown.
//   4) When to use exceptions, all of below are true, to use it
//      - The error being handled is likely to occur only *infrequently*.
//      - The error is *serious* and execution could not continue otherwise
//      - The error *cannot be handled* at the place where it occurs.
//      - There *isn’t* a good alternative way to return an *error code back* to the caller.
int main()
{
    run(1, &(Test1::fn)); // basic exception: throw, try, catch
    run(2, &(Test2::fn)); // unwind stack
    run(3, &(Test3::fn)); // rethrow an exception
    run(4, &(Test4::fn)); // exception class
    run(5, &(Test5::fn)); // function try block
}
