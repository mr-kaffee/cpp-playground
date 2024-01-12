#include "Foo.hpp"

#include <iostream>

std::ostream& operator<<(std::ostream& stream, Foo const& value) {
    if (value._size > 0) {
        stream << value[0];
    } else {
        stream << "NULL";
    }
    stream << "[id: " << value._id << "]";
    return stream;
}
