#include "global.h"
#include <string>

/*
 * === Test 1: virutal function and polymorphism ===
 *
 * 1. Calling a virtual function resolves to the most-derived function that
 *    exists between the base and derived class, known as polymorphism.
 * 2. A derived function is considered a match if it has the same signature
 *    (name, parameter types, const type) and return type, called override.
 * 3. Works for reference and pointer
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
        virtual const char* speak() { Animal::speak(); }
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

int main()
{
    run(1, &(Test1::fn)); // virtual function basis
    run(2, &(Test2::fn)); // override, final and covariant specifier
    run(3, &(Test3::fn)); // destructor function
    //run(4, &(Test4::fn)); // virutal table, compiling only for now
    run(5, &(Test5::fn)); // pure virtual function and abstract base class
    run(6, &(Test6::fn)); // virtual base class
}
