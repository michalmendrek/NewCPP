#include "converterutils.h"

/*!
 * \brief ConverterUtils::setU32 - helper function that writes 32-bit value into the header as 4 bytes.
 * \param destination
 * \param offset
 * \param value
 */
void ConverterUtils::setU32(std::vector<char> &destination, size_t offset, uint32_t value)
{
    destination[offset] = static_cast<int8_t>((value >> 24) & 0xFF);
    destination[offset + 1] = static_cast<int8_t>((value >> 16) & 0xFF);
    destination[offset + 2] = static_cast<int8_t>((value >> 8) & 0xFF);
    destination[offset + 3] = static_cast<int8_t>((value >> 0) & 0xFF);
}

/*!
 * \brief ConverterUtils::setU64 - helper function that writes 64-bit value into the header as 8 bytes.
 * \param destination
 * \param offset
 * \param value
 */
void ConverterUtils::setU64(std::vector<char> &destination, size_t offset, uint64_t value)
{
    setU32(destination, offset, static_cast<uint32_t>((value >> 32) & 0xFFFFFFFF));
    setU32(destination, offset + 4, static_cast<uint32_t>(value & 0xFFFFFFFF));
}

uint32_t ConverterUtils::getU32(const std::vector<char> &data, size_t offset)
{
    return static_cast<uint32_t>((static_cast<uint8_t>(data[offset]) << 24)
                                 | (static_cast<uint8_t>(data[offset + 1]) << 16)
                                 | (static_cast<uint8_t>(data[offset + 2]) << 8)
                                 | (static_cast<uint8_t>(data[offset + 3]) << 0));
}

uint64_t ConverterUtils::getU64(const std::vector<char> &data, size_t offset)
{
    return (static_cast<uint64_t>(getU32(data, offset)) << 32) | getU32(data, offset + 4);
}
