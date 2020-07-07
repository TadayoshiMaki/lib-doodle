#ifndef LIB_DOODLE_FRAMEWORK_doodle_HsvSensorProd_H_
#define LIB_DOODLE_FRAMEWORK_doodle_HsvSensorProd_H_

#include <string>

#include <StApi_TL.h>

#include "doodle/namespace.h"
#include "doodle/HsvSensor.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

using StApi::CIStDataStreamPtr;
using StApi::CIStDevicePtr;
using StApi::CIStStreamBufferPtr;
using StApi::CIStSystemPtr;
using StApi::CStApiAutoInit;
using StApi::IStImage;
using StApi::IStPixelFormatInfo;
using StApi::EStPixelFormatNamingConvention_t;
using GenApi::CNodeMapPtr;

class SentechHsvSensor : public HsvSensor
{
public:
    class Config : public HsvSensor::Config
    {
    public:
        Config(const std::string& configFilePath) :
            configFilePath_(configFilePath)
        {
        }

        ~Config()
        {
        }

        std::string getConfigFilePath()
        {
            return configFilePath_;
        }

    private:
        std::string configFilePath_;
    };

    SentechHsvSensor();
    virtual ~SentechHsvSensor();

    /**
     * @brief Returns the width of captured frames.
     * @return The width in pixels.
     */
    uint32_t getWidth() override;

    /**
     * @brief Returns the height of captured frames.
     * @return The height in pixels.
     */
    uint32_t getHeight() override;

    /**
     * @brief Returns the pixel format of captured frames.
     * @return The pixel format.
     */
    HsvSensor::PixelFormat getPixelFormat() override;

protected:
    /**
     * @brief Called to open a high-speed vision sensor.
     * @return true if successfully opened.
     */
    bool onOpenSensor(const std::shared_ptr<HsvSensor::Config>& config) override;

    /**
     * @brief Called to close a high-speed vision sensor.
     */
    void onCloseSensor() override;
    /**
     * @brief Called to start a high-speed vision sensor.
     * @return true if successfully started.
     */
    bool onStartSensor() override;
    /**
     * @brief Called to stop a high-speed vision sensor.
     * @return true if successfully stopped.
     */
    bool onStopSensor() override;
    /**
     * @brief Captures frames in a loop on the worker thread.
     */
    void run() override;

    // The image width.
    int imageWidth_;
    // The image height.
    int imageHeight_;
    // The pixel format of images.
    EStPixelFormatNamingConvention_t imagePixelFormat_;

    // The Sentech SDK library interface.
    CStApiAutoInit lib_;

    // The system module interface.
    CIStSystemPtr system_;
    // The device interface.
    CIStDevicePtr device_;
    // The data stream interface.
    CIStDataStreamPtr stream_;

    /**
     * @brief Loads sensor configuration from the specified file.
     */
    bool loadConfiguration(const std::string& configFilePath);

    /**
     * @brief Returns a value from the node map.
     * @param name The property name.
     * @param nodeMap The node map.
     * @return The value.
     */
    template<typename T>
    T getValue(const CNodeMapPtr& nodeMap, const gcstring& name);

    template<typename T>
    bool setValue(const CNodeMapPtr& nodePtr, const gcstring& name, T value);

    /**
     * @brief Called to acquires a current image through the stream buffer.
     */
    void onAcquireImage(const CIStStreamBufferPtr& buffer);

    /**
     * @brief Called to acquires a current image as a color image.
     * @param image The image.
     */
    void onAcquireColorImage(IStImage *image);

    /**
     * @brief Called to acquires a current image as a monochrome image.
     * @param image The image.
     * @param pixelFormatInfo The pixel format information to get bit depth of images.
     */
    void onAcquireMonoImage(IStImage *image, const IStPixelFormatInfo& pixelFormatInfo);
};

template<typename T>
inline T SentechHsvSensor::getValue(const CNodeMapPtr& nodeMap, const gcstring& name)
{
    using GenApi::CBooleanPtr;
    using GenApi::CEnumerationPtr;
    using GenApi::CFloatPtr;
    using GenApi::CIntegerPtr;
    using GenApi::CNodePtr;
    using GenApi::EInterfaceType;

    T value = 0;
    CNodePtr node(nodeMap->GetNode(name));
    bool nodeReadable = GenApi::IsReadable(node);
    if (!nodeReadable) return value;

    EInterfaceType type = node->GetPrincipalInterfaceType();
    switch (type)
    {
    case EInterfaceType::intfIBoolean:
    {
        CBooleanPtr property(node);
        if (!property) break;
        value = property->GetValue();
        return (bool)value;
    }
    case EInterfaceType::intfIInteger:
    {
        CIntegerPtr property(node);
        if (!property) return (int)value;
        value = property->GetValue();
        return (int)value;
    }
    case EInterfaceType::intfIFloat:
    {
        CFloatPtr property(node);
        if (!property) return (int)value;
        value = property->GetValue();
        return (float)value;
    }
    case EInterfaceType::intfIEnumeration:
    {
        CEnumerationPtr property(node);
        if (!property) return (int)value;
        value = property->GetIntValue();
        return (int)value;
    }
    default:
        break;
    }
    return value;
}

template<typename T>
inline bool SentechHsvSensor::setValue(const CNodeMapPtr& nodePtr,
                                       const gcstring& name,
                                       T value)
{
    using GenApi::CBooleanPtr;
    using GenApi::CEnumerationPtr;
    using GenApi::CFloatPtr;
    using GenApi::CIntegerPtr;
    using GenApi::CNodePtr;
    using GenApi::EInterfaceType;

    CNodePtr node(nodePtr->GetNode(name));
    bool available = GenApi::IsAvailable(node);
    bool writable = GenApi::IsWritable(node);
    if (!available || !writable) return false;

    EInterfaceType type = node->GetPrincipalInterfaceType();
    switch (type)
    {
    case EInterfaceType::intfIBoolean:
    {
        CBooleanPtr property(node);
        if (!property) return false;
        property->SetValue(value);
        return true;
    }
    case EInterfaceType::intfIInteger:
    {
        CIntegerPtr property(node);
        if (!property) return false;
        property->SetValue(value);
        return true;
    }
    case EInterfaceType::intfIFloat:
    {
        CFloatPtr property(node);
        if (!property) return false;
        property->SetValue(value);
        return true;
    }
    case EInterfaceType::intfIEnumeration:
    {
        GenApi::CEnumerationPtr property(node);
        if (!property) return false;
        property->SetIntValue(value);
        return true;
    }
    default:
        break;
    }
    return false;
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_HsvSensorProd_H_
