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

        {
            auto* buffer = static_cast<char*>(loki::small_object<>::operator new(10));
            loki::small_object<>::operator delete(buffer, 10);

            test::assertion("buffer allocation", (buffer != nullptr), results);
        }

        stress_test();
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

    struct base {
        virtual ~base() {}
    };

    class a : public base, public loki::small_object<> {
        int a[1];
    };

    class b : public base, public loki::small_object<> {
        int a[2];
    };

    class c : public base, public loki::small_object<> {
        int a[3];
    };

    class d : public base, public loki::small_object<> {
        int a[4];
    };

    static void stress_test() {
        constexpr std::size_t repetions = 25;
        constexpr std::size_t elements  = 1024 * 2;

        std::vector<base*> vec;
        vec.reserve(repetions * elements);

        std::srand(1231);
        for (std::size_t i = 0; i < repetions; ++i) {
            for (std::size_t j = 0; j < elements; ++j) {
                base* p = nullptr;

                switch (std::rand() % 4) {
                case 0:
                    p = new a;
                    break;
                case 1:
                    p = new b;
                    break;
                case 2:
                    p = new c;
                    break;
                case 3:
                    p = new d;
                    break;
                }

                vec.push_back(p);
            }

            for (std::size_t j = 0; j < (elements / 2); ++j) {
                size_t pos = std::rand() % vec.size();
                delete vec[pos];
                vec[pos] = nullptr;
            }
        }

        while (!vec.empty()) {
            delete vec.back();
            vec.pop_back();
        }
    }

} small_object_test_instance;

}  // namespace loki

#endif  // NEOLOKI_SMALLOBJECTTESTS_HPP
