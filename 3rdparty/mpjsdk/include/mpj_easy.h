//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_easy.h
//! \brief Easy interface for MPJ SDK
//---------------------------------------------------------------------------

#ifndef _mpj_easy_h_
#define _mpj_easy_h_

#include "mpj_common.h"
#include "mpj_camera.h"
#include "mpj_motor.h"
#include "mpj_depth.h"
#include "mpj_geodesicdome.h"
#include "mpj_pointdetector.h"
#include "mpj_depthsensormanager.h"
#include "mpj_geodesicdomemapper.h"
#include "mpj_transform2d.h"
#include "mpj_vector2d.h"
#include "mpj_frameoperator.h"

#include <functional>

namespace mpjsdk
{
	//! Parameters for \ref mpj_Easy.
	/*!
	For the specification of the JSON file, see \ref easyjson.

	The following fragment illustrates how to load the configuration from JSON file:
	\code{.cpp}
	mpj_EasyParams params("sample.json");

	// you can change the parameters
	params.serialPort = "COM2";
	...

	// of course, you can save the configuration back to file
	params.saveConfigToJsonFile("another.json");
	\endcode
	\sa \ref mpj_Easy
	*/
	class mpj_EasyParams
	{
	public:
		//! Initialize the instance with the default parameters.
		mpj_EasyParams()
		{
			// dummy file name
			jsonFileName = "config.json";

			//
			// Default parameters
			//
			motorModel = mpj_KnownMotors::DXL_MX28T_DPJ_F;
			serialPort = "COM6";

			cameraModel = mpj_KnownCameras::IDS_uEye;
			cameraGain = 100.f;
			cameraPixelClock = 70.f;
			cameraExposure = 33.f;

			pointTransformCsvFileName = "table.csv";
			pointDetectFrameMask = "fishEyeCamMask_half.bmp";
			pointDetectBinThrehold = 128;
			pointDetectZeta = 5.f;

			geodesicDomePcdFileName = "";
			geodesicDomeLevel = mpj_GeodesicDome::DefaultGeodesicDomeLevel;

			planeDetectSize = 350;

			projectorWidth = 1280;
			projectorHeight = 800;

			projIntrinsic = mpj_Mat4(2050, 0, 640, 0, 2050, 110, 0, 0, 1);

			projTranslate = mpj_Vec3(49.25f, 0, 13.017000198f);
			depthCameraTranslate = mpj_Vec3(19.5f, -42.400001526f, 15);
			fishEyeTranslate = mpj_Vec3(0, -97, 210);
			fishEyeRotate = mpj_Angle(0, -30);

			pointDetectEnabled = true;
		}

		//! Initialize the instance by loading configuration from JSON file.
		/*!
		\param jsonFileName Configuration JSON file name.
		The function also updates \ref jsonFileName with the actual loaded JSON file path.
		\sa \ref loadConfigFromJsonFile.
		*/
		explicit mpj_EasyParams(const _FSPFX path & jsonFileName) : mpj_EasyParams()
		{
			loadConfigFromJsonFile(jsonFileName);
		}

		//! Load configuration from JSON file.
		/*!
		\param jsonFileName Configuration JSON file name.
		The function also updates \ref jsonFileName with the actual loaded JSON file path.
		*/
		void loadConfigFromJsonFile(const _FSPFX path & jsonFileName);

		//! Load configuration from JSON string.
		/*!
		\param jsonString Configuration in JSON formatted string.
		*/
		void loadConfigFromString(const std::string& jsonString);

		//! Path converter type definition, which accepts a path and returns another path.
		using PathConverterFunc = std::function<_FSPFX path(const _FSPFX path&)>;

		//! Save configuration to JSON file.
		/*!
		\param jsonFileName Configuration JSON file name. If the file exists, it will be overwritten.
		\param pathConverter Function called on every file path in the configuration name to make the file path portable on the JSON file; if nullptr is specified, it uses the default implementation based on the specified JSON file name. For more information, see \ref getRelativePathConverterForDirectoryPath
		\sa \ref serialize, \ref getRelativePathConverterForDirectoryPath

		*/
		void saveConfigToJsonFile(const _FSPFX path & jsonFileName, const PathConverterFunc& pathConverter = nullptr) const;

		//! Serialize configuration to a JSON formatted string.
		/*!
		\param pathConverter Function to make file paths portable on the JSON. It is typically used to convert absolute file path to relative file path based on the target JSON file path. The class provides a stock implementation, that is returned by \ref getRelativePathConverterForDirectoryPath. The converters convert paths to relative paths based on a directory.
		\sa \ref getRelativePathConverterForDirectoryPath
		*/
		std::string serialize(const PathConverterFunc& pathConverter = nullptr) const;

