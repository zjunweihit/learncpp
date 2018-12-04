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

/*
 * === Test 2: R value ===
 *
 * l-value:
 *   assignable memory address, when const was added, it were split into
 *   modifiable l-value and non-modifiable l-value
 * r-value:
 *   non l-value, like literals(5), temporary values(x+1), anonymouse object(Fraction(5, 2))
 *   cannot be assigned to. have express scope(die at the end of expression they are in)
 *
 * To support move semantics, C++11 introduces 3: pr-values, x-values, and gl-values.
 *
 * C++11
 *   l-value reference(before C++11, only this)
 *     - can be initialized(by l-value, r-value) and modified
 *     - l-value reference to const, can be initialized only
 *   r-value reference
 *     - can be initialized(by r-value) and modified
 *     - r-value reference to const, can be initialized only
 *     - Note:
 *       - extend r-value lifespan to the lifespan of r-value reference
 *       - non-const r-value is allowed to be modified
 *
 * NOTE:
 *   Should never return an r-value reference and an l-value reference.
 *   In most caes, it will end up returning a hanging reference when referenced
 *   object goes out of scope at the end of the function.
 */
namespace Test2
{
    // NOTE:
    // when initializing an r-value with a literal, a temporary is constructed
    // from the literal so that the reference is referencing a temporary object,
    // not a literal value.
    // r-value is not often used like this.
    void sp_rvalue_init(void)
    {
        int &&rr = 1;
        std::cout << rr << "\n";
        rr = 2;
        std::cout << rr << "\n";
    } // rr goes out of scope here

    void fun(const int &lref) // l-value arguments will select this function
    {
        std::cout << "l-value reference to const\n";
    }

    void fun(int &&rref) // r-value argument will select this function
    {
        std::cout << "r-value reference\n";
    }

    void sp_rvalue_func_param(void)
    {
        int x = 5;
        fun(x); // l-value argument calls l-value version of function
        fun(5); // r-value argument calls r-value version of function
                // this is considered a better match than l-value const ref
    }

    void fn(void)
    {
        sp_rvalue_init();
        sp_rvalue_func_param();
    }
}

/*
 * === Test 3: move constructor and move assignment for r-value ===
 *
 * C++11 defines two new functions in service of move semantics:
 *   a move constructor, and a move assignment operator.
 *
 * the copy flavors of these functions take a const l-value reference parameter
 * the move flavors of these functions use non-const r-value reference parameters
 *
 * How to call move functions:
 *    If defined move functions and those arguments are r-value, literal or
 *    temporary value, move functions will be called.
 *
 *    If the class has no copy and move functions, move functions will be defined
 *    by default as shadow copy. So we have to define move functions by ourselves.
 *
 * When both copy and move functions are available, C++11 do it smartly
 *   when l-value is passed, call the copy function.
 *   when r-value is passed, call the move function.
 *
 * Return variable is moved instead of copied, even though it's l-value.
 *   The C++ specification has a special rule that says automatic objects returned
 *   from a function by value can be moved even if they are l-values.
 *   Because the return variable will be released when the function returns.
 *
 * Disable copy:
 *   If it's desired to use move only, we can disable copy function as
 *     Auto_ptr5(const Auto_ptr5& a) = delete;
 *   Then the copy calling in the program will cause compiling error.
 *   e.g. pass l-value
 *
 * ----------------------------------------------------------------------------
 * copy constructor and copy assignment (case 1)
 *
 * do everything as copy
 *   - temporary return value(some compiler will treat it as move)
 *   - assign an object with another object
 *
 * result:
 *   Resource acquired -> create res in generateResource3()
 *   Resource acquired -> assign to mainres as copy, create a temporary Resource
 *   Resource destroyed -> release temporary Resource after the assignment
 *   Resource destroyed -> release mainres before leaves sp_copy()
 * example result: (return value is doing copy as well)
 *   Resource acquired -> create res in generateResource3()
 *   Resource acquired -> do copy constructor before return res
 *   Resource destroyed -> out of generateResource3(), release temporary return value
 *   Resource acquired -> assign to mainres as copy, create a temporary Resource
 *   Resource destroyed -> release temporary Resource after the assignment
 *   Resource destroyed -> release mainres before leaves sp_copy()
 *
 * do r-value as move (case 2)
 *   - temporary return value
 *   - assign an object with another object
 *
 * result:
 *   Resource acquired
 *   Resource acquired
 */
