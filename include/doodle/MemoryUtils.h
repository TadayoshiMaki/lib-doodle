#ifndef LIB_DOODLE_FRAMEWORK_doodle_MemoryUtils_H_
#define LIB_DOODLE_FRAMEWORK_doodle_MemoryUtils_H_

#include <memory>

#include "doodle/namespace.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

/**
 * Trick to allow multiple inheritance of objects inheriting shared_from_this.
 * https://stackoverflow.com/a/12793989/587407
 */

 /**
  * First a common base class.
  * Of course, one should always virtually inherit from it.
  */
class MultipleInheritableEnableSharedFromThis :
    public std::enable_shared_from_this<MultipleInheritableEnableSharedFromThis>
{
public:
    virtual ~MultipleInheritableEnableSharedFromThis()
    {
    }
};

template <class T>
class inheritable_enable_shared_from_this :
    virtual public MultipleInheritableEnableSharedFromThis
{
public:
    std::shared_ptr<T> shared_from_this() {
        return std::dynamic_pointer_cast<T>(MultipleInheritableEnableSharedFromThis::shared_from_this());
    }
    /**
     * @brief Utility method to easily downcast. Useful when a child doesn't inherit
     *        directly from enable_shared_from_this but wants to use the feature.
     */
    template <class Derived>
    std::shared_ptr<Derived> downcasted_shared_from_this() {
        return std::dynamic_pointer_cast<Derived>(MultipleInheritableEnableSharedFromThis::shared_from_this());
    }
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_MemoryUtils_H_
