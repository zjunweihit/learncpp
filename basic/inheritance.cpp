#include <iostream>

/*
 * === Test 1: inheritance order ===
 *
 * 1. Derived class contents Base + Derived
 * 2. It also initialize in this order base, derived
 */
namespace Test1
{
    class Base
    {
    private:
        int m_id;

    public:
        Base(int id = 0)
            : m_id(id)
        {
            std::cout << "--> base contructor\n";
        }

        int getId() const { return m_id; }
    };

    class Derived: public Base
    {
    private:
        double m_cost;

    public:
        Derived(double cost = 0.0, int id = 0)
            : Base(id), m_cost(cost)
        {
            std::cout << "--> derived contructor\n";
        }

        double getCost() const { return m_cost; }
    };

    void run(void)
    {
        std::cout << "=== Test 1 ===\n\n";

        std::cout << "Instantiating Base\n";
        Base cBase;

        std::cout << "Instantiating Derived\n";
        Derived cDerived;

        std::cout << "\n=== Test 1: end ===\n\n";
    }
}

/*
 * === Test 2: inheritance accesor specifiers ===
 *
 * pubic, private, protected
 */
namespace Test2
{
    class Base
    {
    public:
        int m_public;
    private:
        int m_private;
    protected:
        int m_protected;
    };

    // Public inheritance
    // public -> inaccessible
    // private -> inaccessible
    // protected -> inaccessible
    class D2_Pri : private Base // private inheritance
    {
    public:
        D2_Pri()
        {
            //m_public = 1;
            //m_private = 2;
            //m_protected = 3;
        }
    };

    // Public inheritance
    // public -> public
    // private -> inaccessible
    // protected -> protected
    class D2_Pro : private Base // protected inheritance
    {
    public:
        D2_Pro()
        {
            m_public = 1;
            //m_private = 2;
            m_protected = 3;
        }
    };

    // Public inheritance
    // public -> protected
    // private -> inaccessible
    // protected -> protected

    class D2_Pub : private Base // public inheritance
    {
    public:
        D2_Pub()
        {
            m_public = 1;
            //m_private = 2;
            m_protected = 3;
        }
    };

    void run(void)
    {
        std::cout << "=== Test 2 ===\n\n";

        D2_Pri pri;
        D2_Pro pro;
        D2_Pub pub;

        std::cout << "\n=== Test 2: end ===\n\n";
    }
}

/*
 * === Test 3: inheritance order ===
 *
 * inherited functions, overriding, hiding
 */

namespace Test3
{
    class Base
    {
    protected:
        int m_value;
        void proFun() { std::cout << "Base protected function\n"; }

    public:
        Base(int value)
            : m_value(value)
        {
        }

        // only in base class
        // Note: if it's defined as private func, it will cause compiling error
        //       since, it cannot be accessed by Derived class, if Derived class
        //       calls it.
        void identify() { std::cout << "The function in Base only\n"; }

        // will be overided by Derived class
        void print() { std::cout << "I am a printer in Base\n"; }

        // will be overided by Derived class
        void print2() { std::cout << "I am another printer in Base\n"; }

        // will be hided by Derived class
        void hide() { std::cout << "show in Base, hide in Derived\n"; }

        // friend function for Base, but not belongs to Base
        friend std::ostream& operator<< (std::ostream &out, const Base &b)
        {
            out << "In Base\n";
            out << b.m_value << '\n';
            return out;
        }
    };

    class Derived: public Base
    {
    private:
        int m_cost;

    public:
        // Note: we cannot initialize m_value using intialization list
        // calling Base(value) or assigning value in constructor function
        // Since it's Base member (Base must intialize it)
        Derived(int value)
            : Base(value), m_cost(2 * value)
        {
        }

        // overriding Base class
        // Note: access specifier is decided by Derived class rather than Base class
        void print() { std::cout << "I am a printer in Derived\n"; }

        // Another print function to call Base function as well.
        // better to specify the namespace clearly.
        // can we call print2() by Base implicitly? segment fault in MacOS
        void print2()
        {
            Base::print2();
            std::cout << "I am another printer in Derived\n";
        }

        // a new function in Derived class
        void getValue() { std::cout << "created in Derived "<< m_cost << "\n"; }

        // a hided function cannot be called by Derived class
        void hide() = delete;

        // friend function to call Base value
        friend std::ostream& operator<< (std::ostream &out, const Derived &d)
        {
            out << "In Derived\n";
            // static_cast Derived to a Base object, so we call the right version of operator<<
            out << static_cast<Base>(d); 
            return out;
        }

        // change protected function to public, called by others.
        // Note: no parenthesis here.
        using Base::proFun;
    };

    void run(void)
    {
        std::cout << "=== Test 3 ===\n\n";

        Base base(5);
        Derived derived(7);

        derived.identify(); // exist in base class only, it should be not private
        derived.print();    // overided by Derived class
        derived.print2();   // overided by Derived class, calling Base print
        derived.getValue(); // a new function in Derived class
        //derived.hide();   // it's hided in Derived class, not compiling
        std::cout << derived; // friend func to call Base value
        derived.proFun();   // call a Base protected function

        std::cout << "\n=== Test 3: end ===\n\n";
    }
}

int main()
{
    Test1::run(); // inheritance order
    Test2::run(); // accesor specifiers, compiling only
    Test3::run(); // accesor specifiers, compiling only
}

