
#include "unit_test.hpp"
loki::test::tests_list loki::test::tests;

#include "type_list_tests.hpp"
#include "type_manip_tests.hpp"
#include "type_traits_tests.hpp"
#include "singleton_tests.hpp"
#include "small_object_tests.hpp"

int main(int argc, char** argv) {
    const auto result = loki::test::run("neoloki unit tests");
    return result;
}
