#include "global.h"
#include <string>

/*
 * === Test 1: function template ===
 *
 * 1. "Template": tells the compiler that what follows is going to be a list of
 *    template parameters, using "<>", template <typename T1, typename T2>
 * 2. Template type parameter could be keyword: typename or class. No diff.
 *    the class is nota class, it could be variable, point or anything else.
 * 3. T could be a class, so better to pass class by reference instead of value.
 *
 * 4. Template functions often print crazy-looking log to debug
 * 5. It may inscrease the compiling time and code size.
 * 6. 4, 5 are minor drawbacks with templacte power and flexibility
 *
 * 7. C++ does not compile the template function directly.
 *    At compile time, when the compiler encounters a call to a template function,
 *    it replicates the template function and replaces the template type
 *    parameters with actual types.
 *    The function with actual types is called a function *template instance*.
 *    And then compiles it.
 *
 *    1) The compiler is smart enough to know it only needs to create one
 *    template instance per set of unique type parameters (per file).
 *    2) If you create a template function but do not call it, no template
 *    instances will be created.
 */
namespace Test1
{
    /* template function */

    template <typename T> // this is the template parameter declaration
    const T& max(const T& x, const T& y)
    {
        return (x > y) ? x : y;
    }

    class Cents
    {
    private:
        int m_cents;
    public:
        Cents(int cents = 0)
            : m_cents(cents)
        {
        }

        friend bool operator> (const Cents &c1, const Cents &c2)
        {
            return (c1.m_cents > c2.m_cents);
        }

        friend std::ostream& operator<< (std::ostream& out, const Cents &c)
        {
            out << c.m_cents;
            return out;
        }
    };

    void template_function(void)
    {
        Cents nickle(5);
        Cents dime(10);

        std::cout << "nickle is " << nickle << "\n";
        std::cout << "dime is " << dime << "\n";
        std::cout << "The bigger one is " << max(nickle, dime) << "\n";

        std::cout << "\n";

        double a = 2.3;
        double b = 5.6;

        std::cout << "a is " << a << "\n";
        std::cout << "b is " << b << "\n";
        std::cout << "The bigger one is " << max(a, b) << "\n";
    }

    /* template class */

    template <class T> // no ";" at the end
    class Array
    {
    private:
        T *m_data;
        int m_count;

    public:
        Array()
        {
            m_data = nullptr;
            m_count = 0;
        }
        Array(int count)
        {
            assert(count > 0);
            m_data = new T[count];
            m_count = count;
        }
        ~Array()
        {
            delete [] m_data;
            // set null, otherwise it will point to the deallocated memory
            m_data = nullptr;
            m_count = 0;
        }

        T& operator[] (int index)
        {
            //assert(index >= 0 && index < m_count);
            return m_data[index];
        }

        int getCount(void);
    };

    // outside of class, the member function needs own template declaration
    template <typename T>
    // class name is Array<T>, not Array
    int Array<T>::getCount() { return m_count; }

    void template_class(void)
    {
        Array<int> intArray(8);
        Array<double> doubleArray(8);

        for (int i = 0; i < intArray.getCount(); ++i) {
            intArray[i] = i * 2;
            doubleArray[i] = i + 0.5;
        }

        for (int i = 0; i < intArray.getCount(); ++i)
            std::cout << intArray[i] << "\t" << doubleArray[i] << "\n";

        std::cout << intArray.getCount() << "\t" << doubleArray.getCount() << "\n";
    }

    void fn(void)
    {
        std::cout << "\n <<< template function >>>\n";
        template_function();

        std::cout << "\n <<< template class >>>\n";
        template_class();

        // Splitting up template classes into separate files, no exmaple here
    }
};

namespace Test2
{
    template <class T>
    class Storage
    {
    private:
        T m_value;
    public:
        Storage(T val) : m_value(val)
        {
        }

        void print(void)
        {
            std::cout << m_value << "\n";
        }
    };

    // The template <> tells the compiler that this is a template function,
    // but that there are no template parameters (since in this case,
    // we’re explicitly specifying all of the types).
    // Some compilers may allow you to omit this, but it’s proper to include it.
    template <>
    void Storage<double>::print(void)
    {
        std::cout << "double: " << std::scientific << m_value << "\n";
    }

    void func_template_specialization(void)
    {
        Storage<int> S1(123);
        Storage<double> S2(123.456);

        S1.print();
        S2.print();
    }

    // class template specification
    template <class T>
    class Storage8
    {
    private:
        T m_array[8];
    public:
        Storage8()
        {
            std::cout << "This is not bool type class\n";
        }
    };

    template <>
    class Storage8<bool>
    {
    // what follows is just standard class implementation details
    private:
        unsigned char m_data;
    public:
        Storage8()
        {
            std::cout << "This is bool type class\n";
        }
    };

    void class_template_specialization(void)
    {
        Storage8<int> intS;
        Storage8<bool> boolS;
    }

    void fn(void)
    {
        std::cout << "\n <<< function template specialization >>>\n";
        func_template_specialization();

        std::cout << "\n <<< class template specialization >>>\n";
        class_template_specialization();
        // omit the class template specification
    }
};

namespace Test3
{
    template <class T, int size>
    class Array
    {
    private:
        T m_array[size];
    public:
        T* getArray(void) { return m_array; }
        T& operator[](int index)
        {
            assert(index >= 0);
            return m_array[index];
        }
    };

    // a new template function rather than a member funtion
    // Note: it doesn's work for a string
    template <typename T, int size>
    void print(Array<T, size> &array)
    {
        for (int i = 0; i < size; ++i)
            std::cout << array[i] << ' ';
        std::cout << "\n";
    }

    // overload of print() function for partially specialized Array<char, size>
    // Note that as of C++14, partial template specialization can only be
    // used with classes, not template functions (functions must be fully
    // specialized).
    template <int size>
    void print(Array<char, size> &array)
    {
        for (int i = 0; i < size; ++i)
            std::cout << array[i];
        std::cout << "\n";
    }

    // Partial template specialization is not used all that often
    // (but can be useful in specific cases).
    void partial_template_func(void)
    {
        Array<int, 4> int4;

        for (int i = 0; i < 4; ++i)
            int4[i] = i;
        print(int4);

        Array<char, 14> char14;

        //strcpy_s(char14.getArray(), 14, "hello, world!"); // compiling error in Mac
        strncpy(char14.getArray(), "hello, world!", 14);
        //strcpy(char14.getArray(), "hello, world!");
        print(char14);
    }

    void fn(void)
    {
        partial_template_func();
        // skip Partial template specialization for member functions
    }
};

int main()
{
    run(1, &(Test1::fn)); // template function and class
    run(2, &(Test2::fn)); // template and class specialization
    run(3, &(Test3::fn)); // partial template function specialization
}
