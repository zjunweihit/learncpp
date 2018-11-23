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

int main()
{
    run(1, &(Test1::fn)); // template function and class
}
