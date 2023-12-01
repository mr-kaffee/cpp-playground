#include <iostream>
#include <type_traits>
#include <utility>

#include "Foo.hpp"

template <typename T>
T foo(T&& arg);

template <>
Foo& foo(Foo& arg) {
    std::cout << "foo(LVALUE_REF " << arg << ")\n";
    return arg;
}

template <>
Foo const& foo(Foo const& arg) {
    std::cout << "foo(LVALUE_CONST_REF " << arg << ")\n";
    return arg;
}

// NOLINTBEGIN(cppcoreguidelines-rvalue-reference-param-not-moved): used for illustration purposes
template <>
Foo foo(Foo&& arg) {
    std::cout << "foo(RVALUE_REF " << arg << ")\n";
    return arg;
}  // NOLINTEND(cppcoreguidelines-rvalue-reference-param-not-moved)

// NOLINTBEGIN(readability-const-return-type): required to match template
template <>
Foo const foo(Foo const&& arg) {
    std::cout << "foo(RVALUE_CONST_REF " << arg << ")\n";
    return arg;
}  // NOLINTEND(readability-const-return-type)

template <typename S>
S bar(S&& arg) {
    std::cout << "bar(" << arg << ")\n";
    return foo(std::forward<S>(arg));
}

/// bad idea of clone implementation since it will move out of values passed as lvalue references
template <typename T>
[[gnu::noinline]] std::remove_reference_t<T> cloneMoved(T&& arg);

template <>
Foo cloneMoved(Foo& arg) {
    std::cout << "clonedMove(LVALUE REF " << arg << ")\n";
    // moving reference might be a bad idea, the caller might not be able to use
    // the object passed by reference
    return Foo{std::move(arg)};
}

template <>
Foo cloneMoved(Foo&& arg) {
    std::cout << "clonedMove(RVALUE REF " << arg << ")\n";
    return Foo{std::move(arg)};
}

// NOLINTBEGIN(readability-const-return-type,performance-move-const-arg): const return type required to match template,
// std::move is there for illustration purposes
template <>
Foo const cloneMoved(Foo const& arg) {
    std::cout << "clonedMove(LVALUE CREF " << arg << ")\n";
    // std::move of const argument has no effect!
    return Foo{std::move(arg)};
}

template <>
Foo const cloneMoved(Foo const&& arg) {
    std::cout << "clonedMove(RVALUE CREF " << arg << ")\n";
    // std::move of const argument has no effect!
    return Foo{std::move(arg)};
}  // NOLINTEND(readability-const-return-type,performance-move-const-arg)

/// good idea for clone implementation, will copy if it gets an lvalue and move out of an rvalue
template <typename T>
std::remove_reference_t<T> cloneFwd(T&& arg);

template <>
Foo cloneFwd(Foo& arg) {
    std::cout << "cloneFwd(LVALUE REF " << arg << ")\n";
    return Foo{std::forward<Foo&>(arg)};
}

// NOLINTBEGIN(cppcoreguidelines-rvalue-reference-param-not-moved): it is forwarded instead
template <>
Foo cloneFwd(Foo&& arg) {
    std::cout << "cloneFwd(RVALUE REF " << arg << ")\n";
    return Foo{std::forward<Foo>(arg)};
}  // NOLINTEND(cppcoreguidelines-rvalue-reference-param-not-moved)

// NOLINTBEGIN(readability-const-return-type): required to match template
template <>
Foo const cloneFwd(Foo const& arg) {
    std::cout << "cloneFwd(LVALUE CREF " << arg << ")\n";
    return Foo{std::forward<Foo const&>(arg)};
}

template <>
Foo const cloneFwd(Foo const&& arg) {
    std::cout << "cloneFwd(RVALUE CREF " << arg << ")\n";
    return Foo{std::forward<Foo const>(arg)};
}  // NOLINTEND(readability-const-return-type)

/// an alternative which does not make a whole lot sense
template <typename T>
T cloneMovedAlt(T&& arg);

// ** will lead to compiler errors
// template<>
// Foo& cloneMovedAlt(Foo& arg) {
//     std::cout << "cloneMovedAlt(LVALUE REF " << arg << ")\n";
//     return Foo{std::move(arg)};
// }

