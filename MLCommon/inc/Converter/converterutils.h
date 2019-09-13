#ifndef CONVERTERUTILS_H
#define CONVERTERUTILS_H

#include <vector>
#include <cstddef>
#include <cstdint>

class ConverterUtils
{
public:
    static void setU32(std::vector<char> &destination, size_t offset, uint32_t value);
    static void setU64(std::vector<char> &destination, size_t offset, uint64_t value);
    static constexpr size_t getPaddedSize(size_t originalSize)
    {
        return originalSize + (4 - (originalSize % 4)) % 4;
    }
    static uint32_t getU32(const std::vector<char> &data, size_t offset);
    static uint64_t getU64(const std::vector<char> &data, size_t offset);
};

#endif // CONVERTERUTILS_H
