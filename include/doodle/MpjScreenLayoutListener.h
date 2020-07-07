#ifndef LIB_DOODLE_FRAMEWORK_doodle_MpjScreenLayoutListener_H_
#define LIB_DOODLE_FRAMEWORK_doodle_MpjScreenLayoutListener_H_

#include <memory>
#include <vector>

#include "doodle/namespace.h"
#include "doodle/ScreensConfig.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class MpjScreenLayoutListener
{
public:
    /**
     * @brief Called when screens are loaded.
     * @param layouts The screens loaded.
     */
    virtual void onScreensLoaded(const std::vector<std::shared_ptr<Screen>>& screens) = 0;

    /**
     * @brief Called when the screen change is started.
     * @param oldScreenId The old current screen ID which changed from.
     * @param newScreenId The new current screen ID which changed to.
     */
    virtual void onScreenChangeStarted(const int oldScreenId, const int newScreenId) = 0;

    /**
     * @brief Called when the current screen is changed.
     * @param oldScreenId The old current screen ID which changed from.
     * @param newScreenId The new current screen ID which changed to.
     */
    virtual void onCurrentScreenChanged(const int oldScreenId, const int newScreenId) = 0;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_MpjScreenLayoutListener_H_
