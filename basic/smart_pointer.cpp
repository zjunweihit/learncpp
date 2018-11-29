#include "global.h"
#include "math.h"
#include <exception> // for std::exception
#include <stdexcept> // for std::runtime_error

/*
 * === Test 1: smart pointer and move semantics ===
 *
 * A smart pointer is a class holding a pointer manage the allocated memory pointed
 * by the pointer. Free the resource when it goes out of scope.
 *
 * std::auto_ptr: avoid it
 *   Introduced in C++98, which is like Auto_ptr2, but with many problems.
 *   1) It implements move sematics through copy and assignment operator, so
 *      pass a std::auto_ptr by value to a function will cause resouce to get
 *      moved to function parameter, then caller will dereference a null pointer.
 *   2) always deletes its contents using non-array delete. memory leak.
 *   3) not working well with other classes in standard library, including
 *      most containers and algorithms.
 * In C++11, std::auto_ptr is replaced by "move-aware" smart pointers:
 * std::scoped_ptr, std::unique_ptr, std::weak_ptr, and std::shared_ptr.
 *
 * C++ language simply had no mechanism to differentiate “copy semantics” from
 * “move semantics”. Overriding the copy semantics to implement move semantics
 * leads to weird edge cases and inadvertent bugs.
 * For example, you can write res1 = res2 and have no idea whether res2 will be
 * changed or not!
 * Because of this, in C++11, the concept of “move” was formally defined, and
 * “move semantics” were added to the language to differentiate copying from moving.
 */
namespace Test1
{
    template<class T>
    class Auto_ptr1
    {
        T* m_ptr;
    public:
        // Pass in a pointer to "own" via the constructor
        Auto_ptr1(T* ptr=nullptr)
            :m_ptr(ptr)
        {
        }

        // The destructor will make sure it gets deallocated
        ~Auto_ptr1()
        {
            delete m_ptr;
        }

        // Overload dereference and operator-> so we can use Auto_ptr1 like m_ptr.
        T& operator*() const { return *m_ptr; }
        T* operator->() const { return m_ptr; }
    };

    // A sample class to prove the above works
    class Resource
    {
    public:
        Resource() { std::cout << "Resource acquired\n"; }
        ~Resource() { std::cout << "Resource destroyed\n"; }
        void sayHi() { std::cout << "Hi\n"; }
    };

    void sp_return_early(void)
    {
        Auto_ptr1<Resource> ptr(new Resource); // ptr now owns the Resource

        int x;
        //std::cout << "Enter an integer: ";
        //std::cin >> x;
        std::cout << "Set x = 0\n";
        x = 0;

        if (x == 0) {
            std::cout << "The function returns early\n";
            return; // the function returns early
        }

        // do stuff with ptr here
        ptr->sayHi();
    }

    // NOTE:
    //   There is no copy function, so a shadow copy is performed when initialize
    //   res2. Both res1 an res2 point to the same Resource.
    //   After res2 frees the Resource, leave res1 with dangling pointer. And crash!
    void sp_copy_failure(void)
    {
        Auto_ptr1<Resource> res1(new Resource);

        // Similarly, don't initialize res2 and then assign res2 = res1;
        Auto_ptr1<Resource> res2(res1);
    }

    template<class T>
    class Auto_ptr2
    {
        T* m_ptr;
    public:
        Auto_ptr2(T* ptr=nullptr)
            :m_ptr(ptr)
        {
        }

        ~Auto_ptr2()
        {
            delete m_ptr;
        }

        // A copy constructor that implements move semantics
        Auto_ptr2(Auto_ptr2& a) // note: not const
        {
            m_ptr = a.m_ptr; // transfer our dumb pointer from the source to our local object
            a.m_ptr = nullptr; // make sure the source no longer owns the pointer
        }

        // An assignment operator that implements move semantics
        Auto_ptr2& operator=(Auto_ptr2& a) // NOTE: not const
        {
            if (&a == this)
                return *this;

            delete m_ptr; // make sure we deallocate any pointer the destination is already holding first
            m_ptr = a.m_ptr; // then transfer our dumb pointer from the source to the local object
            a.m_ptr = nullptr; // make sure the source no longer owns the pointer
            return *this;
        }

        T& operator*() const { return *m_ptr; }
        T* operator->() const { return m_ptr; }
        bool isNull() const { return m_ptr == nullptr;  }
    };

    void sp_copy_by_move(void)
    {
        std::cout << "create res1\n";
        Auto_ptr2<Resource> res1(new Resource);
        std::cout << "create res2 as nullptr\n";
        Auto_ptr2<Resource> res2; // Start as nullptr

        std::cout << "res1 is " << (res1.isNull() ? "null\n" : "not null\n");
        std::cout << "res2 is " << (res2.isNull() ? "null\n" : "not null\n");

        std::cout << "copy res1 to res2 by move\n";
        res2 = res1; // res2 assumes ownership, res1 is set to null

        std::cout << "Ownership transferred\n";

        std::cout << "res1 is " << (res1.isNull() ? "null\n" : "not null\n");
        std::cout << "res2 is " << (res2.isNull() ? "null\n" : "not null\n");
    }

    void fn(void)
    {
        std::cout << "<<< return early to free the Resource automatically >>>\n";
        sp_return_early();
        std::cout << "\n<<< doing shadow copy causes the program crash >>>\n";
        //sp_copy_failure(); // cause the program crash
        std::cout << "\n<<< do copy by move >>>\n";
        sp_copy_by_move(); // fix the copy failure
    }
}

int main()
{
    run(1, &(Test1::fn)); // smart pointer and move semantics
}
