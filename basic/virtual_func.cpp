#include "global.h"
#include <functional>

/*
 * overload(重载)
 *   same function name, but different parameters
 *   in a same class
 *
 * overriding(覆盖)
 *   same function name and parameters
 *   base class should be virtual
 *   in different class(base, derived)
 *
 * ref:
 *   https://stackoverflow.com/questions/429125/override-and-overload-in-c
 *   http://www.codeceo.com/article/cpp-overload-inherit-override.html
 */
/*
 * === Test 1: virutal function and polymorphism ===
 *
 * 1. Calling a virtual function resolves to the most-derived function that
 *    exists between the base and derived class, known as polymorphism.
 * 2. A derived function is considered a match if it has the *same signature*
 *    (name, parameter types, const type) and return type, called override.
 * 3. Works for *reference* and pointer
 * 4. If a function is marked as virtual, all matching overrides are also
 *    considered virtual, even if they are not explicitly marked as such
 * 5. Don't set virtual function to constructor and destructor
 * 6. Downside: ineffcient, allocate virtual table and search the function
 */
namespace Test1
{

    class A
    {
    public:
        virtual const char* getName() { return "A"; }
    };

    class B: public A
    {
    public:
        virtual const char* getName() { return "B"; }
    };

    class C: public B
    {
    public:
        virtual const char* getName() { return "C"; }
    };

    class D: public C
    {
    public:
        virtual const char* getName() { return "D"; }
    };

    void vf_basis(void)
    {
        C c;
        D d;
        A &rBase = c;
        std::cout << "reference Base is a " << rBase.getName() << '\n';

        A *pBase = &d;
        std::cout << "pointer to Base is a " << pBase->getName() << '\n';
    }

    class Animal
    {
    protected:
        std::string m_name;

        // We're making this constructor protected because
        // we don't want people creating Animal objects directly,
        // but we still want derived classes to be able to use it.
        Animal(std::string name)
            : m_name(name)
        { }

    public:
        std::string getName() { return m_name; }
        virtual const char* speak() { return "???"; }
    };

    class Cat: public Animal
    {
    public:
        Cat(std::string name)
            : Animal(name)
        { }

        virtual const char* speak() { return "Meow"; }
    };

    class Dog: public Animal
    {
    public:
        Dog(std::string name)
            : Animal(name)
        { }

        virtual const char* speak() { return "Woof"; }
    };

    void report(Animal &animal)
    {
        std::cout << animal.getName() << " says " << animal.speak() << '\n';
    }

    void vf_animal(void)
    {
        Cat cat("Mimi");
        Dog dog("Wangwang");

        report(cat);
        report(dog);
    }

    void fn(void)
    {
        std::cout << "\n <<< virtual function basis >>>\n";
        vf_basis();

        std::cout << "\n <<< virtual function animal >>>\n";
        vf_animal();
    }
};

/*
 * === Test 2: override, final, covariant specifier(C++11) ===
 *
 * overide:
 *   (1) use it as much as possible
 *   (2) applied to any override function by placing the specifier in the
 *       same place const would go.
 *       If the function is not going to be a override func, compiling error
 *   Note:
 *       Apply the override specifier to every intended override function you write.
 *
 * final:
 *   (1) don't use it so much in practice
 *   (2) the function will be override in later derived functions
 *
 * Covariant return types
 *   one special case in which a derived class virtual function override can
 *   have a different return type than the base class and still be considered
 *   a matching override.
 *   If the return type of a virtual function is a pointer or a reference to
 *   a class, override functions can return a pointer or a reference to a
 *   derived class. This is called Covariant return type.
 */
namespace Test2
{
    class A
    {
    public:
        virtual const char* getName1(int x) { return "A"; }
        virtual const char* getName2(int x) { return "A"; }
        virtual const char* getName3(void) { return "A"; }
    };

    class B : public A
    {
    public:
        virtual const char* getName1(int x) override { return "B"; }

        // comipling error, the function is not override the base function
        //virtual const char* getName2(short x) override { return "B"; }

        virtual const char* getName2(short x) { return "B"; }

