#include "global.h"
#include "math.h"
#include <exception>    // std::exception
#include <stdexcept>    // std::runtime_error
#include <utility>      // std::move
#include <memory>       // std::unique_ptr, std::shared_ptr

/*
 * === Test 1: smart pointer and move semantics ===
 *
 * A smart pointer is a class holding a pointer manage the allocated memory pointed
 * by the pointer. Free the resource when it goes out of scope.
 *
 * std::auto_ptr: avoid it
 *   Introduced in C++98, which is like Auto_ptr2, but with many problems.
 *   1) It implements move sematics through copy and assignment operator, so
 *      pass a std::auto_ptr by value to a function will cause resource to get
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

/*
 * === Test 4: std::move for l-value ===
 *
 * In C++11, std::move converts input l-value to r-value reference, invoking
 * move constructor or move assignment
 *
 * #include <utility>
 *
 * template<class T>
 * void swap(T& a, T& b)
 * {
 *      T tmp { std::move(a) }; // invokes move constructor
 *      a = std::move(b); // invokes move assignment
 *      b = std::move(tmp); // invokes move assignment
 * }
 *
 * Note:
 *   - Don't to use std::move to persistent object don't want to modify.
 *   - The state of objects applied std::move would be changed from previous one.
 *   - After move, the object should be in well-defined state, null or zero
 *     for later use or check.
 *   - std::move can be used for sorting an array of elements by move swapping.
 */
namespace Test4
{
    void fn(void)
    {
        std::vector<std::string> v;
        std::string str = "Knock";

        std::cout << "Copying str to vector\n";
        v.push_back(str); // calls l-value version of push_back, which copies str into the array element

        std::cout << "str: " << str << '\n';
        std::cout << "vector: " << v[0] << '\n';

        std::cout << "\nMoving str to vector\n";

        v.push_back(std::move(str)); // calls r-value version of push_back, which moves str into the array element

        std::cout << "str: " << str << '\n';
        std::cout << "vector:" << v[0] << ' ' << v[1] << '\n';
    }
}

/*
 * === Test 5: std::unique_ptr ===
 *
 * [move only]
 *
 * Implement only move semantics inside, using a = std::move(b),
 * copy semantics will cause compiling error, e.g. a = b.
 *
 * [access]
 *
 * Before accessing to a std::unique_ptr resource, we should check if it has actually
 * resource. It implicitly casts std::unique_ptr to bool when check in if-statement.
 *
 * [array]
 *
 * Unlike std::auto_ptr, std::unique_ptr knows whether to use scalar delete
 * or array delete.
 * NOTE: std::array and std::vector are better than std::unique_ptr
 *       (fixed/dynamic) array or C-string
 *
 * [std::make_unique]
 *
 * C++14 uses it to create unique pointer simply, optional, but recommended
 * Furthermore it resolves an exception safety issue that can result from
 * C++ leaving the order of evaluation for function arguments unspecified.
 * e.g.
 * class Fraction
 * { ...
 *      Fraction(int numerator = 0, int denominator = 1) :
 *          m_numerator(numerator), m_denominator(denominator)
 * }
 * // Create a single dynamically allocated Fraction with numerator 3 and denominator 5
 *  std::unique_ptr<Fraction> f1 = std::make_unique<Fraction>(3, 5);
 *
 * // Create a dynamically allocated array of Fractions of length 4
 * // We can also use automatic type deduction to good effect here
 * auto f2 = std::make_unique<Fraction[]>(4);
 *
 * [The exception safety issue]
 *
 * some_function(std::unique_ptr<T>(new T), function_that_can_throw_exception());
 * create new T and call function_that_can_throw_exception(), throwing an exception.
 * T will not be deallocated. Because smart pointer std::unique_ptr is not created yet.
 *
 * But std::make_unique makes object T and std::unique_ptr in side itself
 *
 * [return value]
 *
 * In functino return value, if the value is not assigned to anyone, the temporary
 * return value will be out of scope and released automatically. If it's assigned
 * to someone, it will call move assignment.
 *
 * [pass std::unique_ptr to a function]
 *
 * Note: copy is disabled, so move value to the function passing the value
 * pass by value, the function will get the moved value
 * pass by reference, the caller may change the object after calling the function
 *
 * // The function only uses the resource, so we'll accept a pointer to the resource,
 * // not a reference to the whole std::unique_ptr<Resource>
 * void useResource(Resource *res)
 * { ... }
 *
 * auto ptr = std::make_unique<Resource>();
 * useResource(ptr.get()); // note: get() used here to get a pointer to the Resource
 *
 * [std::unique_ptr to a class]
 *
 * it works as a memboer of a class, it will be destroyed when the class object
 * is destroyed. However, if the class object is dynamically allocated, the object
 * is risky not being properly deallocated, in which case even a smart pointer won't help.
 *
 * [misusing]
 *
 * Don't share the same resource
 * e.g.
 *   Resource *res = new Resource;
 *   std::unique_ptr<Resource> res1(res);
 *   std::unique_ptr<Resource> res2(res);
 *
 * Don't delete the resource from std::unique_ptr, who will delete it again,
 * leading undefined behavior.
 *
 * Note:
 *   std::make_unique() prevents both of the above cases from happening inadvertently.
 */
