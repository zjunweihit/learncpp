#include "global.h"
#include <sstream>
#include <string.h>

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
                    at() is slower and checks if index is out of range,
                    throws out_of_range exception when passing in a invalid index

 =, assign()        Assigns a new value to the string
                    assign() checks index, if it's out of range, throws out_of_range
                    exception.

 +=, append(), push_back()   Concatenates characters to end of the string
                    push_back() appends char, not string
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

/*
 * === Test 3: string access and converting to C-style array ===
 */
namespace Test3
{
    void fn(void)
    {
        std::cout << "<<< string access and converting to C-style array >>>\n";

        std::string str("0123456789");
        std::cout << "string: 0123456789" << "\n";
        str[5] = 'A';
        str.at(6) = 'B';
        std::cout << "index 5->A, 6->B: " << str << "\n";

        std::cout << "strlen as C-style array: " << strlen(str.c_str()) << "\n";

        std::cout << "using data() for comparison:\n";
        const char *str1 = "01234AB789";
        if (memcmp(str.data(), str1, str.length()) == 0)
            std::cout << "The string is 01234AB789 now\n";
        else
            std::cout << "The string is not 01234AB789 now\n";

        std::cout << "copy(): AB to a new char array\n";
        char str2[10];
        int length = str.copy(str2, 2, 5);
        str2[length] = '\0'; // NOTE: make NULL terminator for the char array
        std::cout << str2 << "\n";
    }
}

/*
 * === Test 4: string access and converting to C-style array ===
 */
namespace Test4
{
    void fn(void)
    {
        std::cout << "<<< string assign and swap >>>\n";
        std::string str1;
        str1.assign("111");
        std::cout << "str1: " << str1 << '\n';

        std::string str2;
        str2.assign("222");
        std::cout << "str2: " << str2 << '\n';

        std::cout << "\ndo swap():\n";
        swap(str1, str2);
        std::cout << "str1: " << str1 << '\n';
        std::cout << "str2: " << str2 << '\n';

        std::cout << "\ndo str.swap():\n";
        str1.swap(str2);
        std::cout << "str1: " << str1 << '\n';
        std::cout << "str2: " << str2 << '\n';
    }
}

/*
 * === Test 5: string appending ===
 */
namespace Test5
{
    void fn(void)
    {
        std::cout << "<<< string appending >>>\n";

        std::string str("one");
        std::cout << str << '\n';

        str += " two";
        std::cout << "appended by +=:\n" << str << '\n';

        str.append(" three");
        std::cout << "appended by str.append():\n" << str << '\n';

        str += ' ';
        str.push_back('4');
        std::cout << "appended by str.push_back():\n" << str << '\n';
    }
}

int main()
{
    run(1, &(Test1::fn)); // string constructor
    run(2, &(Test2::fn)); // string length and capacity
    run(3, &(Test3::fn)); // string access and converting to C-style array
    run(4, &(Test4::fn)); // string assign and swap
    run(5, &(Test5::fn)); // string appending
}
