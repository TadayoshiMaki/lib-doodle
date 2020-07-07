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

#include "fpga/so_fpga_reg_addr.h"

namespace so
{
	namespace component
	{
		class IHsvComponent;
	}
}

namespace so
{
    namespace fpga
    {
		class Fpga
		{

		public:
			Fpga(so::component::IHsvComponent& component);
			~Fpga();

			void getSensingFirmwareVersion(SensingFirmwareVersion_t & ver);
			void reboot(const Config_t & config);
			MainStatus_t getMainStatus(void);
			uint8_t getFrameCounterMain(void);
			uint8_t getFrameCounterDma(void);

			void siOutSetMode(SiOutMode mode);
			SiOutMode siOutGetMode(void);
			void siOutSetCsMode(SiOutCsMode endian);
			SiOutCsMode siOutGetCsMode(void);
			void siOutSetHFlip(bool en);
			bool siOutGetHFlip(void);
			void siOutSetVFlip(bool en);
			bool siOutGetVFlip(void);
			void siOutSetScalingFactor(float scalingFactor);
			float siOutGetScalingFactor(void);
			void siOutSetNum(uint8_t num);
			uint8_t siOutGetNum(void);
			void siOutSetFramePeriod(uint8_t framePeriod);
			uint8_t siOutGetFramePeriod(void);
			void siOutSetImageWidth(uint16_t width);
			uint16_t siOutGetImageWidth(void);
			void siOutSetImageHeight(uint16_t height);
			uint16_t siOutGetImageHeight(void);

			void setSensorMode(uint8_t sonsor_mode);
			uint8_t getSensorMode(void);

			void setResetConfigExecute(bool en);
			bool getResetConfigExecute(void);

		private:
			so::component::IHsvComponent&	m_component;
		};
    }
}