namespace Test5
{
    class Resource
    {
    private:
        int m_value;
    public:
        Resource() { std::cout << "Resource acquired\n"; }
        Resource(int value = 0)
            : m_value(value)
        {
            std::cout << "Resource acquired\n";
        }
        ~Resource() { std::cout << "Resource destroyed\n"; }

        // NOTE: print value that res is owning
        friend std::ostream& operator<< (std::ostream& out, const Resource &res)
        {
            out << res.m_value;
            return out;
        }
    };

    void sp_unique_ptr(void)
    {
        std::unique_ptr<int> p1(new int(8));
        std::unique_ptr<int> p2(new int(6));
        //std::unique_ptr<int> p3(p1);  // Error, Init cannot copy unique_ptr
        //std::unique_ptr<int> p4 = p1; // Error, Init cannot copy unique_ptr
        std::cout << "get p1 value " << *(p1.get()) << "\n";
        std::cout << "get p1 value " << *p1 << "\n";
        std::cout << "p1.get() returns p1 pointer, equals p1\n";

        // set internal pointer null, return pointed object, which SHOULD be released manually.
        int *tmp = p1.release();
        delete tmp;

        // set internal pointer null, free pointed object.
        p2.reset();
        std::cout << "reset p2 NULL: " << (static_cast<bool>(p2) ? "not null\n" : "null\n");
        p2.reset(new int(9));
        std::cout << "reset p2 value 9: " << *p2 << "\n";
    }

    void sp_unique_ptr_move(void)
    {
        std::unique_ptr<Resource> res1(new Resource(5)); // Resource created here
        std::unique_ptr<Resource> res2; // Start as nullptr

        std::cout << "res1 is " << (static_cast<bool>(res1) ? "not null\n" : "null\n");
        std::cout << "res2 is " << (static_cast<bool>(res2) ? "not null\n" : "null\n");
        if (res1) // NOTE: use implicit cast to bool to ensure res contains a Resource
            std::cout << "res1 value is " << *res1 << "\n";
        if (res2)
            std::cout << "res2 value is " << *res2 << "\n";

        // res2 = res1; // NOTE: Won't compile: copy assignment is disabled
        res2 = std::move(res1); // res2 assumes ownership, res1 is set to null

        std::cout << "Ownership transferred\n";

        std::cout << "res1 is " << (static_cast<bool>(res1) ? "not null\n" : "null\n");
        std::cout << "res2 is " << (static_cast<bool>(res2) ? "not null\n" : "null\n");

        if (res1)
            std::cout << "res1 value is " << *res1 << "\n";
        if (res2)
            std::cout << "res2 value is " << *res2 << "\n";
    } // Resource destroyed here when res2 goes out of scope

    void fn(void)
    {
        std::cout << "<<< unique pointer >>>\n";
        sp_unique_ptr();

        std::cout << "<<< move unique_ptr >>>\n";
        sp_unique_ptr_move();
    }
}

/*
 * === Test 6: std::shared_ptr ===
 *
 * multiple smart pointer could point to the same resource, it can track the
 * number of users accessing to the resource. If any user is accessing to the
 * resource, it won'twill be deallocated until the last user goes out of scope.
 *
 * Requires <memory> as std::unique_ptr
 *
 * NOTE:
 *   Aways do a copy of std::shared_ptr, if need to share the same resource.
 *   rather than share the resouce directly.
 *
 * Like std::make_unique(C++14), std::make_shared(C++11) is used to create
 * std::shared_ptr.
 *
 * [a deep look at std::shared_ptr]
 *
 * std::shared_ptr has two pointers
 *   - one points to the resource, passed in
 *   - the other points to the control block, created by constructor
 * If pass the resource to the std::shared_ptr, each shared pointer's individual
 * control block will think it's belong to itself. But if pass the shared pointer,
 * copy assignment, the data in control block will be updated to indicate there
 * are an addtional shared pointer co-managing the resource.
 *
 * [convert std::unique_ptr to std::shared_ptr]
 *
 * std::unique_ptr could be assigned to std::shared_ptr via a special constructor
 * accepting a std::unique_ptr r-value. Then std::unique_ptr moves to std::shared_ptr.
 * But not vice versa. So a function is better to return std::unique_ptr, that
 * could be assigned to a std::shared_ptr.
 *
 * [The perils of std::shared_ptr]
 *
 * If a std::unique_ptr doesn't delete the resource after initialization dynamically
 * allocating, the std::shared_ptr could not manage the resource properly.
 * Now we have to take care of any shared pointer sharing the resource.
 *
 * [Array]
 *
 * In C++14 and earlier, std::shared_ptr does not have proper support for
 * managing arrays, and should not be used to manage a C-style array.
 * As of C++17, std::shared_ptr does have support for arrays. However,
 * as of C++17, std::make_shared is still lacking proper support for arrays,
 * and should not be used to create shared arrays. This will likely be addressed
 * in C++20.
 */
