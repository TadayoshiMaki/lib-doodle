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

#include "Singleton.h"
#include "so_def.h"

#include "IHsvComponent.h"

namespace so {
	namespace component {
		class IHsvComponent;
	}
}

namespace so {
	namespace component {

class HsvComponentFactory : public Singleton<HsvComponentFactory>
{
	friend Singleton<HsvComponentFactory>;
public:

	IHsvComponent* createComponent(SystemSelect system, RegisterInterface reg_if, int device_id);

#if 0	// ê⁄ë±USB deviceéÊìæèàóù
	static bool getDeviceList(std::vector<IHsvComponent::DeviceIdentify>& device_list);
#endif
private:
	HsvComponentFactory();
	~HsvComponentFactory();

	static void destroy();
};

	}
}