namespace Test3
{
    template<class T>
    class Auto_ptr3
    {
        T* m_ptr;
    public:
        Auto_ptr3(T* ptr = nullptr)
            :m_ptr(ptr)
        {
        }

        ~Auto_ptr3()
        {
            delete m_ptr;
        }

        // Copy constructor
        // Do deep copy of a.m_ptr to m_ptr
        Auto_ptr3(const Auto_ptr3& a)
        {
            m_ptr = new T;
            *m_ptr = *a.m_ptr;
        }

        // Copy assignment
        // Do deep copy of a.m_ptr to m_ptr
        Auto_ptr3& operator=(const Auto_ptr3& a)
        {
            // Self-assignment detection
            if (&a == this)
                return *this;

            // Release any resource we're holding
            delete m_ptr;

            // Copy the resource
            m_ptr = new T;
            *m_ptr = *a.m_ptr;

            return *this;
        }

        T& operator*() const { return *m_ptr; }
        T* operator->() const { return m_ptr; }
        bool isNull() const { return m_ptr == nullptr; }
    };

    class Resource
    {
    public:
        Resource() { std::cout << "Resource acquired\n"; }
        ~Resource() { std::cout << "Resource destroyed\n"; }
    };

    Auto_ptr3<Resource> generateResource3()
    {
        Auto_ptr3<Resource> res(new Resource);
        return res; // this return value will invoke the copy constructor
    }

    void sp_copy(void)
    {
        Auto_ptr3<Resource> mainres;
        mainres = generateResource3(); // this assignment will invoke the copy assignment
    }

    // ------------------------------------------------------------------------
    template<class T>
    class Auto_ptr4
    {
        T* m_ptr;
    public:
        Auto_ptr4(T* ptr = nullptr)
            :m_ptr(ptr)
        {
        }

        ~Auto_ptr4()
        {
            delete m_ptr;
        }

        // Copy constructor
        // Do deep copy of a.m_ptr to m_ptr
        Auto_ptr4(const Auto_ptr4& a)
        {
            m_ptr = new T;
            *m_ptr = *a.m_ptr;
        }

        // Move constructor
        // Transfer ownership of a.m_ptr to m_ptr
        Auto_ptr4(Auto_ptr4&& a)
            : m_ptr(a.m_ptr)
        {
            a.m_ptr = nullptr; // NOTE: avoid later calling a dangling pointer
        }

        // Copy assignment
        // Do deep copy of a.m_ptr to m_ptr
        Auto_ptr4& operator=(const Auto_ptr4& a)
        {
            // Self-assignment detection
            if (&a == this)
                return *this;

            // Release any resource we're holding
            delete m_ptr;

            // Copy the resource
            m_ptr = new T;
            *m_ptr = *a.m_ptr;

            return *this;
        }

        // Move assignment
        // Transfer ownership of a.m_ptr to m_ptr
        Auto_ptr4& operator=(Auto_ptr4&& a)
        {
            // Self-assignment detection
            if (&a == this)
                return *this;

            // Release any resource we're holding
            delete m_ptr;

            // Transfer ownership of a.m_ptr to m_ptr
            m_ptr = a.m_ptr;
            a.m_ptr = nullptr; // we'll talk more about this line below

            return *this;
        }

        T& operator*() const { return *m_ptr; }
        T* operator->() const { return m_ptr; }
        bool isNull() const { return m_ptr == nullptr; }
    };

    Auto_ptr4<Resource> generateResource4()
    {
        Auto_ptr4<Resource> res(new Resource);
        return res; // this return value will invoke the move constructor
    }

    void sp_move(void)
    {
        Auto_ptr4<Resource> mainres;
        mainres = generateResource4(); // this assignment will invoke the move assignment
    }

    void fn(void)
    {
        std::cout << "<<< do copy >>>\n";
        sp_copy();

        std::cout << "<<< do move >>>\n";
        sp_move();
    }
}

int main()
{
    run(1, &(Test1::fn)); // smart pointer and move semantics
    run(2, &(Test2::fn)); // r-value
    run(3, &(Test3::fn)); // move constructor and move assignment for r-value
}
