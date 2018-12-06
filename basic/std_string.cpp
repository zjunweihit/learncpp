#include "global.h"
#include <sstream>

/*
 * Overview
 *
 * Header file <string>
 *
 * Basic string as template

 namespace std
 {
     template<class charT, class traits = char_traits<charT>, class Allocator = allocator<charT> >
     class basic_string;
 }

 * string and wstring, no utf-32 string by default, using the basic_string if any

 namespace std
 {
     typedef basic_string<char> string;     // utf-8
     typedef basic_string<wchar_t> wstring; // utf-16
 }

 * member functions

 capacity()         Returns the number of characters that can be held without reallocation
 empty()            Returns a boolean indicating whether the string is empty
 length(), size()   Returns the number of characters in string
 max_size()         Returns the maximum string size that can be allocated
 reserve()          Expand or shrink the capacity of the string

 [], at()           Accesses the character at a particular index

 =, assign()        Assigns a new value to the string
 +=, append(), push_back()   Concatenates characters to end of the string
 insert()           Inserts characters at an arbitrary index in string
 clear()            Delete all characters in the string
 erase()            Erase characters at an arbitrary index in string
 replace()          Replace characters at an arbitrary index with other characters
 resize()           Expand or shrink the string (truncates or adds characters at end of string)
 swap()             Swaps the value of two strings

 >>, getline()      Reads values from the input stream into the string
 <<                 Writes string value to the output stream
 c_str              Returns the contents of the string as a NULL-terminated C-style string
 copy()             Copies contents (not NULL-terminated) to a character array
 data()             Returns the contents of the string as a non-NULL-terminated character array

 ==, !=             Compares whether two strings are equal/unequal (returns bool)
 compare()          Compares whether two strings are equal/unequal (returns -1, 0, or 1)

 find               Find index of first character/substring
 find_first_of      Find index of first character from a set of characters
 find_first_not_of  Find index of first character not from a set of characters

 begin(), end()     Forward-direction iterator support for beginning/end of string
 get_allocator()    Returns the allocator
 rbegin(), rend()   Reverse-direction iterator support for beginning/end of string

 *
 *
 */

/*
 * === Test 1: string constructor ===
 */
namespace Test1
{
    void str_constructor1(void)
    {
        std::string str("string in constructor");
        std::cout << str << "\n";
    }

    void str_constructor2(void)
    {
        const char *c_str = "string as C stylein constructor";
        std::string str(c_str);
        std::cout << str << "\n";
    }

    void str_constructor3(void)
    {
        std::string str(3, 'R');
        std::cout << str << "\n";
    }

    template <typename T>
    inline std::string ToString(T X)
    {
        std::ostringstream oStream;
        oStream << X;
        return oStream.str();
    }

    void str_constructor4(void)
    {
        std::cout << "Convert X to string:\n";
        std::string str(ToString(3));
        std::cout << str << "\n";

        str = ToString('A');
        std::cout << str << "\n";

        str = ToString(3.5);
        std::cout << str << "\n";
    }

    template <typename T>
    inline bool FromString(std::string str, T &X)
    {
        std::istringstream iStream(str);
        return (iStream >> X) ? true : false;
    }

    void str_constructor5(void)
    {
        std::cout << "Convert string to number:\n";
        double dX = 0.0;
        if (FromString("3.5", dX))
            std::cout << dX << "\n";
        if (FromString("A", dX))
            std::cout << dX << "\n";
    }

    void fn(void)
    {
        std::cout << "<<< string constructor >>>\n";
        str_constructor1();
        str_constructor2();
        str_constructor3();
        str_constructor4();
        str_constructor5();
    }
}

/*
 * === Test 2: string length and capacity ===
 */
namespace Test2
{
    void fn(void)
    {
        std::cout << "<<< string length and capacity >>>\n";

        std::cout << "string: 01234" << "\n";
        std::string str("01234");

        std::cout << "length(): " << str.length() << "\n";
        std::cout << "size(): " << str.size() << "\n";
        std::cout << "empty(): " << (str.empty() ? "true" : "false") << "\n";

        // NOTE:
        // Returns the maximum number of characters that a string is allowed to have.
        // This value will vary depending on operating system and system architecture.
        std::cout << "max_size(): " << str.max_size() << "\n";

        // NOTE:
        // How much memory the string allocated to hold its contents, excluding NULL terminator
        // 1) Reallocating is expensive, and has to copy the contents to new memory.
        //    So allocate a bit more for performance.
        // 2) after string is reallocated, all pointers and references and
        //    iterators to the string is invalid.
        std::cout << "capacity(): " << str.capacity() << "\n";

        std::cout << "\nadd 11 charactors\n";
        str += "0123456789a";
        std::cout << "size(): " << str.size() << "\n";
        std::cout << "capacity(): " << str.capacity() << "\n";

        std::cout << "\nreserve 200\n";
        str.reserve(200);
        std::cout << "size(): " << str.size() << "\n";
        std::cout << "capacity(): " << str.capacity() << "\n";

        std::cout << "\nreserve to fit the string\n";
        // NOTE:
        // shrink the capacity to fit the string size
        str.reserve();
        std::cout << "size(): " << str.size() << "\n";
        std::cout << "capacity(): " << str.capacity() << "\n";
    }
}

int main()
{
    run(1, &(Test1::fn)); // string constructor
    run(2, &(Test2::fn)); // string length and capacity
}
