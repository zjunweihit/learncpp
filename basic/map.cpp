#include "global.h"
#include <map>

/*
 * Map/Multimap
 * 
 * Constructs a group of <key, value> elements.
 * Map doesn't contain duplicat key, but Multimap does.
 *
 * include <map>
 * 
 * Map: <1, a> <2, b> <5, e> <6,f>
 * Multimap: <1, a> <1, b> <3, e> <4, f>
 *
 * constructor
 *   std::map<int, std::string> m
 */

/*
 * === Test 1: map ===
 */
namespace Test1
{
    // Could it be done by template??
    void print_map(std::map<int, std::string> &m)
    {
        std::map<int, std::string>::iterator p;
        std::cout << "list map:\n";
        //for (p = m.begin(); p != m.end(); ++p) {
        //    std::cout << "  <" << p->first << ", "
        //              << p->second << ">" << std::endl;
        //}
        // same as above [begin, end)
        for (auto& p : m) {
            std::cout << "  <" << p.first << ", "
                      << p.second << ">" << std::endl;
        }

    }

    void map_test(void)
    {
        std::cout << "create map <int, std::string>" << std::endl;

        // define map
        std::map<int, std::string> m;

        // insert an element into map
        // 1. specify an exact value as map<key, value>
        m.insert(std::map<int, std::string>::value_type(1, "a"));
        // 2. use pair
        m.insert(std::pair<int, std::string>(2, "b"));
        m.insert(std::pair<int, const std::string>(3, "c"));
        // 3. make_pair
        m.insert(std::make_pair(4, "d"));
        // 4. insert by key like array, if it's not existing, insert it
        m[5] = "e";
        print_map(m); // by reference

        // remove an element from map
        std::cout << "remove key: 4 directly\n";
        m.erase(4);
        print_map(m); // by reference

        std::cout << "remove an interator by find, key: 2\n";
        std::map<int, std::string>::iterator p = m.find(2);
        if (p != m.end()) // it's found
            m.erase(p);
        print_map(m); // by reference
    }

    void fn(void)
    {
        std::cout << "<<< map create, insert, earse, iterator >>>\n";
        map_test();
    }
}

/*
 * === Test 2: multimap ===
 */
namespace Test2
{
    // Could it be done by template??
    void print_multimap(std::multimap<int, std::string> &m)
    {
        std::multimap<int, std::string>::iterator p;
        std::cout << "list map:\n";
        //for (p = m.begin(); p != m.end(); ++p) {
        //    std::cout << "  <" << p->first << ", "
        //              << p->second << ">" << std::endl;
        //}
        // same as above [begin, end)
        for (auto& p : m) {
            std::cout << "  <" << p.first << ", "
                      << p.second << ">" << std::endl;
        }

    }

    void multimap_test(void)
    {
        std::cout << "create map <int, std::string>" << std::endl;

        // define map
        std::multimap<int, std::string> m = {{5, "e"}};

        // insert an element into map
        // 1. specify an exact value as map<key, value>
        m.insert(std::multimap<int, std::string>::value_type(1, "a"));
        // 2. use pair
        m.insert(std::pair<int, std::string>(1, "b"));
        m.insert(std::pair<int, const std::string>(2, "c"));
        // 3. make_pair
        m.insert(std::make_pair(1, "d"));
        m.insert(std::make_pair(1, "e"));
        m.insert(std::make_pair(1, "f"));
        // 4. [NOT supported]
        //m[5] = "e";
        print_multimap(m); // by reference

        // remove an element from map
        std::cout << "remove key: 2 directly\n";
        m.erase(2);
        print_multimap(m); // by reference

        std::cout << "remove an interator by find, remove the 1st key: 1\n";
        std::map<int, std::string>::iterator p = m.find(1);
        if (p != m.end()) // it's found
            m.erase(p);
        print_multimap(m); // by reference
    }

    void fn(void)
    {
        std::cout << "<<< multimap create, insert, earse, iterator >>>\n";
        multimap_test();
    }
}
int main()
{
    run(1, &(Test1::fn)); // map
    run(2, &(Test2::fn)); // multimap
}