namespace Test6
{
    class Resource
    {
    public:
        Resource() { std::cout << "Resource acquired\n"; }
        ~Resource() { std::cout << "Resource destroyed\n"; }
    };

    void sp_shared_ptr(void)
    {
        // allocate a Resource object and have it owned by std::shared_ptr
        Resource *res = new Resource;
        std::shared_ptr<Resource> ptr1(res);
        {
            // use copy initialization to make another std::shared_ptr pointing to the same thing
            // NOTE: initialized by a shared pointer, instead of Resource
            std::shared_ptr<Resource> ptr2(ptr1);

            // If so, it will free the resource when ptr2 is out of scope.
            // then later ptr1 goes out of scope, the deallocation of the same
            // resource will crash the program.
            //std::shared_ptr<Resource> ptr2(res);

            std::cout << "Killing one shared pointer\n";
        } // ptr2 goes out of scope here, but nothing happens

        std::cout << "Killing another shared pointer\n";
    } // ptr1 goes out of scope here, and the allocated Resource is destroyed

    void sp_make_shared(void)
    {
        // allocate a Resource object and have it owned by std::shared_ptr
        auto ptr1 = std::make_shared<Resource>();
        {
            auto ptr2 = ptr1; // create ptr2 using copy initialization of ptr1

            std::cout << "Killing one shared pointer\n";
        } // ptr2 goes out of scope here, but nothing happens

        std::cout << "Killing another shared pointer\n";
    } // ptr1 goes out of scope here, and the allocated Resource is destroyed

    void fn(void)
    {
        std::cout << "<<< shared pointer >>>\n";
        sp_shared_ptr();

        std::cout << "<<< make shared pointer >>>\n";
        sp_make_shared();
    }
}

/*
 * === Test 7: std::weak_ptr ===
 *
 * [Circular reference]
 *
 * also called cyclical reference or cycle. is a series of reference where each
 * object references the next, and the last object references back to the first.
 * It can be pointer, unique IDs and other means of identifying specific object.
 * (for shared pointers, the reference is a pointer.
 *
 * [std::weak_ptr]
 *
 * A std::weak_ptr can observe and access the same object as a std::shared_ptr
 * (or other std::weak_ptr) but it is not considered an owner and taken into
 * acount for the object owning.
 *
 * std::weak_ptr is not directly used, but converted into a std::shared_ptr
 * by lock() member function.
 *
 * [A reductive case]
 *
 * Although it's less possible to use that. A std::shared_ptr points to itself.

   class Resource
   {
   public:
   std::shared_ptr<Resource> m_ptr; // initially created empty
   };
   auto ptr1 = std::make_shared<Resource>();
   ptr1->m_ptr = ptr1; // m_ptr is now sharing the Resource that contains it

 *
 *
 */
namespace Test7
{
    class Person
    {
        std::string m_name;

        //std::shared_ptr<Person> m_partner; // initially created empty

        std::weak_ptr<Person> m_partner; // note: This is now a std::weak_ptr

    public:

        Person(const std::string &name) : m_name(name)
        {
            std::cout << m_name << " created\n";
        }
        ~Person()
        {
            std::cout << m_name << " destroyed\n";
        }

        friend bool partnerUp(std::shared_ptr<Person> &p1, std::shared_ptr<Person> &p2)
        {
            if (!p1 || !p2)
                return false;

            p1->m_partner = p2;
            p2->m_partner = p1;

            std::cout << p1->m_name << " is now partnered with " << p2->m_name << "\n";

            return true;
        }

        // NOTE: use lock() to convert weak_ptr to shared_ptr for access
        const std::shared_ptr<Person> getPartner() const { return m_partner.lock(); } 

        const std::string& getName() const { return m_name; }
    };

    // lucy -> Lucy
    // ricky.m_partner -> Lucy
    // ricky -> Ricky
    // lucy.m_partner -> Ricky
    //
    // [std::shared_ptr<Person> m_partner]
    //
    // when lucy goes out of scope, lucy is destroyed, but don's deallocate Lucy
    // as ricky.m_partner is pointing Lucy.
    // So does Ricky. At last both Lucy and Ricky are not deallocated.
    //
    // [std::weak_ptr<Person> m_partner] could save it
    //
    // it can observe and access the object as std::shared_ptr but not an owner.
    void fn(void)
    {
        auto lucy = std::make_shared<Person>("Lucy");
        auto ricky = std::make_shared<Person>("Ricky");

        partnerUp(lucy, ricky);

        // NOTE: get std::weak_ptr converted to std::shared_ptr for access
        auto partner = ricky->getPartner();
        std::cout << ricky->getName() << "'s partner is: " << partner->getName() << '\n';
    }
}

int main()
{
    run(1, &(Test1::fn)); // smart pointer and move semantics
    run(2, &(Test2::fn)); // r-value
    run(3, &(Test3::fn)); // move constructor and move assignment for r-value
    run(4, &(Test4::fn)); // std::move for l-value
    run(5, &(Test5::fn)); // std::unique_ptr
    run(6, &(Test6::fn)); // std::shared_ptr
    run(7, &(Test7::fn)); // std::weak_ptr
}
