#ifndef LIB_DOODLE_FRAMEWORK_doodle_ScreenProperties_H_
#define LIB_DOODLE_FRAMEWORK_doodle_ScreenProperties_H_

#include <cstdint>

#include "doodle/namespace.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

struct ScreenProperty
{
    int id;
    uint32_t width;
    uint32_t height;

    ScreenProperty()
    {
    }

    ScreenProperty(const int id,
                   const uint32_t width,
                   const uint32_t height) :
        id(id), width(width), height(height)
    {
    }
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_ScreenProperties_H_