/// only relevant/valid instantiation of template, equivalent to cloneMoved
template <>
Foo cloneMovedAlt(Foo&& arg) {
    std::cout << "cloneMovedAlt(RVALUE REF " << arg << ")\n";
    return Foo{std::move(arg)};
}

// ** will lead to segmentation faults
// template <>
// Foo const& cloneMovedAlt(Foo const& arg) {
//     std::cout << "cloneMovedAlt(LVALUE CREF " << arg << ")\n";
//     return Foo{std::move(arg)};
// }

// ** not used
// NOLINTBEGIN(readability-const-return-type,performance-move-const-arg)
// template <>
// Foo const cloneMovedAlt(Foo const&& arg) {
//     std::cout << "cloneMovedAlt(RVALUE CREF " << arg << ")\n";
//     return Foo{std::move(arg)};
// }  // NOLINTEND(readability-const-return-type,performance-move-const-arg)

static constexpr int bufferSize{10};

// NOLINTBEGIN(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,performance-move-const-arg,performance-unnecessary-copy-initialization):
// don't care about const correctness here, numbers identify output here, move of const arg is there for illustration,
// std::move for illustration, copies for illustration
int main() {
    {
        std::cout << "------ 1.1 --- foo(PLAIN)\n";
        Foo foo_0 = Foo{bufferSize, 11};
        Foo foo_1 = foo(foo_0);
    }

    {
        std::cout << "------ 1.2 --- foo(bar(PLAIN))\n";
        Foo foo_0 = Foo{bufferSize, 12};
        Foo foo_1 = bar(foo_0);
    }

    {
        std::cout << "------ 2.1 --- foo(REF)\n";
        Foo foo_0 = Foo{bufferSize, 21};
        Foo& foo_r = foo_0;
        Foo foo_1 = foo(foo_r);
    }

    {
        std::cout << "------ 2.2 --- foo(bar(REF))\n";
        Foo foo_0 = Foo{bufferSize, 22};
        Foo& foo_r = foo_0;
        Foo foo_1 = bar(foo_r);
    }

    {
        std::cout << "------ 3.1 --- foo(CREF)\n";
        Foo foo_0 = Foo{bufferSize, 31};
        Foo const& foo_r = foo_0;
        Foo foo_1 = foo(foo_r);
    }

    {
        std::cout << "------ 3.2 --- foo(bar(CREF))\n";
        Foo foo_0 = Foo{bufferSize, 32};
        Foo const& foo_r = foo_0;
        Foo foo_1 = bar(foo_r);
    }

    {
        std::cout << "------ 4.1 --- foo(TEMPORARY)\n";
        Foo foo_1 = foo(Foo{bufferSize, 41});
    }

    {
        std::cout << "------ 4.2 --- foo(bar(TEMPORARY))\n";
        Foo foo_1 = bar(Foo{bufferSize, 42});
    }

    {
        std::cout << "---- 101.1 --- foo(move(PLAIN))\n";
        Foo foo_0 = Foo{bufferSize, 1011};
        Foo foo_1 = foo(std::move(foo_0));
    }

    {
        std::cout << "---- 101.2 --- foo(bar(move(PLAIN)))\n";
        Foo foo_0 = Foo{bufferSize, 1012};
        Foo foo_1 = bar(std::move(foo_0));
    }

    {
        std::cout << "---- 102.1 --- foo(move(REF))\n";
        Foo foo_0 = Foo{bufferSize, 1021};
        Foo& foo_r = foo_0;
        Foo foo_1 = foo(std::move(foo_r));
    }

    {
        std::cout << "---- 102.2 --- foo(bar(move(REF)))\n";
        Foo foo_0 = Foo{bufferSize, 1022};
        Foo& foo_r = foo_0;
        Foo foo_1 = bar(std::move(foo_r));
    }

    {
        std::cout << "---- 103.1 --- foo(move(CREF))\n";
        Foo foo_0 = Foo{bufferSize, 1031};
        Foo const& foo_r = foo_0;
        Foo foo_1 = foo(std::move(foo_r));
    }

    {
        std::cout << "---- 103.2 --- foo(bar(move(CREF)))\n";
        Foo foo_0 = Foo{bufferSize, 1032};
        Foo const& foo_r = foo_0;
        Foo foo_1 = bar(std::move(foo_r));
    }

    {
        std::cout << "--- 9901.0 --- = PLAIN / = move(PLAIN)\n";
        Foo foo_0 = Foo{bufferSize, 99010};
        Foo foo_1 = foo_0;
        Foo foo_2 = std::move(foo_1);
    }

    {
        std::cout << "--- 9902.0 --- = REF / = move(REF)\n";
        Foo foo_0 = Foo{bufferSize, 99020};
        Foo& foo_r = foo_0;
        Foo foo_1 = foo_r;
        Foo foo_2 = std::move(foo_r);
    }

    {
        std::cout << "--- 9903.0 --- = CREF / = move(CREF)\n";
        Foo foo_0 = Foo{bufferSize, 99030};
        Foo const& foo_r = foo_0;
        Foo foo_1 = foo_r;
        Foo foo_2 = std::move(foo_r);
    }

    {
        std::cout << "-- 10001.1 --- cloneMoved(PLAIN)\n";
        Foo foo_0 = Foo{bufferSize, 100011};
        Foo foo_1 = cloneMoved(foo_0);
    }

    {
        std::cout << "-- 10001.2 --- cloneFwd(PLAIN)\n";
        Foo foo_0 = Foo{bufferSize, 100012};
        Foo foo_1 = cloneFwd(foo_0);
    }

    // will lead to compiler errors
    // {
    //     std::cout << "-- 10001.3 --- cloneMovedAlt(PLAIN)\n";
    //     Foo foo_0 = Foo{bufferSize, 100013};
    //     Foo foo_1 = cloneMovedAlt(foo_0);
    // }

    {
        std::cout << "-- 10002.1 --- cloneMoved(REF)\n";
        Foo foo_0 = Foo{bufferSize, 100021};
        Foo& foo_r = foo_0;
        Foo foo_1 = cloneMoved(foo_r);
    }

    {
        std::cout << "-- 10002.2 --- cloneFwd(REF)\n";
        Foo foo_0 = Foo{bufferSize, 100022};
        Foo& foo_r = foo_0;
        Foo foo_1 = cloneFwd(foo_r);
    }

    // will lead to compiler errors
    // {
    //     std::cout << "-- 10002.3 --- cloneMovedAlt(REF)\n";
    //     Foo foo_0 = Foo{bufferSize, 100023};
    //     Foo& foo_r = foo_0;
    //     Foo foo_1 = cloneMovedAlt(foo_r);
    // }

    {
        std::cout << "-- 10003.1 --- cloneMoved(CREF)\n";
        Foo foo_0 = Foo{bufferSize, 100031};
        Foo const& foo_r = foo_0;
        Foo foo_1 = cloneMoved(foo_r);
    }

    {
        std::cout << "-- 10003.2 --- cloneFwd(CREF)\n";
        Foo foo_0 = Foo{bufferSize, 100032};
        Foo const& foo_r = foo_0;
        Foo foo_1 = cloneFwd(foo_r);
    }

    // will lead to segmentation fault
    // {
    //     std::cout << "-- 10003.3 --- cloneMovedAlt(CREF)\n";
    //     Foo foo_0 = Foo{bufferSize, 100033};
    //     Foo const& foo_r = foo_0;
    //     Foo foo_1 = cloneMovedAlt(foo_r);
    // }

    {
        std::cout << "-- 10004.1 --- cloneMoved(TEMPORARY)\n";
        Foo foo_1 = cloneMoved(Foo{bufferSize, 100041});
    }

    {
        std::cout << "-- 10004.2 --- cloneFwd(TEMPORARY)\n";
        Foo foo_1 = cloneFwd(Foo{bufferSize, 100042});
    }

    {
        std::cout << "-- 10004.3 --- cloneMovedAlt(TEMPORARY)\n";
        Foo foo_1 = cloneMovedAlt(Foo{bufferSize, 100043});
    }

    return 0;
}  // NOLINTEND(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,performance-move-const-arg,performance-unnecessary-copy-initialization)
