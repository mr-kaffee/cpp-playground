#ifndef FOO_HPP
#define FOO_HPP

#include <algorithm>
#include <iostream>

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
    // NOLINTBEGIN(bugprone-easily-swappable-parameters)
    /**
     * Construct instance
     * @param size the size of the memory to allocate
     */
    explicit Foo(int size, int val) : _id{objectCount++}, _size{size} {
        alloc();
        if (_size > 0) { (*this)[0] = val; }
        std::cout << "CTOR " << *this << "\n";
    }  // NOLINTEND(bugprone-easily-swappable-parameters)

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

#endif