        // note use of final specifier on following line
        // that makes this function no longer overridable
        virtual const char* getName3(void) override final { return "B"; }
    };

    // C is final class, not use it after "public B"
    class C final : public B
    {
    public:
        // compile error: overrides B::getName(), which is final
        //virtual const char* getName3(void) override { return "C"; }
    };

    // compiling error: C is the final class, cannot inherit from it.
    //class D : public C
    //{
    //};

    void vf_override_final(void)
    {
        B b;
        A &rBase = b;
        std::cout << "call base getName1(override): " << rBase.getName1(1) << '\n';
        std::cout << "call base getName2(no override): " << rBase.getName2(2) << '\n';
    }

    class Base
    {
    public:
        // This version of getThis() returns a pointer to a Base class
        virtual Base* getThis() { std::cout << "called Base::getThis()\n"; return this; }
        void printType() { std::cout << "returned a Base\n"; }
    };

    class Derived : public Base
    {
    public:
        // Normally override functions have to return objects of the same type as the base function
        // However, because Derived is derived from Base, it's okay to return Derived* instead of Base*
        virtual Derived* getThis() { std::cout << "called Derived::getThis()\n";  return this; }
        void printType() { std::cout << "returned a Derived\n"; }
    };

    // We call b->getThis(). Variable b is a Base pointer to a Derived object.
    // Base::getThis() is virtual function, so this calls Derived::getThis().
    // Although Derived::getThis() returns a Derived*, because base version of
    // the function returns a Base*, the returned Derived* is downcast to a
    // Base*. And thus, Base::printType() is called.
    void vf_covariant(void)
    {
        Derived d;
        Base *b = &d;
        d.getThis()->printType(); // calls Derived::getThis(), returns a Derived*, calls Derived::printType
        b->getThis()->printType(); // calls Derived::getThis(), returns a Base*, calls Base::printType
    }

    void fn(void)
    {
        std::cout << "\n <<< virtual function override and final >>>\n";
        vf_override_final();

        std::cout << "\n <<< virtual function covariant >>>\n";
        vf_covariant();
    }
}

/*
 * === Test 3: destructure function ===
 *
 * (1) If it's necessary to deallocate the memory, the own destructor is required.
 *     If it's in inheritance, the base destructor should be vitual.
 * (2) Whenever you are dealing with inheritance, should make any explicit
 *     destructors virtual.
 * (3) Rarely we may want to ignore the virtualization of a function.
 *     But you can call base function directly
 * (4) A base class destructor should be either public and virtual, or
 *     protected and nonvirtual, which smart pointor should be used.
 * Note:
 *   - If you intend your class to be inherited from, make sure your destructor is virtual.
 *   - If you do not intend your class to be inherited from, mark your class as final.
 *     This will prevent other classes from inheriting from it in the first place.
 */
namespace Test3
{
    class Base
    {
    public:
        //~Base() // not virtual, it will be called
        virtual ~Base() // virtual, the derived destructor will be called firstly
        {
            std::cout << "Calling ~Base()" << std::endl;
        }

        virtual const char* getName() { return "Base"; }
    };

    class Derived: public Base
    {
    private:
        int* m_array;

    public:
        Derived(int length)
        {
            m_array = new int[length];
        }

        //~Derived() // note: not virtual
        virtual ~Derived() // note: virtual, it will be called
        {
            std::cout << "Calling ~Derived()" << std::endl;
            delete[] m_array;
        }

        virtual const char* getName() { return "Derived"; }
    };

