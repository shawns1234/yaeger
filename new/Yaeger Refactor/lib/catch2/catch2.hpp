#ifndef CATCH2_HPP
#define CATCH2_HPP

#define CATCH2_VERSION_MAJOR 3
#define CATCH2_VERSION_MINOR 2

// Minimal Catch2-compatible header for the test suite.
// In production, this would be replaced by the full Catch2 library.

#include <cstdio>
#include <iostream>
#include <string>
#include <stdexcept>

struct Catch2TestCase {
    std::string name;
    bool passed;
    std::string error;
    Catch2TestCase(const std::string& n) : name(n), passed(true), error("") {}
};

namespace Catch2 {

struct AssertionResult {
    bool passed() const { return passed_; }
    std::string getResultAsString() const { return result_; }
    static AssertionResult makeFailure(const std::string& msg) {
        AssertionResult r; r.passed_ = false; r.result_ = msg; return r;
    }
    static AssertionResult makeSuccess() {
        AssertionResult r; r.passed_ = true; r.result_ = "true"; return r;
    }
private:
    AssertionResult() : passed_(true), result_("") {}
    bool passed_;
    std::string result_;
};

template<typename T>
AssertionResult makeAssertion(T&& value, const char* expr, const char* exprStr) {
    if (value) {
        return AssertionResult::makeSuccess();
    } else {
        return AssertionResult::makeFailure(std::string(exprStr) + " evaluates to false");
    }
}

template<typename T>
AssertionResult makeNotAssertion(T&& value, const char* expr, const char* exprStr) {
    if (!value) {
        return AssertionResult::makeSuccess();
    } else {
        return AssertionResult::makeFailure(std::string(exprStr) + " evaluates to true");
    }
}

struct TestCaseInfo {
    std::string name;
    std::string description;
    bool passed;
    int line;
    TestCaseInfo(const std::string& n, const std::string& d, bool p, int l)
        : name(std::move(n)), description(std::move(d)), passed(p), line(l) {}
};

} // namespace Catch2

#define CATCH2_INTERNAL_REQUIRE(expr) \
    do { \
        auto _ar = Catch2::makeAssertion(expr, #expr, __FILE__ ":" std::to_string(__LINE__)); \
        if (!_ar.passed()) { \
            std::cerr << "FAILED: " << #expr << "\n"; \
            std::cerr << "  at " << __FILE__ ":" << __LINE__ << "\n"; \
            std::cerr << "  " << _ar.getResultAsString() << "\n"; \
            throw std::runtime_error("Test failed"); \
        } \
    } while (false)

#define CATCH2_INTERNAL_CHECK(expr) \
    do { \
        auto _ar = Catch2::makeAssertion(expr, #expr, __FILE__ ":" std::to_string(__LINE__)); \
        if (!_ar.passed()) { \
            std::cerr << "CHECK FAILED: " << #expr << "\n"; \
            std::cerr << "  at " << __FILE__ ":" << __LINE__ << "\n"; \
            std::cerr << "  " << _ar.getResultAsString() << "\n"; \
        } \
    } while (false)

#define REQUIRE(expr) CATCH2_INTERNAL_REQUIRE(expr)
#define CHECK(expr)   CATCH2_INTERNAL_CHECK(expr)

#define CATCH2_INTERNAL_TEST_CASE(name, desc, line) \
    extern Catch2::TestCaseInfo testCase_##name{#name, desc, true, line};

#define TEST_CASE(name, ...) \
    CATCH2_INTERNAL_TEST_CASE(name, "", __LINE__)

#define CATCH2_INTERNAL_STATIC_TEST_CASE(name, desc, line) \
    extern Catch2::TestCaseInfo testCase_##name{#name, desc, true, line};

#define STATIC_TEST_CASE(name, ...) \
    CATCH2_INTERNAL_STATIC_TEST_CASE(name, "", __LINE__)

#define CATCH2_INTERNAL_TEST_CASE_METHOD(className, name, desc, line) \
    extern Catch2::TestCaseInfo testCase_##className##_##name{#name, desc, true, line};

#define TEST_CASE_METHOD(className, name, ...) \
    CATCH2_INTERNAL_TEST_CASE_METHOD(className, name, "", __LINE__)

#define CATCH2_INTERNAL_CAPTURE_MACROS() \
    decltype(__VA_ARGS__)(__VA_ARGS__),

#define CATCH2_INTERNAL_GETLINE() \
    CATCH2_INTERNAL_CAPTURE_MACROS() \
    int __catch2_line__ = __LINE__; \
    CATCH2_INTERNAL_GETLINE()

#define CATCH2_INTERNAL_TEST_CASE2(className, name, desc, line) \
    extern Catch2::TestCaseInfo testCase_##className##_##name{#name, desc, true, line};

#define TEST_CASE_METHOD(className, name, ...) \
    CATCH2_INTERNAL_TEST_CASE2(className, name, "", __LINE__)

#define SECTION(name) \
    do { \
        std::cerr << "SECTION: " << name << "\n"; \
    } while(false)

#endif // CATCH2_HPP
