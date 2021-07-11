#ifndef NEOLOKI_SMALLOBJECTTESTS_HPP
#define NEOLOKI_SMALLOBJECTTESTS_HPP

#include <loki/small_object.hpp>

#include "unit_test.hpp"

namespace loki {

struct small_object_test : test {
    small_object_test() : test("small_object.hpp") {}

    virtual void execute(test_result& results) override {
        {
            auto* ptr = new small_class();
            delete ptr;

            test::assertion("small class allocation", (ptr != nullptr), results);
        }
        {
            auto* ptr = new small_class2();
            delete ptr;

            test::assertion("small sub class allocation", (ptr != nullptr), results);
        }
        {
            auto* ptr = new big_class();
            delete ptr;

            test::assertion("big class allocation", (ptr != nullptr), results);
        }
        {
            auto* ptr = new big_class2();
            delete ptr;

            test::assertion("big sub class allocation", (ptr != nullptr), results);
        }

        //  char* buff = static_cast<char*>(Loki::SmallObject<>::operator new(10));
        //  Loki::SmallObject<>::operator delete(buff, 10);

        //  bool test = buff != NULL;

        //  //    stress_test();

        //  r = smallTest1 && smallTest2 && bigTest1 && bigTest2 && test;

        //  testAssert("SmallObject", r, result);
    }

private:
    struct small_class : loki::small_object<> {
        int a;
    };
    struct small_class2 : small_class {
        int b;
    };

    struct big_class : loki::small_object<> {
        char a[200];
    };
    struct big_class2 : big_class {
        int b;
    };
} small_object_test_instance;

}  // namespace loki

#endif  // NEOLOKI_SMALLOBJECTTESTS_HPP