		//! Obtain a path converter that converts each file path to relative file path based on a directory.
		/*!
		\param baseDir Base directory used to resolve relative path for every file path.
		\return Function to convert file paths.

		The following fragment illustrates how to use the function:
		\code{.cpp}
		mpj_EasyParams params("test.json");

		// The JSON string returned from serialize method contains several files names relative to c:\testdir.
		auto jsonStr = params.serialize(params.getRelativePathConverterForDirectoryPath("c:\\testdir"));
		\endcode

		\sa \ref serialize, \ref saveConfigToJsonFile
		*/
		PathConverterFunc getRelativePathConverterForDirectoryPath(const _FSPFX path& baseDir) const;

		//! Obtain a path converter that converts each file path to relative file path to the current JSON path (\ref jsonFileName).
		PathConverterFunc getRelativePathConverterForCurrentJsonPath() const
		{
			if (jsonFileName.empty())
				return [](auto f) { return f; };
			auto dest = _FSPFX canonical(jsonFileName).parent_path();
			return getRelativePathConverterForDirectoryPath(dest);
		}

		//! Resolve full path for the input file path based on the \ref jsonFileName.
		/*!
		\param filePath File to resolve the full path.
		\return
		If the input file path is already a full path, the function simply returns it.
		If the input is empty path, the method return empty path.
		*/
		_FSPFX path getFullPathNameFor(_FSPFX path filePath) const
		{
			auto parentDir = jsonFileName.parent_path();
			if (!filePath.empty() && !parentDir.empty() && filePath.is_relative())
				return parentDir / filePath;
			return filePath;
		}

		/*!
			The value is updated when the configuration is loaded from JSON file; see \ref loadConfigFromJsonFile.
		*/
		_FSPFX path jsonFileName; //!< The JSON file path the configuration loaded from.

		// motor
		/*! See \ref mpj_KnownMotors for the known motor list. */
		std::string motorModel; //!< Motor model name.
		std::string serialPort; //!< Serial port (COMx) for motor.
		mpj_MotorParams motorParams; //!< Parameters to initialize motor with.

		// camera
		/*! See \ref mpj_KnownCameras for the known motor list. */
		std::string cameraModel; //!< Camera model name.
		/*! See \ref mpj_CameraConfigManager for more info. */
		float cameraGain; //!< Camera gain.
		/*! See \ref mpj_CameraConfigManager for more info. */
		float cameraPixelClock; //!< Camera pixel clock.
		/*! See \ref mpj_CameraConfigManager for more info. */
		float cameraExposure; //!< Camera exposure.

		// point-detector
		/*! See \ref mpj_Transform2d for more info. */
		_FSPFX path pointTransformCsvFileName; //!< Table CSV file.
		/*! See \ref mpj_PointDetector::setFrameMask for more info. */
		_FSPFX path pointDetectFrameMask; //!< Frame mask image.
		uint8_t pointDetectBinThrehold; //!< Bitonalization threshold.
		/*! See \ref mpj_PointDetector for more info.  */
		float pointDetectZeta; //!< Zeta for \ref mpj_PointDetector.
		bool pointDetectEnabled; //!< If false, rotation is by manually swing moving projector.

		uint32_t planeDetectSize; //!< Extent for the PCD extraction on plane detection.

		/*! Higher value results in more point cloud density. See \ref mpj_GeodesicDome for more info. */
		int geodesicDomeLevel; //!< Geodesic dome level.
		/*!
		No PCD file is loaded if it is empty string or nullptr.
		*/
		_FSPFX path geodesicDomePcdFileName; //!< Pregenerated PCD file.

		/*!
		\sa \ref projIntrinsic, \ref projectorHeight
		*/
		uint32_t projectorWidth; //!< Projector's screen width in pixels.
		/*!
		\sa \ref projIntrinsic, \ref projectorWidth
		*/
		uint32_t projectorHeight; //!< Projector's screen height in pixels.
		/*!
		For the definition about intrinsic matrix, see \ref about_intrinsic.
		\sa \ref projectorWidth, \ref projectorHeight
		*/
		mpj_Mat4 projIntrinsic; //!< Projector intrinsic matrix.
		/*!
		\sa \ref proj_translate
		*/
		mpj_Vec3 projTranslate; //!< Projector optical center's 3D offset from the projector's rotation origin in millimeters.
		/*!
		\sa \ref dc_translate
		*/
		mpj_Vec3 depthCameraTranslate; //!< Depth-sensor optical center's 3D offset from the projector's rotation origin in millimeters.
		/*!
		\sa \ref fec_translate
		*/
		mpj_Vec3 fishEyeTranslate; //!< Fish-eye camera optical center's 3D offset from the projector's rotation origin in millimeters.
		/*!
		\sa \ref fec_rotate
		*/
		mpj_Angle fishEyeRotate; //!< Rotation (pan/tilt) angles of fish-eye camera from the Z-axis in degrees.
	};