    void fn(void)
    {
        Derived *derived = new Derived(5);
        Base *base = derived ;

        std::cout << "calling base function directly, ignoring virtualization\n";
        std::cout << base->Base::getName() << "\n";

        std::cout << "\ndestructure Derived object by Base pointer\n";
        delete base;
    }
}
/*
 * === Test 4: virutal table ===
 *
 * Early binding: when compiling, it knows where is the function address
 * Late binding: when program is running, it knows the function address
 -
 *   // Set pFcn to point to the function the user chose
 *   switch (op)
 *   {
 *       case 0: pFcn = add; break;
 *       case 1: pFcn = subtract; break;
 *       case 2: pFcn = multiply; break;
 *   }
 *
 *   // Call the function that pFcn is pointing to with x and y as parameters
 *   // This uses late binding
 *   std::cout << "The answer is: " << pFcn(x, y) << std::endl;
 -
 * The virtual table is a lookup table of functions used to resolve function
 * calls in a dynamic/late binding manner.
 * it's also named, such as “vtable”, “virtual function table”, “virtual method table”,
 * or “dispatch table”.
 * (1) every class that uses the virtual functions has its own virtual table
 *     - the virtual table is simply a static array, set up on compiling
 *     - each entry is a function pointer to the most-derived accessible function
 * (2) compiler adds a hidden pointer to the base class, *__vptr.
 *     - *__vptr is set automatically when a class instance is created.
 *     - unlike *this, which is a function parameter used by compiler to resolve
 *       self-references, *__vptr is a real pointer, each class allcats one more
 *       pointer size.
 *     - *__vptr is inherited by derived classes.
 * Calling a virtual function is slower than a non-virtual function.
 * Any class that uses virtual funtions has a __vptr, bigger by 1 pointer.
 * But for moden computer, it's much less significant, although still has perf cost.
 */
namespace Test4 // compiling only for now
{
    /*
     * Compiler creates 3 virtual tables for each class(Base, D1, D2)
     * and adds a hidden pointer to the most base class(Base).
     *
     * When a class object is created, *__vptr is set to point to the virtual
     * table for that class.
     */

    class Base
    {
    public:
        //FunctionPointer *__vptr;
        virtual void function1() {};
        virtual void function2() {};
    };

    class D1: public Base
    {
    public:
        virtual void function1() {};
    };

    class D2: public Base
    {
    public:
        virtual void function2() {};
    };
    //      +--------------------+
    //      |       Base         |
    //      |                    |         +--------------+
    //      | *__vptr(inherit) --+-------->| Base VTable  |
    // +--->| virtual function1()|<-----+  |              |
    // | +->| virtual function2()|<-+   +--+-function1()  |
    // | |  +--------------------+  +------+-function2()  |
    // | |                                 +--------------+
    // | +----------------------------------------------------+
    // |    +--------------------+                            |
    // |    | D1: public Base    |                            |
    // |    |                    |         +--------------+   |
    // |    | *__vptr(inherit) --+-------->| D1 VTable    |   |
    // |    | virtual function1()|<----+   |              |   |
    // |    +--------------------+     +---+-function1()  |   |
    // |                                   | function2()--+---+
    // |    +--------------------+         +--------------+
    // |    | D2: public Base    |
    // |    |                    |          +--------------+
    // |    | *__vptr(inherit) --+--------->| D2 VTable    |
    // |    | virtual function2()|<----+    |              |
    // |    +--------------------+     |    | function1()--+--+
    // |                               +----+-function2()  |  |
    // |                                    +--------------+  |
    // |                                                      |
    // +------------------------------------------------------+


    //
    // int main()
    // {
    //     D1 d1;
    //     Base *dPtr = &d1;
    // }
    //
    // Note that because dPtr is a base pointer, it only points to the Base portion of d1.
    // However, also note that *__vptr is in the Base portion of the class,
    // so dPtr has access to this pointer. Finally, note that dPtr->__vptr points
    // to the D1 virtual table! Consequently, even though dPtr is of type Base,
    // it still has access to D1’s virtual table (through __vptr).
}

/*
 * === Test 5: pure virutal function and abstract base class ===
 *
 * A pure virtual function (or abstract function) has no function body, which
 * is placeholder that will/MUST be implemented in derived classes.
 * i.e. Set to 0
 *
 *      virtual int getValue() = 0; // a pure virtual function
 *
 * A class has one or more pure virtual function is an abstract base class.
 * It means the class *cannot be instantiated*! Compiling error.
 * It can implement the function body outside of the class as a default implementation,
 * but it's still considered as an abstract base class.
 *
 * Abstract base class has virtual table as well.
 * The virtual table entry for a pure virtual function will generally either
 * contain a null pointer, or point to a generic function that prints an error
 * (sometimes this function is named __purecall) if no override is provided.
 */
