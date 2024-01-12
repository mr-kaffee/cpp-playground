#include <iostream>
#include <type_traits>

class Foo {
   public:
    explicit Foo(int value) : _value{value} {
        std::cout << "CTOR " << _value << "\n";
    }

    Foo(Foo const& other) : _value{other._value} {
        std::cout << "COPY CTOR " << _value << "\n";
    }

    Foo(Foo&& other) noexcept : _value{other._value} {
        std::cout << "MOVE CTOR " << _value << "\n";
        other._value = -1;
    };

    Foo& operator=(Foo const& other) {
        if (this == &other) {
            return *this;
        }
        std::cout << "COPY = " << _value << " <= " << other._value << "\n";
        _value = other._value;
        return *this;
    };

    Foo& operator=(Foo&& other) noexcept {
        std::cout << "MOVE = " << _value << " <= " << other._value << "\n";
        _value = other._value;
        other._value = -1;
        return *this;
    }

    ~Foo() {
        std::cout << "DTOR " << _value << "\n";
    }

    [[nodiscard]] int value() const {
        return _value;
    }

    void increment() {
        ++_value;
    }

   private:
    int _value;
};

// NOLINTBEGIN(cppcoreguidelines-missing-std-forward,bugprone-move-forwarding-reference,google-readability-casting)
template <typename T>
T bar1(T&& foo) {
    // ok
    return T(std::move(foo));
}

template <typename T>
std::remove_reference_t<T> bar2(T&& foo) {
    // ok
    return T(std::move(foo));
}

template <typename T>
T bar3(T&& foo) {
    using Type = std::remove_reference_t<T>;
    return Type(std::move(foo));
}

template <typename T>
std::remove_reference_t<T> bar4(T&& foo) {
    // ok
    using Type = std::remove_reference_t<T>;
    return Type(std::move(foo));
}

template <typename T>
T bar5(T&& foo) {
    return T{std::move(foo)};
}

template <typename T>
std::remove_reference_t<T> bar6(T&& foo) {
    return T{std::move(foo)};
}

template <typename T>
T bar7(T&& foo) {
    using Type = std::remove_reference_t<T>;
    return Type{std::move(foo)};
}

template <typename T>
std::remove_reference_t<T> bar8(T&& foo) {
    // ok
    using Type = std::remove_reference_t<T>;
    return Type{std::move(foo)};
}

// NOLINTEND(cppcoreguidelines-missing-std-forward,bugprone-move-forwarding-reference,google-readability-casting)

auto constexpr var1 = 1 << 0;
auto constexpr var2 = 1 << 1;
auto constexpr var3 = 1 << 2;
auto constexpr var4 = 1 << 3;
auto constexpr var5 = 1 << 4;
auto constexpr var6 = 1 << 5;
auto constexpr var7 = 1 << 6;
auto constexpr var8 = 1 << 7;

auto constexpr shLval = 0;
auto constexpr shRval = 8;

auto constexpr var1Id = 1;
auto constexpr var2Id = 2;
auto constexpr var3Id = 3;
auto constexpr var4Id = 4;
auto constexpr var5Id = 5;
auto constexpr var6Id = 6;
auto constexpr var7Id = 7;
auto constexpr var8Id = 8;

auto constexpr facLVal = 1;
auto constexpr facRVal = 10;

// NOLINTBEGIN(misc-const-correctness)
int main() {
    auto constexpr variants =
        (var1 | var2 | var4 | var8) << shLval | (var1 | var2 | var3 | var4 | var5 | var6 | var7 | var8) << shRval;

    if constexpr ((variants & (var1 << shLval)) > 0) {
        Foo foo1{var1Id * facLVal};
        Foo foo2 = bar1(foo1);
    }

    if constexpr ((variants & (var1 << shRval)) > 0) {
        Foo foo = bar1(Foo{var1Id * facRVal});
    }

    if constexpr ((variants & (var2 << shLval)) > 0) {
        Foo foo1{var2Id * facLVal};
        Foo foo2 = bar2(foo1);
    }

    if constexpr ((variants & (var2 << shRval)) > 0) {
        Foo foo = bar2(Foo{var2Id * facRVal});
    }

    if constexpr ((variants & (var3 << shLval)) > 0) {
        Foo foo1{var3Id * facLVal};
        Foo foo2 = bar3(foo1);
    }

    if constexpr ((variants & (var3 << shRval)) > 0) {
        Foo foo = bar3(Foo{var3Id * facRVal});
    }

    if constexpr ((variants & (var4 << shLval)) > 0) {
        Foo foo1{var4Id * facLVal};
        Foo foo2 = bar4(foo1);
    }

    if constexpr ((variants & (var4 << shRval)) > 0) {
        Foo foo = bar4(Foo{var4Id * facRVal});
    }

    if constexpr ((variants & (var5 << shLval)) > 0) {
        Foo foo1{var5Id * facLVal};
        Foo foo2 = bar5(foo1);
    }

    if constexpr ((variants & (var5 << shRval)) > 0) {
        Foo foo = bar5(Foo{var5Id * facRVal});
    }

    if constexpr ((variants & (var6 << shLval)) > 0) {
        Foo foo1{var6Id * facLVal};
        Foo foo2 = bar6(foo1);
    }

    if constexpr ((variants & (var6 << shRval)) > 0) {
        Foo foo = bar6(Foo{var6Id * facRVal});
    }

    if constexpr ((variants & (var7 << shLval)) > 0) {
        Foo foo1{var7Id * facLVal};
        Foo foo2 = bar7(foo1);
    }

    if constexpr ((variants & (var7 << shRval)) > 0) {
        Foo foo = bar7(Foo{var7Id * facRVal});
    }

    if constexpr ((variants & (var8 << shLval)) > 0) {
        Foo foo1{var8Id * facLVal};
        Foo foo2 = bar8(foo1);
    }

    if constexpr ((variants & (var8 << shRval)) > 0) {
        Foo foo = bar8(Foo{var8Id * facRVal});
    }

    return 0;
}

// NOLINTEND(misc-const-correctness)