	//! Directions and position to emulate projector
	class mpj_EasyProjectorDirections
	{
	public:
		//
		// NOTE: DO NOT change variable order (or adding some) without updating calculateProjectorVectors* implementations
		//
		mpj_Vec3 leftTopDirection; //!< Direction vector from camera center to projection left-top corner. (Normally normalized)
		mpj_Vec3 rightTopDirection; //!< Direction vector from camera center to projection right-top corner. (Normally normalized)
		mpj_Vec3 rightBottomDirection; //!< Direction vector from camera center to projection right-bottom corner. (Normally normalized)
		mpj_Vec3 leftBottomDirection; //!< Direction vector from camera center to projection left-bottom corner. (Normally normalized)
		mpj_Vec3 cameraCenterPosition; //!< Camera center position.
		mpj_Vec3 rayCenterDirection; //!< Direction vector from camera center to ray center (projection center). (Normally normalized)
		mpj_Vec3 cameraLookAtDirection; //!< Direction vector camera looks at.
		mpj_Vec3 cameraUpDirection; //!< Direction vector of camera up direction.
	};

	//! Plane detection result.
	struct mpj_PlaneDetectionResult
	{
		//! Whether the detection result is valid or not.
		bool isValid;
		//! Angle of the detection.
		mpj_Angle angle;
		//! Quaternion describing the plane's rotation status.
		mpj_Quaternion q;
		//! Plane detection result.
		mpj_PlaneInfo plane;

		//! Initialization with the default values.
		mpj_PlaneDetectionResult() : isValid(false) {}
	};

	//! Easy interface for point-to-swing use case.
	/*!
	The following fragment illustrates how to load the configuration from JSON file:
	\code{.cpp}
	mpj_EasyParams params;
	params.loadConfigFromJsonFile("sample.json");

	// you can change the parameters
	params.serialPort = "COM2";

	mpj_Easy easy(params);

	// load point cloud data from a PCD file.
	easy.loadGeodesicDome("sample.pcd");

	// MPJ SDK uses milli-meter as the unit for distance/length.
	// farDist=10000 means that we see the things inside 10 meter sight.
	const float nearDist = 1, farDist = 10000;
	float l, t, r, b;
	easy.calculateProjectorFrustum(l, r, b, t, nearDist);
	glFrustum(l, r, b, t, nearDist, farDist);

	mpj_PlaneDetectionResult result;

	// main loop
	while (true)
	{
	  easy.swingToPointerDirection();

	  easy.updateResultIfPlaneDetected(result);

	  if (result.isValid)
	  {
	    // calculate camera rotation
	    auto dirs = easy.calculateProjectorVectorsRotated(newResult.angle, nearDist, 5000);
		// for gluLookAt or such function, we should calculate "position" rather than "direction"
		auto cameraLookAtPosition = dirs.cameraCenterPosition + dirs.cameraLookAtDirection;
		gluLookAt(
		  dirs.cameraCenterPosition.x, dirs.cameraCenterPosition.y, dirs.cameraCenterPosition.z,
	      cameraLookAtPosition.x, cameraLookAtPosition.y, cameraLookAtPosition.z,
		  dirs.cameraUpDirection.x, dirs.cameraUpDirection.y, dirs.cameraUpDirection.z);

		// calculate vertices of the image drawn
		// NOTE: origin: (0,0,0) is the rotation center of MPJ
		//       X-axis: right-to-left
		//       Y-axis: bottom-to-up
		//       Z-axis: depth
		const auto& params = easy.getParams();
		auto half_width = static_cast<float>(params.projectorWidth) / 2;
		auto half_height = static_cast<float>(params.projectorHeight) / 2;
		mpj_Vec3 p[4] = {
			{ -half_width / 2, half_height / 2, 0 },  // right-bottom
			{ -half_width / 2, -half_height / 2, 0 }, // right-top
			{ half_width / 2, half_height / 2, 0 },   // left-top
			{ half_width / 2, -half_height / 2, 0 }   // left-bottom
		};
		auto qmat = mpj_Mat4::createFromQuaternion(result.q);
		for (size_t i = 0; i < 4; i++)
		  p[i] = qmat * p[i] + result.plane.center; // first, rotate the point and then offset by plane center.

		drawImage(p[0], p[1], p[2], p[3]);

		...
	  }
	}
	\endcode
	*/
	class mpj_Easy
	{
	public:
		//! Initialize an instance with the specified parameters.
		/*!
		\param params Parameters to initialize the instance with.
		*/
		explicit mpj_Easy(const mpj_EasyParams& params) : params(params)
		{
			setup();
		}

		//! Initialize an instance with the parameters loaded from the specified file.
		/*!
		\param jsonFileName Parameter JSON file name. It can be nullptr if you don't want to load configuration JSON file.
		*/
		explicit mpj_Easy(const _FSPFX path & jsonFileName)
		{
			if (!jsonFileName.empty())
				params.loadConfigFromJsonFile(jsonFileName);
			setup();
		}

		virtual ~mpj_Easy()
		{
			mpj_Debug::log("mpj_Easy: terminated.");
		}

