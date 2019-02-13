#include "global.h"

/*
 * std::vector
 *
 * It provides dynamic array functionality that handles its own memory management.
 *
 */

/*
 * === Test 1: vector initialization ===
 */
namespace Test1
{
    void vec_init(void)
    {
        // No need to specify the size of the vector at initialization.
        std::vector<int> array1;

        // Initialize a vector by initializer list
        std::vector<int> array2 = {1, 2, 3};

        // Initialize a vector by uniform initialization, C++11 onward
        std::vector<int> array3 {4, 5, 6};
        std::cout << "Initialize array3 = {4, 5, 6}" << "\n";

        // C++11: assign value to vector by initializer list
        std::cout << "array3[2] = " << array3[2] << "\n";
        array3 = {1, 2, 3, 4};
        std::cout << "assign valude to array3 = {1, 2, 3, 4}" << "\n";
        std::cout << "array3[2] = " << array3[2] << "\n";

        // array[i]: no bounds check
        // array.at(i): do bounds check
        array3 = {7, 8};
        std::cout << "assign valude to array3 = {7, 8}" << "\n";
        std::cout << "array3[0] = " << array3[0] << "\n";

        // error:
        //   terminate called after throwing an instance of 'std::out_of_range'
        //     what():  vector::_M_range_check: __n (which is 2) >= this->size() (which is 2)
        //std::cout << "array3.at(2) = " << array3.at(2) << "\n";
    }

    void fn(void)
    {
        std::cout << "<<< vector initialization >>>\n";
        vec_init();
    }
}

/*
 * === Test 2: vector size ===
 */
namespace Test2
{
    void vec_size(void)
    {
        std::vector<int> array = {1, 2, 3, 4};
        std::cout << "assign valude to array = {1, 2, 3, 4}" << "\n";
        std::cout << "the length of array is array.size() = " << array.size() << "\n";

        // asign a new initializer list as a different size()
        array = {7, 8};
        std::cout << "assign valude to array = {7, 8}" << "\n";
        std::cout << "the length of array is array.size() = " << array.size() << "\n";

        // resize an array
        array.resize(5);
        std::cout << "the length of array is resized to " << array.size() << "\n";

        std::cout << "array: ";
        for (auto const &element: array)
            std::cout << element << ' ';
        std::cout << "\n";
    }

    void fn(void)
    {
        std::cout << "<<< vector size(length) >>>\n";
        vec_size();
    }
}

/*
 * === Test 3: vector traversing ===
 */
namespace Test3
{
    void fn(void)
    {
        std::cout << "<<< vector traversing >>>\n";

        std::vector<int> array = {1, 2, 3, 4};

        // access by reference to avoid copying C++11
        std::cout << "array(reference):\n";
        for (auto const &element: array)
            std::cout << element << ' ';
        std::cout << "\n";

        // iterator
        std::cout << "array(iterator):\n";
        for (std::vector<int>::iterator it = array.begin(); it != array.end(); ++it)
            std::cout << *it << ' ';
        std::cout << "\n";

        // iterator by auto type
        std::cout << "array(iterator auto):\n";
        for (auto it = array.begin(); it != array.end(); ++it)
            std::cout << *it << ' ';
        std::cout << "\n";

        // C style
        std::cout << "array(C style):\n";
        for (int i = 0; i < array.size(); i++)
            std::cout << array.at(i) << ' ';
        std::cout << "\n";

        // C++17 ??
        //std::cout << "array(iterator auto):\n";
        //std::for_each(array.begin(), array.end(),
        //        [](int *element)
        //        std::cout << *element << ' ';
        //std::cout << "\n";
    }
}

/*
 * === Test 4: vector insert ===
 */
namespace Test4
{
    void vec_print(std::vector<int> &array)
    {
        std::cout << "array:";
        for (auto const &element: array)
            std::cout << element << ' ';
        std::cout << "\n";
    }

    void fn(void)
    {
        std::cout << "<<< vector insert >>>\n";
        std::vector<int> array;

        std::cout << "push_back: 0 1 2 3 4\n";
        for (int i = 0; i < 5; i++)
            array.push_back(i);

        vec_print(array);

        std::cout << "insert 10 at begin() + 2\n";
        array.insert(array.begin()+2, 10);

        vec_print(array);
    }
}

/*
 * === Test 5: vector access ===
 */
namespace Test5
{
    void fn(void)
    {
        std::cout << "<<< vector access >>>\n";

        std::vector<int> array = {1, 2, 3, 4};
        int *p = array.data();

        Test4::vec_print(array);

        std::cout << "*p++ is "<< *p++ << "\n";
        std::cout << "*p is "<< *p << "\n";
        std::cout << "*p[1] is "<< p[1] << "\n";
    }
}

int main()
{
    run(1, &(Test1::fn)); // vector initialization
    run(2, &(Test2::fn)); // vector size
    run(3, &(Test3::fn)); // vector traversing
    run(4, &(Test4::fn)); // vector insert
    run(5, &(Test5::fn)); // vector access
}
