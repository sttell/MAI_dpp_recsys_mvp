#include "descriptor.hpp"

#include <iostream>
#include <stdexcept>
#include <cstring>

int hex_value(unsigned char hex_digit)
{
    static const signed char hex_values[256] = {
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
            -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    };
    int value = hex_values[hex_digit];
    if (value == -1) throw std::invalid_argument("invalid hex digit");
    return value;
}

namespace recsys_t2s::database {

    std::string DescriptorToHexBlob(const Descriptor& descriptor) {

        static const char hex_digits[] = "0123456789ABCDEF";

        std::string output;
        output.reserve(DESCRIPTOR_BYTES * 2 + 2);
        output.push_back('\"');
        const auto* data = reinterpret_cast<const unsigned char*>(descriptor.data());
        for (size_t i = 0; i < DESCRIPTOR_BYTES; i++)
        {
            const unsigned char c = data[i];
            output.push_back(hex_digits[c >> 4]);
            output.push_back(hex_digits[c & 15]);
        }
        output.push_back('\"');
        return output;
    }

    Descriptor HexBlobToDescriptor(const std::string& blob) {
        const auto len = blob.length();
        if (len & 1) throw std::invalid_argument("odd length");

        std::string output;
        output.reserve(len / 2);
        for (auto it = blob.begin(); it != blob.end(); )
        {
            int hi = hex_value(*it++);
            int lo = hex_value(*it++);
            output.push_back(hi << 4 | lo);
        }

        Descriptor descriptor;
        std::memcpy(descriptor.data(), output.data(), DESCRIPTOR_BYTES);
        return descriptor;
    }

}