		//! Run geodesic dome mapping process.
		/*!
			The process will clear the existing geodesic dome.
			\sa \ref mpj_GeodesicDomeMapper
		*/
		void runGeodesicDomeMapping()
		{
			try
			{
				// FIXME: depth camera init-params are fixed here :(
				auto depthCamera = mpj_DepthSensorManager::create()->createDepthCamera(640, 480, 30);
				depthCamera->startCapture();

				enableMotorTorque(true);

				{
					mpj_GeodesicDomeMapper mapper(params.depthCameraTranslate);
					geodesicDome = mapper.doMapping(depthCamera, motorManager);
				}

				revertMotorTorque();
			}
			catch (...)
			{
				revertMotorTorque();
				throw;
			}
		}

		//! Load geodesic dome mapping data.
		/*!
		\param fileName PCD file name to load.
		*/
		bool loadGeodesicDome(const _FSPFX path& fileName)
		{
			geodesicDome = std::make_shared<mpj_GeodesicDome>(fileName, false, params.geodesicDomeLevel);
			return geodesicDome->isValid();
		}

		//! Determine whether geodesic dome data is ready (loaded or generated) or not.
		/*!
		\return true if the data is loaded or generated already; otherwise false.
		*/
		bool isGeodesicDomeReady() const
		{
			return geodesicDome && geodesicDome->isValid();
		}

		//! Detect pointer and swing head to that direction.
		/*!
		\param frame Frame to detect point. If it is nullptr, the method tries to obtain a frame from camera.
		*/
		void swingToPointerDirection(const mpj_Image* frame = nullptr)
		{
			mpj_Angle angle;
			if (calculateMotorAngleByDetectPointer(angle, frame))
				motorManager->setGoalAngle(angle);
		}

		//! Calculate motor angle by detecting pointer.
		/*!
		\param angle Receive motor angle.
		\param frame Frame to detect point. If it is nullptr, the method tries to obtain a frame from camera.
		\return true if motor angle is correctly calculated; otherwise false.
		*/
		bool calculateMotorAngleByDetectPointer(mpj_Angle& angle, const mpj_Image* frame = nullptr)
		{
			if (pointDetector)
			{
				std::shared_ptr<const mpj_Image> tmp;
				if (!frame && camera)
				{
					tmp = camera->captureFrameWait();
					frame = tmp.get();
				}
				mpj_Angle motorAngle;
				if (frame && detectPointFromFrame(frame, motorAngle) && pointerAngleToMotorAngle(motorAngle, angle))
					return true;
			}
			return false;
		}

		//! Detect plane according to angle.
		/*!
		\param planeInfo Detected plane information.
		\return true if plane is detected; otherwise false.
		If no plane is detected, planeInfo is not updated.
		*/
		bool updateResultIfPlaneDetected(mpj_PlaneDetectionResult& result) const
		{
			return updateResultIfPlaneDetected(motorManager->getCurrentAngle(), result);
		}

		//! Detect plane according to angle.
		/*!
		\param angle Angle.
		\param planeInfo Detected plane information.
		\return true if plane is detected; otherwise false.
		If no plane is detected, planeInfo is not updated.
		*/
		bool updateResultIfPlaneDetected(const mpj_Angle& angle, mpj_PlaneDetectionResult& result) const
		{
			mpj_PlaneDetectionResult tmp;
			tmp.angle = angle;
			if (detectPlane(mpj_Mat4::createFromAngle(angle), tmp.plane, tmp.q))
			{
				result = tmp;
				result.isValid = true;
				return true;
			}
			return false;
		}

		//! Determine whether point detection is enabled or not.
		bool isPointDetectEnabled() const { return camera && pointDetector; }

		//! Get current motor angle.
		mpj_Angle getMotorAngle() const { return motorManager->getCurrentAngle(); }

		//! Get underlying motor manager.
		std::shared_ptr<mpj_MotorManager> getMotorManager() { return motorManager; }
		//! Get underlying motor manager.
		std::shared_ptr<const mpj_MotorManager> getMotorManager() const { return motorManager; }
		//! Get underlying fish-eye camera.
		std::shared_ptr<mpj_Camera> getFishEyeCamera() { return camera; }
		//! Get underlying fish-eye camera.
		std::shared_ptr<const mpj_Camera> getFishEyeCamera() const { return camera; }
		//! Get underlying geodesicDome.
		std::shared_ptr<const mpj_GeodesicDome> getGeodesicDome() const { return geodesicDome; }

		//! Calculate projector vectors for the specified rotation angle.
		/*!
		\param rotation Rotation angle.
		\param nearDist Near Z coordinate.
		\param multiplier Multiplier for the four projector corners.
		\return Rotated projector vectors.
		For the definition for projector vector and the term "near", see \ref about_intrinsic.
		\note Unlike \ref calculateProjectorVectors, the projector directions calculated by the function is rotated after tranlation by \ref mpj_EasyParams::projTranslate.
		\sa \ref calculateProjectorVectors.
		*/
		mpj_EasyProjectorDirections calculateProjectorVectorsRotated(const mpj_Angle& rotation, float nearDist = 1.f, float multiplier = 1.f) const
		{
			return calculateProjectorVectorsRotated(mpj_Mat4::createFromAngle(rotation), nearDist, multiplier);
		}

