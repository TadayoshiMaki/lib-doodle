#include "doodle/IrPen.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

const std::string IrPen::kServiceUuid = "{CDB80001-C15C-471E-90EE-CE1413C86C5D}";
const std::string IrPen::kRgbLedConfigCharacteristicUuid = "{CDB80002-C15C-471E-90EE-CE1413C86C5D}";
const std::string IrPen::kRgbLedColorCharacteristicUuid = "{CDB80003-C15C-471E-90EE-CE1413C86C5D}";
const std::string IrPen::kIrLedConfigCharacteristicUuid = "{CDB80004-C15C-471E-90EE-CE1413C86C5D}";
const std::string IrPen::kTipStateCharacteristicUuid = "{CDB80005-C15C-471E-90EE-CE1413C86C5D}";
const std::string IrPen::kColorPickerCharacteristicUuid = "{CDB80006-C15C-471E-90EE-CE1413C86C5D}";

IrPen::IrPen()
{
}

IrPen::~IrPen()
{
    listeners_.clear();
    listeners_.shrink_to_fit();
}

void IrPen::foreachListeners(std::function<void(std::shared_ptr<IrPenListener>)> action)
{
    for (auto&& l : listeners_)
    {
        action(l);
    }
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
