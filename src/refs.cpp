#include <algorithm>
#include <iostream>
#include <type_traits>
#include <utility>

/// control whether allocation/de-allocation is displayed
static constexpr auto displayAllocation = false;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace): static globals ok for
// this toy example
/// counter to create unique object identity
static int objectCount{0};
/// store total size of allocated memory
static int totalAlloc{0};

// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace)

// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic): pointer arithmetic used for illustration purposes
/// Sample class which manages some resources and is copy- and move-assignable and copy- and move-constructible
class Foo {
   public:
    /**
     * Construct instance
     * @param size the size of the memory to allocate
     */
    explicit Foo(int size) : _id{objectCount++}, _size{size} {
        alloc();
    }

    ~Foo() {
        std::cout << "DTOR " << *this << "\n";
        dealloc();
    }

    Foo(Foo const& other) : _id{objectCount++} {
        std::cout << "COPY Foo(" << other << ") -> [id: " << _id << "]\n";
        copy(other);
    }

    Foo(Foo&& other) noexcept : _id{objectCount++} {
        std::cout << "MOVE Foo(" << other << ") -> [id: " << _id << "]\n";
        take(other);
    }

    Foo& operator=(Foo const& rhs) {
        if (&rhs != this) {
            std::cout << "COPY = " << rhs << " -> [id: " << _id << "]\n";
            dealloc();
            copy(rhs);
        }

        return *this;
    }

    Foo& operator=(Foo&& rhs) noexcept {
        std::cout << "MOVE = " << rhs << " -> [id: " << _id << "]\n";
        dealloc();
        take(rhs);

        return *this;
    }

    [[nodiscard]] int size() const noexcept {
        return _size;
    }

    [[nodiscard]] int operator[](int idx) const {
        return _buffer[idx];
    }

    [[nodiscard]] int& operator[](int idx) {
        return _buffer[idx];
    }

    friend std::ostream& operator<<(std::ostream& stream, Foo const& value);

   private:
    int _size{0};
    int* _buffer{nullptr};
    int _id;

    /// Allocate memory
    /// Requires _size to be set and target not to have any memory allocated
    void alloc() {
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory): ok in toy example
        _buffer = new int[_size];
        if constexpr (displayAllocation) {
            std::cout << "  allocated [id: " << _id << "], Σ = " << (totalAlloc += _size) << "\n";
        }
    }

    /// De-allocate memory
    /// Will not clean-up!
    void dealloc() noexcept {
        delete[] _buffer;
        if constexpr (displayAllocation) {
            if (_size > 0) { std::cout << "  de-allocated [id: " << _id << "], Σ = " << (totalAlloc -= _size) << "\n"; }
        }
    }

    /// Take ownership of the resources of other
    /// Requires target not to have any memory allocated
    void take(Foo& other) noexcept {
        _size = other._size;
        _buffer = other._buffer;

        other._size = 0;
        other._buffer = nullptr;
    }

    /// Copy resources of other
    /// Requires target not to have any memory allocated
    void copy(Foo const& other) {
        _size = other._size;
        alloc();
        std::copy(other._buffer, other._buffer + _size, _buffer);
    }
};  // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

std::ostream& operator<<(std::ostream& stream, Foo const& value) {
    if (value._size > 0) {
        stream << value[0];
    } else {
        stream << "NULL";
    }
    stream << "[id: " << value._id << "]";
    return stream;
}

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

template <typename T>
std::remove_reference_t<T> cloneMoved(T&& arg);

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
    std::cout << "clonedMove(LVALUE CONST REF " << arg << ")\n";
    // std::move of const argument has no effect!
    return Foo{std::move(arg)};
}

template <>
Foo const cloneMoved(Foo const&& arg) {
    std::cout << "clonedMove(RVALUE CONST REF " << arg << ")\n";
    // std::move of const argument has no effect!
    return Foo{std::move(arg)};
}  // NOLINTEND(readability-const-return-type,performance-move-const-arg)

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
    std::cout << "cloneFwd(LVALUE CONST REF " << arg << ")\n";
    return Foo{std::forward<Foo const&>(arg)};
}

template <>
Foo const cloneFwd(Foo const&& arg) {
    std::cout << "cloneFwd(RVALUE CONST REF " << arg << ")\n";
    return Foo{std::forward<Foo const>(arg)};
}  // NOLINTEND(readability-const-return-type)

static constexpr int bufferSize{10};

Foo build(int val) {
    Foo foo{bufferSize};
    foo[0] = val;
    std::cout << "build: " << foo << "\n";
    // copy elision! Foo constructed into the storage of its final destination
    // See https://en.cppreference.com/w/cpp/language/copy_elision
    return foo;
}