		//! Calculate projector vectors for the specified rotation matrix.
		/*!
		\param rotation Rotation matrix.
		\param nearDist Near Z coordinate.
		\param multiplier Multiplier for the four projector corners.
		\return Rotated projector vectors.
		For the definition for projector vector and the term "near", see \ref about_intrinsic.
		\note Unlike \ref calculateProjectorVectors, the projector directions calculated by the function is rotated after tranlation by \ref mpj_EasyParams::projTranslate.
		\sa \ref calculateProjectorVectors.
		*/
		mpj_EasyProjectorDirections calculateProjectorVectorsRotated(const mpj_Mat4& rotation, float nearDist = 1.f, float multiplier = 1.f) const
		{
			auto dirs = calculateProjectorVectors(nearDist, multiplier);

			// FIXME: the code here assumes mpj_EasyProjectorDirections's member order and packing.
			auto arr = &dirs.leftTopDirection;
			for (size_t i = 0; i < 6; i++)
				arr[i] += params.projTranslate;

			for (size_t i = 0; i < 8; i++)
				arr[i] = rotation * arr[i];
			return dirs;
		}

		//! Calculate projector's frustum.
		/*!
		The function calculates glFrustum compatible values from projector's intrinsic matrix (\ref mpj_EasyParams::projIntrinsic).
		\param left Receives left coordinate.
		\param right Receives right coordinate.
		\param bottom Receives bottom coordinate.
		\param top Receives top coordinate.
		\param nearDist Near Z coordinate.
		For the definition for the projetor "frustum" and "near", see \ref about_intrinsic.
		\note Unlike other functions in the SDK, the parameter order is left, right, bottom, top; it's identical order to OpenGL's [glFrustum](https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glFrustum.xml) function.
		*/
		void calculateProjectorFrustum(float& left, float& right, float& bottom, float& top, float nearDist = 1.f) const
		{
			auto fx = params.projIntrinsic.m[0];
			auto fy = params.projIntrinsic.m[5];
			auto cx = params.projIntrinsic.m[2];
			auto cy = params.projIntrinsic.m[6];

			left = -cx * nearDist / fx;
			top = (params.projectorHeight - cy) * nearDist / fy;
			right = (params.projectorWidth - cx) * nearDist / fx;
			bottom = -cy * nearDist / fy;
		}

		//! Calculate default projector vectors.
		/*!
		\param multiplier Multiplier for the four projector corners.
		\return Projector vectors.
		For the definition for projector vector and the term "near", see \ref about_intrinsic.
		\sa \ref calculateProjectorVectorsRotated.
		*/
		mpj_EasyProjectorDirections calculateProjectorVectors(float nearDist = 1.f, float multiplier = 1.f) const
		{
			mpj_EasyProjectorDirections dirs;

			auto fx = params.projIntrinsic.m[0];
			auto fy = params.projIntrinsic.m[5];
			auto cx = params.projIntrinsic.m[2];
			auto cy = params.projIntrinsic.m[6];

			float left, top, right, bottom;
			calculateProjectorFrustum(left, right, bottom, top, nearDist);

			dirs.leftTopDirection = mpj_Vec3(left, top, nearDist).normalize() * multiplier;
			dirs.rightTopDirection = mpj_Vec3(right, top, nearDist).normalize() * multiplier;
			dirs.rightBottomDirection = mpj_Vec3(right, bottom, nearDist).normalize() * multiplier;
			dirs.leftBottomDirection = mpj_Vec3(left, bottom, nearDist).normalize() * multiplier;
			dirs.rayCenterDirection = mpj_Vec3(dirs.leftTopDirection + dirs.rightTopDirection + dirs.rightBottomDirection + dirs.leftBottomDirection).normalize() * multiplier;
			dirs.cameraCenterPosition = mpj_Vec3(0, 0, 0);

			dirs.cameraLookAtDirection = mpj_Vec3(0, 0, 1);
			dirs.cameraUpDirection = mpj_Vec3(0, 1, 0);
			return dirs;
		}

		//! Update point detect mask with the current frame.
		/*!
		If the point detection is not yet enabled, it is implicitly enabled.
		*/
		void updatePointDetectMaskWithCurrentCameraFrame()
		{
			updatePointDetectMaskWithFrame(camera->captureFrameWait().get());
		}

		//! Update point detect mask with the specified frame.
		/*!
		\param frame Frame to use as a mask.
		If the point detection is not yet enabled, it is implicitly enabled.
		*/
		void updatePointDetectMaskWithFrame(const mpj_Image* frame)
		{
			enablePointDetect(true);
			pointDetector->setFrameMask(frame, params.pointDetectBinThrehold);
		}

