#ifndef CONVERTERCONSTANTS_H
#define CONVERTERCONSTANTS_H

#include <cstddef>
#include <stdint.h>

namespace converter
{

static const size_t MAX_NAME_LENGTH = 63; // + trailing 0x00
static const size_t FRAME_HEADER_LENGTH = 28; // + padded name string length
static const size_t VIDEO_HEADER_LENGTH = 16;
static const uint8_t HEADER_VERSION = 1;
static const uint8_t HEADER_SUBVERSION = 0;
static const uint32_t MEDIA_TYPE_VIDEO = 0x0100;
static const uint32_t MEDIA_SUBTYPE_VIDEO_UNCOMPRESSED = 0x0000;

static const size_t OFFSET_VIDEO_WIDTH = 0;
static const size_t OFFSET_VIDEO_HEIGHT = 4;
static const size_t OFFSET_VIDEO_BYTES_PER_LINE = 8;
static const size_t OFFSET_VIDEO_BITS_PER_PIXEL = 12;
static const size_t OFFSET_VIDEO_COLOR_FORMAT = 13;

enum HeaderOffset
{
    OFFSET_FRAME_SIZE = 0,
    OFFSET_HEADER_SIZE = 4,
    OFFSET_HEADER_VERSION = 8,
    OFFSET_HEADER_SUBVERSION = 9,
    OFFSET_NAME_LENGTH = 10,
    OFFSET_RESERVED_1 = 11,
    OFFSET_MEDIA_TYPE = 12,
    OFFSET_MEDIA_SUBTYPE = 16,
    OFFSET_TIMESTAMP = 20,
    OFFSET_NAME = 28,
};

} // namespace converter

#endif // CONVERTERCONSTANTS_H