namespace Test5
{
    class Animal // This Animal is an abstract base class
    {
    protected:
        std::string m_name;

    public:
        Animal(std::string name)
            : m_name(name)
        { }

        std::string getName() { return m_name; }

        virtual const char* speak() = 0; // a pure virtual function
    };

    // Although we can set function body for a pure virtual function
    // Animal is still a base class, cannot be instantiated.
    // If we have to do so, it must be defined out of class instead of inline function
    // It could provide a default implementation.
    const char* Animal::speak() { return "I'm an Animal"; }

    class Cow: public Animal
    {
    public:
        Cow(std::string name)
            : Animal(name)
        { }

        // If we forgot to redefine speak(), it will comiple error
        // error: cannot declare variable ‘cow’ to be of abstract type ‘Test5::Cow’
        // note:   because the following virtual functions are pure within ‘Test5::Cow’:
        // note:        virtual const char* Test5::Animal::speak()
        virtual const char* speak() { return "Moo"; }
    };

    class Dragonfly: public Animal
    {
    public:
        Dragonfly(std::string name)
            : Animal(name)
        { }
        virtual const char* speak() { return Animal::speak(); }
    };

    void fn(void)
    {
        // Compiling error as Animal is an abstract base class
        //Animal A("Buzz");

        Cow cow("Betsy");
        std::cout << "Create Cow from Animal abstract base class\n";
        std::cout << cow.getName() << " says " << cow.speak() << '\n';

        Dragonfly dfly("Sally");
        std::cout << "Create Dragonfly from Animal abstract base class\n";
        std::cout << dfly.getName() << " says " << dfly.speak() << '\n';
    }

    // Interface class: every function is virtual and must be implemented in
    // derived classes, even if it returns 0 directly.
    //
    // We can derive FileErrorLog and ScreenErrorLog from IErrorLog,
    // and resolve the Error handling in the same way regardless of input error
    // class
    //
    // class IErrorLog
    // {
    // public:
    //     virtual bool openLog(const char *filename) = 0;
    //     virtual bool closeLog() = 0;

    //     virtual bool writeError(const char *errorMessage) = 0;

    //     virtual ~IErrorLog() {}; // make a virtual destructor in case we delete an IErrorLog pointer, so the proper derived destructor is called
    // };
    //
    // double mySqrt(double value, IErrorLog &log)
    //{
    //    if (value < 0.0)
    //    {
    //        log.writeError("Tried to take square root of value less than 0");
    //        return 0.0;
    //    }
    //    else
    //        return sqrt(value);
    //}
}

/*
 * === Test 6: virutal base class ===
 *
 * For diamond issue of multiple inheritance, the base class will be constructed
 * many times(twice here), virtual base class will fix it and construct once.
 *
 * The most-derived class(Copier) creates the base class(PoweredDevice).
 *
 * 1) virtual base classes are always created before non-virtual base classes,
 *    which ensures all bases get created before their derived classes.
 * 2) note that the Scanner and Printer constructors still have calls to the
 *    PoweredDevice constructor. When creating an instance of Copier, these
 *    constructor calls are simply ignored because Copier is responsible for
 *    creating the PoweredDevice. But if we were to create an instance of
 *    Scanner or Printer, those constructors would be used.
 * 3) If a class inherits one or more classes that have virtual parents,
 *    the most derived class is reponsible for contructing the vitual base class.
 *    (Even though it's not a diamond inheritance.)
 * 4) a virtual base class is always considered a direct base of its most derived
 *    class. But classes inheriting the virtual base still need access to it.
 *    the compiler creates a virtual table for each class directly inheriting
 *    the virtual class (Printer and Scanner), pointing to the most derived class.
 */
namespace Test6
{

    class PoweredDevice
    {
    public:
        PoweredDevice(int power)
        {
            std::cout << "PoweredDevice: " << power << '\n';
        }
    };