		//! Enable/disable point detect.
		/*!
		\param enable true to enable point detect; otherwise, false to disable it.
		*/
		void enablePointDetect(bool enable)
		{
			if (isPointDetectEnabled() == enable)
				return;
			if (enable)
			{
				reloadFishEyeCamera(true);
				reloadPointDetector(true);

				enableMotorTorque(true);
			}
			else
			{
				enablePointDetectPreview(false);
				enableMotorTorque(false);
				pointDetector = nullptr;
				camera = nullptr;
			}
		}

		//! Enable/disable point detection preview.
		/*!
		\param enable true to enable point detection preview; otherwise, false to disable it.
		*/
		bool enablePointDetectPreview(bool enable)
		{
			if (isPointDetectPreviewEnabled() == enable)
				return true;
			if (enable)
			{
				if (isPointDetectEnabled())
					previewWindow = mpj_ImagePreviewWindow::create("point-detect preview");
			}
			else
			{
				previewWindow = nullptr;
			}
			return previewWindow ? true : false;
		}

		//! Determine whether point detection preview is enabled or not.
		bool isPointDetectPreviewEnabled() const
		{
			return isPointDetectEnabled() && previewWindow;
		}

		//! Enable motor torque.
		void enableMotorTorque(bool f)
		{
			motorManager->getMotor()->setMotorTorqueEnabled(mpj_MotorSpec::Pan, f);
			motorManager->getMotor()->setMotorTorqueEnabled(mpj_MotorSpec::Tilt, f);
		}

		//! Reset motor torque to the configurated state.
		void revertMotorTorque()
		{
			enableMotorTorque(params.pointDetectEnabled);
		}

		//! Reload \ref MotorManager.
		void reloadMontorManager()
		{
			if (motorManager)
				motorManager->setGoalAngle(mpj_Angle::zero()); // for safety
			motorManager = mpj_MotorManager::create(
				params.motorModel.c_str(), params.serialPort.c_str(), params.motorParams);
		}

		//! Apply \ref json_motor "motor related parameters" to the actual motor.
		void applyMotorParams()
		{
			motorManager->applyMotorParams(params.motorParams);
		}

		//! Reload fish-eye camera.
		/*!
		\param forceLoad Normally, if \ref mpj_EasyParams::pointDetectEnabled is false,
		the method does nothing. But if you set \p forceLoad to true, the method tries to activate
		the camera.
		*/
		void reloadFishEyeCamera(bool forceLoad = false)
		{
			if (!forceLoad && !params.pointDetectEnabled)
				return;

			camera = mpj_Camera::create(params.cameraModel);
			applyFishEyeCameraParameters();
			camera->startCapture();
		}

		//! Apply \ref json_fisheyecamera "fish-eye camera related parameters" to the actual camera.
		/*!
		\param forceLoad Normally, if \ref mpj_EasyParams::pointDetectEnabled is false,
		the method does nothing. But if you set \p forceLoad to true, the method tries to activate
		the camera.
		*/
		void applyFishEyeCameraParameters(bool forceLoad = false)
		{
			if (!camera)
			{
				if (!forceLoad)
					return;
				reloadFishEyeCamera(true);
			}

			auto cc = camera->getConfigManager();
			auto supported = cc->getSupportedConfigs();
			if (supported & mpj_CameraConfigManager::c_Gain)
				cc->setGain(params.cameraGain);
			if (supported & mpj_CameraConfigManager::c_Exposure)
				cc->setExposure(params.cameraExposure);
			if (supported & mpj_CameraConfigManager::c_PixelClock)
				cc->setPixelClock(params.cameraPixelClock);
		}

		//! Reload point detector.
		/*!
		\param forceLoad Normally, if \ref mpj_EasyParams::pointDetectEnabled is false,
		the method does nothing. But if you set \p forceLoad to true, the method tries to activate
		the camera.
		*/
		void reloadPointDetector(bool forceLoad = false)
		{
			if (!forceLoad && !params.pointDetectEnabled)
				return;

			pointDetector = mpj_PointDetector::create();
			if (_FSPFX exists(params.getFullPathNameFor(params.pointDetectFrameMask)))
			{
				pointDetector->setFrameMask(params.getFullPathNameFor(params.pointDetectFrameMask), params.pointDetectBinThrehold);
			}
			else
			{
				if (!camera)
					applyFishEyeCameraParameters(true);

				pointDetector->setFrameMaskFromDefaultAngles(camera->getWidth(), camera->getHeight(), pointTransform);
			}

			pointDetector->setZeta(params.pointDetectZeta);
		}

		//! Apply \ref json_pointdetect "point detector related parameters" to the actual point detector.
		void applyPointDetectorParams()
		{
			if (pointDetector)
			{
				pointDetector->setZeta(params.pointDetectZeta);
			}
		}

		//! Reload point-transform parameters (\ref mpj_Transform2d).
		void reloadPointTransform()
		{
			pointTransform = mpj_Transform2d::createFromIIFFCsvTable(params.getFullPathNameFor(params.pointTransformCsvFileName));
		}

		//! Get the current parameters.
		const mpj_EasyParams& getParams() const { return params; }

