#include "global.h"
#include <string>

/*
 * === Test 1: virutal function and polymorphism ===
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
 * (1) If it's necessary to deallocate the memory, the own destructor is required.
 *     If it's in inheritance, the base destructor should be vitual.
 * (2) Whenever you are dealing with inheritance, should make any explicit
 *     destructors virtual.
 * (3) Rarely we may want to ignore the virtualization of a function.
 *     But you can call base function directly
 * (4) A base class destructor should be either public and virtual, or
 *     protected and nonvirtual, which smart pointor should be used.
 * Note:
 *   (1) If you intend your class to be inherited from, make sure your destructor is virtual.
 *   (2) If you do not intend your class to be inherited from, mark your class as final.
 *       This will prevent other classes from inheriting from it in the first place.
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

int main()
{
    run(1, &(Test1::fn)); // virtual function basis
    run(2, &(Test2::fn)); // override, final and covariant specifier
    run(3, &(Test3::fn)); // 
}
