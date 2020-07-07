/*
 * Copyright 2017 Sony Semiconductor Solutions Corporation.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 * No part of this file may be copied, modified, sold, and distributed in any
 * form or by any means without prior explicit permission in writing from
 * Sony Corporation.
 *
 */
#ifdef HSVCAMWRAP_EXPORTS
#define HSVCAMWRAP_API __declspec(dllexport)
#else
#define HSVCAMWRAP_API __declspec(dllimport)
#endif

#include <cstdint>
#include <string> 
#include <opencv2/opencv.hpp>

namespace so {
	class HsvCam;
}

namespace so {
	class HSVCAMWRAP_API CViewer{
		friend class HsvCamWrap;
	public:
		void Open(void);
		void ShowImage(void);
		void Close(void);

	private:
		CViewer();
		~CViewer(void);
		void SetCamHandle(HsvCam *pCam){ m_pCam = pCam; };

		enum {
			ST_INIT,
			ST_OPEN,
		} m_state;

		HsvCam *m_pCam;
	};
}

namespace so {
	class HSVCAMWRAP_API HsvCamWrap {
	public:
		struct Position_t {
			uint16_t x;
			uint16_t y;
		};

		struct SensingInformation_t {
			bool		result;
			uint32_t	moment0;
			uint32_t	moment1_x;
			uint32_t	moment1_y;
			Position_t	grvpos;
			Position_t	ave_grvpos;
			Position_t	mvpos;
			uint16_t	mvmgn;
			uint8_t		mvdir;
		};

		struct SensingInformationPacket_t {
			uint8_t frameCount;
			SensingInformation_t win[6];
		};

		HsvCamWrap(void);
		~HsvCamWrap(void);

		bool LoadSetupFile(const std::string &filepath);
		bool Setup(const std::string &paramSubDir);
		void Close(void);
		bool Start(void);
		bool Stop(void);
		bool GetRawImage(cv::Mat &img, SensingInformationPacket_t &sip);
		bool GetRgbImage(cv::Mat &img, SensingInformationPacket_t &sip);
		bool GetSensingInformation(SensingInformationPacket_t &sip);

		CViewer debug_viewer;

	private:
		void init(void);
		void deinit(void);

		HsvCam *m_pCam;
	};
}