		//! Get access to the parameters.
		/*!
		Please note that the modifications to the parameters are not immediately reflected to the underlying system.
		To reflect the changes, use \ref applyMotorParams, \ref applyFishEyeCameraParameters according to the changed parameter categories.
		For more information, see \ref easyjson.
		*/
		mpj_EasyParams& getParams() { return params; }

	private:
		mpj_Easy(const mpj_Easy&) = delete;
		mpj_Easy& operator=(const mpj_Easy&) = delete;

		constexpr static const float AngleDegreeFloorThreshold = 20.f;
		constexpr static const float AngleDegreeCeilingThreshold = 160.f;

		mpj_EasyParams params;

		std::shared_ptr<mpj_MotorManager> motorManager;
		std::shared_ptr<mpj_Camera> camera;
		std::shared_ptr<mpj_PointDetector> pointDetector;
		std::shared_ptr<mpj_GeodesicDome> geodesicDome;
		std::shared_ptr<mpj_Transform2d> pointTransform;
		std::shared_ptr<mpj_ImagePreviewWindow> previewWindow;

		void setup()
		{
			reloadPointTransform();

			reloadMontorManager();

			enablePointDetect(params.pointDetectEnabled);

			if (_FSPFX exists(params.getFullPathNameFor(params.geodesicDomePcdFileName)))
			{
				if (!loadGeodesicDome(params.getFullPathNameFor(params.geodesicDomePcdFileName)))
					throw mpj_FileIoException(_LINE_AND_FILE params.getFullPathNameFor(params.geodesicDomePcdFileName), mpj_Error::IoError, "Could not load the specified PCD file.");
			}

			revertMotorTorque();
		}

		const mpj_Angle* detectPointFromFrame(const mpj_Image* frame, mpj_Angle& angle) const
		{
			if (!pointDetector)
				return nullptr;
			if (!frame)
				return nullptr;
			auto points = pointDetector->detectPointsOnFrame(frame, params.pointDetectBinThrehold,
				[this](auto frame, auto width, auto height, auto stride)
				{
					if (isPointDetectPreviewEnabled())
					{
						auto fmameImage = mpj_Image::create(frame, static_cast<int>(stride), width, height, mpj_PixelFormat::Gray8);
						previewWindow->sendImage(fmameImage.get());
					}
				});
			if (points.size())
			{
				auto pt = points[0];
				float pan, tilt;
				if (pointTransform->transform(pt.x, pt.y, pan, tilt))
				{
					angle = mpj_Angle(pan, tilt);
					return &angle;
				}
			}
			return nullptr;
		}

		bool pointerAngleToMotorAngle(const mpj_Angle& pointAngle, mpj_Angle& angle) const
		{
			if (!geodesicDome)
				return false;

			auto rot = mpj_Mat4::createFromQuaternion(mpj_Quaternion(params.fishEyeRotate.tilt, mpj_Vec3(1.f, 0.f, 0.f)));
			auto dst = rot * mpj_Vec3::createUnitVectorFromPoint(pointAngle);

			const float AngleThreshold = 10.f;
			float minAngle;
			mpj_Vec3 minPoint;
			if (geodesicDome->findMostClosestDistPoint(params.fishEyeTranslate, dst, minAngle, minPoint) && minAngle < AngleThreshold)
			{
				angle = minPoint.toAngle();
				computeProjectorDriveAngle(minPoint, angle);
				return true;
			}
			return false;
		}

		const mpj_Vec3 getProjectorAxisVector() const
		{
			return calculateProjectorVectors(1.f).rayCenterDirection;
		}

