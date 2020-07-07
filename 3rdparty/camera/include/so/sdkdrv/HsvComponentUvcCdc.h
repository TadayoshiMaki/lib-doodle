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

#include "usb/usbclnt_api.h"

#include "IHsvComponent.h"

namespace so {
	namespace component {
		namespace stream {
			class StreamUsbUvc;
		}
		namespace command {
			class CommandUsbCdc;
		}
	}
}

namespace so {
	namespace component {

class HsvComponentUvcCdc : public IHsvComponent {

public:

	HsvComponentUvcCdc();
	virtual ~HsvComponentUvcCdc();

	Status_t initComponent(const IHsvComponent::InitParam& param, const CallBack_t &callback);

	bool open();
	bool close();

	void getDeviceList(std::vector<DeviceIdentify>& device_list);

	void getPacketSize(int &width, int &height);

	// -----------------------------------
	// Asyncronous process
	// -----------------------------------
	bool startCapture();
	bool stopCapture();

	// -----------------------------------
	// Syncronous process
	// -----------------------------------
	bool start();
	bool getCurrentFrame(uint8_t* buff, uint32_t buff_size);
	bool stop();




	uint8_t readSensRegU8(uint32_t addr);
	uint16_t readSensRegU16(uint32_t addr);
	uint32_t readSensRegU32(uint32_t addr);
	uint32_t readSensRegU32(uint32_t addr, uint32_t msb, uint32_t lsb);

	void writeSensRegU8(uint32_t addr, uint8_t val);
	void writeSensRegU16(uint32_t addr, uint16_t val);
	void writeSensRegU32(uint32_t addr, uint32_t val);
	void writeSensRegU32(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t val);

	uint8_t readFpgaRegU8(uint32_t addr);
	uint16_t readFpgaRegU16(uint32_t addr);
	uint32_t readFpgaRegU32(uint32_t addr);
	uint32_t readFpgaRegU32(uint32_t addr, uint32_t msb, uint32_t lsb);

	void writeFpgaRegU8(uint32_t addr, uint8_t val);
	void writeFpgaRegU16(uint32_t addr, uint16_t val);
	void writeFpgaRegU32(uint32_t addr, uint32_t val);
	void writeFpgaRegU32(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t val);


	StateCode writeSensReg(uint8_t cat, uint16_t offset, uint8_t write_data[], int write_size);
	StateCode readSensReg(uint8_t cat, uint16_t offset, uint8_t* read_buff_ptr, int read_size);

	StateCode unlockSensFlash();
	StateCode allWriteSensFlash();
	StateCode eraseSensFlash(uint32_t addr);
	StateCode writeSensFlash(uint32_t addr, uint8_t write_data[], int write_size);
	StateCode readSensFlash(uint32_t addr, uint8_t* read_buff_ptr, int read_size);

	StateCode writeFpgaReg(uint8_t cat, uint16_t offset, uint8_t write_data[], int write_size);
	StateCode readFpgaReg(uint8_t  cat, uint16_t offset, uint8_t* read_buff_ptr, int read_size);

	StateCode unlockFpgaFlash();
	StateCode allWriteFpgaFlash();
	StateCode eraseFpgaFlash(uint32_t addr);
	StateCode writeFpgaFlash(uint32_t addr, uint8_t write_data[], int write_size);
	StateCode readFpgaFlash(uint32_t addr, uint8_t* read_buff_ptr, int read_size);

	StateCode rebootSens(void);
	StateCode rebootFpga(void) { return SUCCESS; };

	StateCode startLog(LogMode mode);
	StateCode stopLog();
	StateCode getLog(uint8_t index, uint8_t number);

	uint8_t getLastResponseError() { return 0; }

    StateCode getDriverInfo(DriverInfo &info);

private:

	command::CommandUsbCdc* m_command;
	stream::StreamUsbUvc* m_stream;

	DeviceIdentify m_identify;
	std::string m_port_name;
	uint32_t m_baudrate;
};

	}
}