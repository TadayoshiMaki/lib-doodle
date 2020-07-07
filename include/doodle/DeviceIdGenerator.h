#ifndef LIB_DOODLE_FRAMEWORK_doodle_DeviceIdGenerator_H_
#define LIB_DOODLE_FRAMEWORK_doodle_DeviceIdGenerator_H_

#include <map>
#include <vector>

#include "doodle/namespace.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class DeviceIdGenerator
{
public:
    static const int kInvalidId = -1;

    DeviceIdGenerator()
    {
    }

    virtual ~DeviceIdGenerator()
    {
        ids_.clear();
    }

    int generate(const std::string& address)
    {
        const int deviceId = get(address);
        if (deviceId != kInvalidId)
        {
            // Already exists.
            return deviceId;
        }
        size_t size = ids_.size();
        ids_.insert(std::make_pair(address, size));
        return size;
    }

    int get(const std::string& address)
    {
        auto it = ids_.find(address);
        if (it != ids_.end())
        {
            // Already exists.
            return it->second;
        }
        return kInvalidId;
    }

    std::vector<int> getIds()
    {
        std::vector<int> values;
        for (auto& it = ids_.begin(); it != ids_.end(); ++it) {
            values.push_back(it->second);
        }
        return std::move(values);
    }

private:
    // TODO: Save IDs into a file and load it if they should be immutable after restarting the app.
    std::map<const std::string, int> ids_;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_DeviceIdGenerator_H_