		bool detectPlane(const mpj_Mat4& rotation, mpj_PlaneInfo& outPlaneInfo, mpj_Quaternion& q) const
		{
			if (!geodesicDome.get())
				return false;

			mpj_PlaneInfo plane;
			if (!geodesicDome->detectPlane(
				plane,
				(params.projectorWidth - params.planeDetectSize) / 2,
				(params.projectorHeight - params.planeDetectSize) / 2,
				params.planeDetectSize,
				params.planeDetectSize,
				rotation,
				params.projIntrinsic,
				params.projTranslate))
			{
				return false;
			}

			const auto yAxis = mpj_Vec3(0, 1.f, 0);
			const auto zAxis = mpj_Vec3(0, 0, 1.f);
			auto angleBetweenYaxis = yAxis.angleDegree(plane.normal);

			mpj_Vec3 rotYaxis;
			if (angleBetweenYaxis < AngleDegreeFloorThreshold) // floor
			{
				auto refVec = rotation * zAxis;
				refVec.y = 0.f;
				refVec = refVec.normalize();
				refVec *= -1;
				rotYaxis = plane.normal.getCrossingVector(plane.normal.getCrossingVector(refVec));
			}
			else if (angleBetweenYaxis > AngleDegreeCeilingThreshold) // ceiling
			{
				auto refVec = rotation * zAxis;
				refVec.y = 0.f;
				refVec = refVec.normalize();
				rotYaxis = plane.normal.getCrossingVector(plane.normal.getCrossingVector(refVec));
			}
			else // side wall
			{
				auto refVec = mpj_Vec3(0, -1.f, 0);
				rotYaxis = plane.normal.getCrossingVector(plane.normal.getCrossingVector(refVec));
				rotYaxis.y = std::abs(rotYaxis.y);
			}

			// changed the center position to projector image center
			auto x0 = rotation * params.projTranslate;
			auto p = (rotation * (getProjectorAxisVector() + params.projTranslate) - x0).normalize();
			auto rate = (plane.normal.x * (x0.x - plane.center.x) + plane.normal.y * (x0.y - plane.center.y) + plane.normal.z * (x0.z - plane.center.z)) / (plane.normal.x * p.x + plane.normal.y * p.y + plane.normal.z * p.z);
			plane.center = x0 - p * rate;

			auto length = plane.center.distance(x0);
			auto widthLength = length * params.projIntrinsic.m[2] / params.projIntrinsic.m[0];
			plane.pixelsPerMillimeter = params.projectorWidth / widthLength;

			//
			// output
			//
			outPlaneInfo = plane;

			auto rotZaxis = -plane.normal;
			auto rotXaxis = rotYaxis.getCrossingVector(rotZaxis);
			q = mpj_Mat4(
				rotXaxis.x, rotYaxis.x, rotZaxis.x, 0.f,
				rotXaxis.y, rotYaxis.y, rotZaxis.y, 0.f,
				rotXaxis.z, rotYaxis.z, rotZaxis.z, 0.f,
				0.f, 0.f, 0.f, 1.f).getRotate();

			return true;
		}

		void computeProjectorDriveAngle(const mpj_Vec3& dstPoint, mpj_Angle& dstAngle) const
		{
			//dst angle
			auto tmp3DPJDir = getProjectorAxisVector().normalize();

			//compute pan angle(xz plane)
			{
				mpj_Vec2 tarPoint(dstPoint.x, dstPoint.z);
				auto tarPointLength = tarPoint.length();
				//PJ local position and direction
				auto pjPos = mpj_Vec2(params.projTranslate.x, params.projTranslate.z);
				auto pjDir = mpj_Vec2(tmp3DPJDir.x, tmp3DPJDir.z).normalize();

				auto b = pjPos.dotProduct(pjDir);
				auto c = pjPos.length() * pjPos.length() - tarPointLength * tarPointLength;
				auto sqrtVal = std::sqrt(b * b - c);
				auto pjDirRate = -b + sqrtVal;
				auto tarPointAlongPjDir = pjPos + pjDir *  pjDirRate;

				dstAngle.pan = tarPointAlongPjDir.angleDegree(tarPoint);
			}

			//compute tilt angle (xyz space)
			{
				auto virtualQuat = mpj_Quaternion(0.f, mpj_Vec3(1.f, 0.f, 0.f), -1 * dstAngle.pan, mpj_Vec3(0.f, 1.f, 0.f), 0.f, mpj_Vec3(0.f, 0.f, 1.f));

				//projector tilt rotation center
				auto pjPanRotateCent = virtualQuat * mpj_Vec3(params.projTranslate.x, 0.f, 0.f);
				//dst point from pj tilt rotation center, and length
				auto dstPointFromPjRotateCent = dstPoint - pjPanRotateCent;
				auto tarPointLength = dstPointFromPjRotateCent.length();

				//pj rotated pan position and direction 
				auto pjPanRotatePos = virtualQuat * params.projTranslate;
				auto pjPanRotateDir = (virtualQuat * tmp3DPJDir).normalize();
				//pj offset from tilt rotation center
				auto pjPanRotateFromCent = pjPanRotatePos - pjPanRotateCent;

				auto b = pjPanRotateFromCent.dotProduct(pjPanRotateDir);
				auto c = pjPanRotateFromCent.length() * pjPanRotateFromCent.length() - tarPointLength * tarPointLength;
				auto sqrtVal = std::sqrt(b * b - c);
				auto pjDirRate = -b + sqrtVal;
				// target position along to pjDir and same length of target point length
				auto tarPointAlongPjPanRotateDir = pjPanRotatePos + pjDirRate * pjPanRotateDir;
				auto tarPointAlongPjPanRotateDirFromPjRotateCent = tarPointAlongPjPanRotateDir - pjPanRotateCent;
				//double  testDist = (tarPPJDir-pjRotateCent).length();
				auto angleP = tarPointAlongPjPanRotateDirFromPjRotateCent.angleDegree(dstPointFromPjRotateCent);
				if (dstPointFromPjRotateCent.normalize().y < tarPointAlongPjPanRotateDirFromPjRotateCent.normalize().y)
				{
					dstAngle.tilt = -angleP;
				}
				else
				{
					dstAngle.tilt = angleP;
				}
				dstAngle.pan *= -1.f;
			}
		}
	};
}

#endif // _mpj_easy_h_
