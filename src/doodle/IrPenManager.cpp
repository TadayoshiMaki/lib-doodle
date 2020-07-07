#include "doodle/IrPenManager.h"
#include "doodle/BleIrPenManager.h"

//#include "async_ir_pen.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

const Color IrPenManager::kInitialRgbLedColor = { 255, 255, 255 };

IrPenManager::IrPenManager()
{
}

IrPenManager::~IrPenManager()
{
}

std::shared_ptr<IrPenManager> IrPenManager::newInstance()
{
    return std::make_shared<BleIrPenManager>();
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
