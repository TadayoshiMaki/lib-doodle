/*
* Copyright 2017 Sony Semiconductor Solutions Corporation.
*
* This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Semiconductor Solutions Corporation.
* No part of this file may be copied, modified, sold, and distributed in any
* form or by any means without prior explicit permission in writing from
* Sony Semiconductor Solutions Corporation.
*
*/

#pragma once

#include "so_def.h"

namespace so
{
	namespace component
	{
		class IHsvComponent;
	}
}

namespace so {
    namespace sensing {
        class Common
        {
        public:
            enum DeviceStatus_t {
                DEVSTS_INITIAL,
                DEVSTS_PRE_ACTIVE,
                DEVSTS_INITIAL2,
                DEVSTS_ACTIVE
            };

			enum ColorFilter_t {
				BAYER,
				MONO
			};

			Common(so::component::IHsvComponent& component);
			~Common();

            uint16_t getSensorRomVersion(void);
            uint16_t getSensorParameterVersion(void);
            DeviceStatus_t getDeviceStatus(void);
            void setSensorMode(uint8_t mode);
            uint8_t getSensorMode(void);
            uint16_t getVmax(void);
            uint16_t getHmax(void);
            void setShutter(uint16_t val);
            uint16_t getShutter(void);
            float setAnalogGain(float analog_gain);
            float getAnalogGain(void);
            float setDigitalGain(float digital_gain);
            float getDigitalGain(void);
            void setFdGain(FdGain sel);
			FdGain getFdGain(void);
            void setWbGain(float gain_r, float gain_g, float gain_b);
            void setWbGainR(float gain_r);
            void setWbGainG(float gain_g);
            void setWbGainB(float gain_b);
            float getWbGainR(void);
            float getWbGainG(void);
            float getWbGainB(void);
            uint32_t getPixelClockHz(void);
            float getPixelClockMHz(void);
            float getFrameRate(void);
			ColorFilter_t getColorFilter(void);
		private:
			so::component::IHsvComponent&	m_component;

			uint16_t convWbGainFtoI(float f_gain);
            float convWbGainItoF(uint16_t i_gain);
		};
    }
}
