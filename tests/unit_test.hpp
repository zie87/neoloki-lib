#ifndef LOKINEO_UNITTESTS_HPP
#define LOKINEO_UNITTESTS_HPP

#include <iostream>
#include <string>
#include <vector>

namespace loki {

struct test_result {
    int positon;
    int passed;
    int failed;
    int not_supported;
};

class test {
    using tests_list = std::vector<test*>;

public:
    virtual void execute(test_result&) = 0;

    static int run(const std::string& title) {
        test_result test_results{};

        for (auto& test_case : test::tests) {
            test_case->print_name(test_results);
            test_case->execute(test_results);
        }

        std::cout << std::string(76, '-') << '\n';

        const int total     = test_results.passed + test_results.failed;
        const int total_all = total + test_results.not_supported;

        if (total != 0) {
            std::cout << "total - " << test_results.passed << '/' << total << (total == 1 ? " test, " : " tests, ")
                      << test_results.passed * 100 / total << "% passed\n";
        }

        if (test_results.not_supported != 0) {
            std::cout << "not supported - " << test_results.not_supported << '/' << total_all << ", "
                      << test_results.not_supported * 100 / total_all << "%\n";
        }

        return test_results.failed;
    }

protected:
    explicit test(const std::string& test_name) : name(test_name) { test::tests.push_back(this); }
    virtual ~test() noexcept = default;

    static tests_list tests;
    const std::string name;

    void print_name(const test_result& results) const {
        if (!name.empty()) {
            std::cout << std::string(results.positon, ' ') << name << '\n';
        }
    }

    void assertion(const std::string& s, bool asserts, test_result& results, bool supported = true,
                   const std::string& failstr = "") {
        constexpr std::size_t offset = 63U;

        auto str = std::string(results.positon + 2, ' ') + s;
        str += std::string(offset - str.length(), ' ');

        if (!supported) {
            std::cout << str << "not supported\n";
            ++results.not_supported;
            return;
        }

        if (!asserts) {
            std::cout << str << "failed" << ((!failstr.empty()) ? " - " + failstr : "") << '\n';
            ++results.failed;
            return;
        }

        std::cout << str << "passed\n";
        ++results.passed;
    }
};

}  // namespace loki

#endif  // LOKINEO_UNITTESTS_HPP
