#ifndef LIB_DOODLE_FRAMEWORK_doodle_StrokeStatusBank_H_
#define LIB_DOODLE_FRAMEWORK_doodle_StrokeStatusBank_H_

#include <map>

#include "doodle/namespace.h"
#include "doodle/StrokeStatus.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

// This class is tentative in refactoring.
class StrokeStatusBank
{
public:
    void foreach(std::function<void(int, std::shared_ptr<StrokeStatus>)> action) {
        for (const auto &[id, status] : map_)
        {
            action(id, status);
        }
    }

    void foreachId(std::function<void(int)> action) {
        for (const auto& [id, status] : map_)
        {
            action(id);
        }
    }

    void clear() {
        map_.clear();
    }

    const std::shared_ptr<StrokeStatus> & get(int id) const {
        return map_.at(id);
    }

    void put(int id, std::shared_ptr<StrokeStatus> &status) {
        map_.emplace(id, std::move(status));
    }

    const std::shared_ptr<StrokeStatus> & tryGet(int id) const {
        auto &it = map_.find(id);
        if (it == map_.end()) {
            return std::move(empty);
        }
        const auto &[ignored, status] = *it;
        return status;
    }

    size_t size() {
        return map_.size();
    }

    std::pair<int, std::shared_ptr<StrokeStatus>> first() {
        auto &it = map_.begin();
        return *it;
    }

private:
    // Used to hold the current stroke state which is transited by events.
    // TODO: Use a statemachine to handle each stroke state.
    std::map<int, std::shared_ptr<StrokeStatus>> map_;

    const std::shared_ptr<StrokeStatus> empty;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_StrokeStatusBank_H_