    class Scanner: virtual public PoweredDevice // note: PoweredDevice is now a virtual base class
    {
    public:
        Scanner(int scanner, int power)
            : PoweredDevice(power) // this line is required to create Scanner objects, but ignored in this case
        {
            std::cout << "Scanner: " << scanner << '\n';
        }
    };

    class Printer: virtual public PoweredDevice // note: PoweredDevice is now a virtual base class
    {
    public:
        Printer(int printer, int power)
            : PoweredDevice(power) // this line is required to create Printer objects, but ignored in this case
        {
            std::cout << "Printer: " << printer << '\n';
        }
    };

    class Copier: public Scanner, public Printer
    {
    public:
        Copier(int scanner, int printer, int power)
            : PoweredDevice(power), // PoweredDevice is constructed here
            Scanner(scanner, power), Printer(printer, power)
        { }
    };

    void fn(void)
    {
        Copier copier(1, 2, 3);
    }
}

/*
 * === Test 7: object slicing ===
 *
 * The assigning of a Derived class object to a Base class object is called
 * object slicing (or slicing for short).
 *
 * It may happen when pass a parameter to a function. And it's hard to debug
 * if there is no print in virtual function.
 */
namespace Test7
{
    class Base
    {
    protected:
        int m_value;

    public:
        Base(int value)
            : m_value(value)
        { }

        virtual const char* getName() const { return "Base"; }
        int getValue() const { return m_value; }
    };

    class Derived: public Base
    {
    public:
        Derived(int value)
            : Base(value)
        { }

        virtual const char* getName() const { return "Derived"; }
    };

    //void printName(const Base &base) // passed by reference, fine
    void printName(const Base base) // passed by value, slicing
    {
        std::cout << "I am a " << base.getName() << "\n";
    }

    void vf_vector_slicing(void)
    {
        std::vector<Base> v;
        // NOTE:
        //   going to get the reference for derived object, but it doesn't work.
        //   the elements of std::vector must be assignable, while reference is
        //   cannot be reassigned (only initialized).
        //
        // std::vector<Base&> v;
        //
        // NOTE:
        //   using a Base pointer for std::vector is working, but it's complicated
        //   to manage the element's memory, at least should delete each one at last
        //
        // std::vector<Base*> v;
        // v.push_back(new Base(5));
        // for (int count = 0; count < v.size(); ++count)
        //      delete v[count];
        //
        // NOTE:
        //   Good news is that we can use std::reference_wrapper as reference
        //   and it's assignable.

        std::cout << "create Base(5)\n";
        v.push_back(Base(5)); // add a Base object to our vector

        std::cout << "create Derived(6)\n";
        v.push_back(Derived(6)); // add a Derived object to our vector

        // Print out all of the elements in our vector
        std::cout << "traverse the vector:\n";
        for (int count = 0; count < v.size(); ++count)
            std::cout << "\tI am a " << v[count].getName() << " with value "
                      << v[count].getValue() << "\n";
    }

    // Good news is that we can use std::reference_wrapper as reference
    // NOTE:
    //   1) std::reference_wrapper lives in the <functional> header
    //   2) the object cannot be anonymous (since anonymous objects have
    //      expression scope would leave the reference dangling).
    //   3) using get() to get the member
    void vf_vector_ref_wrapper(void)
    {
        std::cout << "create a vector by reference_wrapper\n";

        // our vector is a vector of std::reference_wrapper wrapped Base (not Base&)
        std::vector<std::reference_wrapper<Base>> v;

        // b and d can't be anonymous objects
        std::cout << "create Base(5) explicitly\n";
        Base b(5);
        std::cout << "create Derived(6) explicitly\n";
        Derived d(6);

        v.push_back(b); // add a Base object to our vector
        v.push_back(d); // add a Derived object to our vector

        // we use .get() to get our element from the wrapper
        std::cout << "traverse the vector:\n";
        for (int count = 0; count < v.size(); ++count)
            std::cout << "\tI am a " << v[count].get().getName() << " with value "
                      << v[count].get().getValue() << "\n";
    }