// NOLINTBEGIN(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,performance-move-const-arg,performance-unnecessary-copy-initialization):
// don't care about const correctness here, numbers identify output here, move of const arg is there for illustration,
// std::move for illustration, copies for illustration
int main() {
    {
        std::cout << "------ 1.1 --- foo(PLAIN)\n";
        Foo foo_0 = build(11);
        Foo foo_1 = foo(foo_0);
    }

    {
        std::cout << "------ 1.2 --- foo(bar(PLAIN))\n";
        Foo foo_0 = build(12);
        Foo foo_1 = bar(foo_0);
    }

    {
        std::cout << "------ 2.1 --- foo(REF)\n";
        Foo foo_0 = build(21);
        Foo& foo_r = foo_0;
        Foo foo_1 = foo(foo_r);
    }

    {
        std::cout << "------ 2.2 --- foo(bar(REF))\n";
        Foo foo_0 = build(22);
        Foo& foo_r = foo_0;
        Foo foo_1 = bar(foo_r);
    }

    {
        std::cout << "------ 3.1 --- foo(CONST REF)\n";
        Foo foo_0 = build(31);
        Foo const& foo_r = foo_0;
        Foo foo_1 = foo(foo_r);
    }

    {
        std::cout << "------ 3.2 --- foo(bar(CONST REF))\n";
        Foo foo_0 = build(32);
        Foo const& foo_r = foo_0;
        Foo foo_1 = bar(foo_r);
    }

    {
        std::cout << "------ 4.1 --- foo(TEMPORARY)\n";
        Foo foo_1 = foo(build(41));
    }

    {
        std::cout << "------ 4.2 --- foo(bar(TEMPORARY))\n";
        Foo foo_1 = bar(build(42));
    }

    {
        std::cout << "---- 101.1 --- foo(move(PLAIN))\n";
        Foo foo_0 = build(1011);
        Foo foo_1 = foo(std::move(foo_0));
    }

    {
        std::cout << "---- 101.2 --- foo(bar(move(PLAIN)))\n";
        Foo foo_0 = build(1012);
        Foo foo_1 = bar(std::move(foo_0));
    }

    {
        std::cout << "---- 102.1 --- foo(move(REF))\n";
        Foo foo_0 = build(1021);
        Foo& foo_r = foo_0;
        Foo foo_1 = foo(std::move(foo_r));
    }

    {
        std::cout << "---- 102.2 --- foo(bar(move(REF)))\n";
        Foo foo_0 = build(1022);
        Foo& foo_r = foo_0;
        Foo foo_1 = bar(std::move(foo_r));
    }

    {
        std::cout << "---- 103.1 --- foo(move(CONST REF))\n";
        Foo foo_0 = build(1031);
        Foo const& foo_r = foo_0;
        Foo foo_1 = foo(std::move(foo_r));
    }

    {
        std::cout << "---- 103.2 --- foo(bar(move(CONST REF)))\n";
        Foo foo_0 = build(1032);
        Foo const& foo_r = foo_0;
        Foo foo_1 = bar(std::move(foo_r));
    }

    {
        std::cout << "--- 9901.0 --- = PLAIN / = move(PLAIN)\n";
        Foo foo_0 = build(99010);
        Foo foo_1 = foo_0;
        Foo foo_2 = std::move(foo_1);
    }

    {
        std::cout << "--- 9902.0 --- = REF / = move(REF)\n";
        Foo foo_0 = build(99020);
        Foo& foo_r = foo_0;
        Foo foo_1 = foo_r;
        Foo foo_2 = std::move(foo_r);
    }

    {
        std::cout << "--- 9903.0 --- = CONST REF / = move(CONST REF)\n";
        Foo foo_0 = build(99030);
        Foo const& foo_r = foo_0;
        Foo foo_1 = foo_r;
        Foo foo_2 = std::move(foo_r);
    }

    {
        std::cout << "-- 10001.1 --- cloneMoved(PLAIN)\n";
        Foo foo_0 = build(100011);
        Foo foo_1 = cloneMoved(foo_0);
    }

    {
        std::cout << "-- 10001.2 --- cloneFwd(PLAIN)\n";
        Foo foo_0 = build(100012);
        Foo foo_1 = cloneFwd(foo_0);
    }

    {
        std::cout << "-- 10002.1 --- cloneMoved(REF)\n";
        Foo foo_0 = build(100021);
        Foo& foo_r = foo_0;
        Foo foo_1 = cloneMoved(foo_r);
    }

    {
        std::cout << "-- 10002.2 --- cloneFwd(REF)\n";
        Foo foo_0 = build(100022);
        Foo& foo_r = foo_0;
        Foo foo_1 = cloneFwd(foo_r);
    }

    {
        std::cout << "-- 10003.1 --- cloneMoved(CONST REF)\n";
        Foo foo_0 = build(100031);
        Foo const& foo_r = foo_0;
        Foo foo_1 = cloneMoved(foo_r);
    }

    {
        std::cout << "-- 10003.2 --- cloneFwd(CONST REF)\n";
        Foo foo_0 = build(100032);
        Foo const& foo_r = foo_0;
        Foo foo_1 = cloneFwd(foo_r);
    }

    {
        std::cout << "-- 10004.1 --- cloneMoved(TEMPORARY)\n";
        Foo foo_1 = cloneMoved(build(100041));
    }

    {
        std::cout << "-- 10004.2 --- cloneFwd(TEMPORARY)\n";
        Foo foo_1 = cloneFwd(build(100042));
    }

    return 0;
}  // NOLINTEND(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,performance-move-const-arg,performance-unnecessary-copy-initialization)