    // Frankenobject -- composed of parts of multiple objects
    void vf_frankenobject(void)
    {
        std::cout << "create d1 Derived(5)\n";
        Derived d1(5);

        std::cout << "create d2 Derived(6)\n";
        Derived d2(6);

        std::cout << "Base b gets Base slicing of d2(6)\n";
        Base &b = d2;

        std::cout << "assign d1 Base to b, i.e. d2\n";
        b = d1; // this line is problematic
        std::cout << "d2 is " << d2.getValue() << "\n";
    }

    void fn(void)
    {
        Derived derived(5);
        Base base = derived; // slice off the derived part for base

        std::cout << "Slicing assignment\n";
        std::cout << "base is a " << base.getName() << " and has value "
                  << base.getValue() << "\n";

        std::cout << "\nSlicing function\n";
        printName(derived);

        std::cout << "\nSlicing vector\n";
        vf_vector_slicing();
        vf_vector_ref_wrapper();

        std::cout << "\nSlicing Frankenobject\n";
        vf_frankenobject();
    }
}

/*
 * === Test 8: dynamic cast ===
 *
 * When to use it:
 *   derived class has a specific function, not vitual function,
 *   but we only have base reference or pointer, how to access the derived::function()
 *
 *   dynamic_cast converts base-class pointer into derived-class pointer.
 *
 * upcasting:
 *   C++ will implicitly let you convert a Derived pointer into a Base pointer
 *   (in fact, getObject() does just that). This process is called upcasting.
 * downcasting:
 *   dynamic_cast base pointer into derived pointer.
 *
 * dynamic_cast error check:
 *   If dynmaic_cast fails, it returns NULL pointer, e.g. pointer to a Base
 *   instead of Derived object.
 *
 * NOTE:
 *   there are several cases where downcasting using dynamic_cast will not work:
 *   1) With protected or private inheritance.
 *   2) For classes that do not declare or inherit any virtual functions
 *      (and thus don’t have a virtual table). Seems not??
 *   3) In certain cases involving virtual base classes. Seems not??
 *
 * Downcasting with static_cast:
 *   It cannot check in runtime, so if you can ensure it will always be sucess.
 *   static_cast is acceptable and faster.
 *   In case the Base pointer isn's pointing to a Derived object, will result in
 *   undefined behavior.
 *
 * When to use static_cast or dynamic_cast:
 *   if virtual function works, use virtual functions
 *   if it's for downcasting, use dynamic_cast
 *   otherwise, static_cast
 * Someone thinks dynmaic_cast is evil, should use virtual function
 * sometimes doencasting is a better choice:
 *   When you can not modify the base class to add a virtual function.
 *     (e.g. in standard library)
 *   When you need access to something that is derived-class specific.
 *     (access function in Derived class only)
 *   When adding a virtual function to your base class doesn’t make sense.
 *     (no appropriate value in base class, or use pure virtual function,
 *      if it doesn's need to instantiate the base class)
 */

namespace Test8
{
    // Class identifier for static_cast
    enum ClassID
    {
        BASE,
        DERIVED
            // Others can be added here later
    };

    class Base
    {
    protected:
        int m_value;

    public:
        Base(int value)
            : m_value(value)
        { }

        virtual ~Base() {}

        // verify virtual function for dynmaic_cast, compiling and executing are OK.
        virtual int getValue() { return m_value; }

        // static_cast identifier
        virtual ClassID getClassID() { return BASE; }
    };

    // compiling error if derived class inherits Base protectedly or privately.
    //class Derived : protected Base
    //class Derived : private Base
    //class Derived : virtual public Base // virtual class works??
    class Derived : public Base
    {
    protected:
        std::string m_name;

    public:
        Derived(int value, std::string name)
            : Base(value), m_name(name)
        { }

        // this specifc funtion for Derived class
        const std::string& getName() { return m_name; }

        // virtual function from Base
        virtual int getValue() { return m_value; }

        // static_cast identifier
        virtual ClassID getClassID() { return DERIVED; }
    };

    Base* getObject(bool bReturnDerived)
    {
        if (bReturnDerived)
            return new Derived(1, "Derived class");
        else
            return new Base(2);
    }

    void vf_dynamic_cast(void)
    {
        Base *b = getObject(true);

        Derived *d = dynamic_cast<Derived*>(b);

        // Error check: NULL if it fails, that b is pointing to a Base object
        // rather than Derived object.
        if (d) {
            std::cout << "The derived name is " << d->getName() << "\n";
            std::cout << "The derived value is " << d->getValue() << "\n";
            std::cout << "The base pointer's value is " << b->getValue() << "\n";
        }

        delete b;
    }

    void vf_static_cast(void)
    {
        Base *b = getObject(true);

        if (b->getClassID() == DERIVED)
        {
            // We already proved b is pointing to a Derived object, so this should always succeed
            Derived *d = static_cast<Derived*>(b);
            std::cout << "The name of the Derived is: " << d->getName() << '\n';
        }

        delete b;
    }

    void vf_dynamic_cast_ref(void)
    {
        std::cout << "create Derived object apple\n";
        Derived apple(1, "apple");

        std::cout << "create reference b for Base apple\n";
        Base &b = apple;

        std::cout << "dynamic_cast b by reference instead of pointer\n";
        // NOTE:
        //   There is no error check for NULL pointer, but an exception of
        //   std::bad_cast will be thrown, if it fails.
        Derived d = dynamic_cast<Derived&>(b);

        std::cout << "The derived name is " << d.getName() << "\n";
    }
    void fn(void)
    {
        std::cout << "dynamic cast\n";
        vf_dynamic_cast();

        std::cout << "\nstatic cast\n";
        vf_static_cast();

        std::cout << "\ndynamic cast by reference\n";
        vf_dynamic_cast_ref();
    }
}

/*
 * === Test 9: override << operator ===
 *
 * Friend function in Base class is not a member function and cannot be overrided
 * as a virtual function.
 *
 * Set operator<< in Base class and call the virtual function to Derived class.
 * It will not implement operator<< for each Derived class.
 */

namespace Test9
{
    class Base
    {
    public:
        Base() {}

        // Here's our overloaded operator<<
        friend std::ostream& operator<<(std::ostream &out, const Base &b)
        {
            // Delegate printing responsibility for printing to member function print()
            return b.print(out);
        }

        // We'll rely on member function print() to do the actual printing
        // Because print is a normal member function, it can be virtualized
        virtual std::ostream& print(std::ostream& out) const
        {
            out << "Base";
            return out;
        }
    };

    class Derived : public Base
    {
    public:
        Derived() {}

        // Here's our override print function to handle the Derived case
        virtual std::ostream& print(std::ostream& out) const override
        {
            out << "Derived";
            return out;
        }
    };

    void fn(void)
    {
        std::cout << "Override << operator\n";

        // call Base print() directly
        std::cout << "\n<< b:\n";
        Base b;
        std::cout << b << '\n';

        // Derived class has no operator<< , so call Base operator<<
        // and compiler does an implicit upcase of Derived object to Base object
        // Base.print() -> Derived.print()
        std::cout << "\n<< d:\n";
        Derived d;
        std::cout << d << '\n'; // note that this works even with no operator<< that explicitly handles Derived objects

        // Base.print() -> Derived.print()
        std::cout << "\n<< refrence base:\n";
        Base &bref = d;
        std::cout << bref << '\n';
    }
}

int main()
{
    run(1, &(Test1::fn)); // virtual function basis
    run(2, &(Test2::fn)); // override, final and covariant specifier
    run(3, &(Test3::fn)); // destructor function
    //run(4, &(Test4::fn)); // virutal table, compiling only for now
    run(5, &(Test5::fn)); // pure virtual function and abstract base class
    run(6, &(Test6::fn)); // virtual base class
    run(7, &(Test7::fn)); // object slicing
    run(8, &(Test8::fn)); // dynamic cast
    run(9, &(Test9::fn)); // override << operatior
}
