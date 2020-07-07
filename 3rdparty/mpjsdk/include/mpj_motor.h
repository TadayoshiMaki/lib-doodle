//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_motor.h
//! \brief Motor related definitions
//---------------------------------------------------------------------------

#ifndef _mpj_motor_h_
#define _mpj_motor_h_

#include <map>
#include <memory>
#include <filesystem>
#include <algorithm>
#include <cmath>

#include "mpj_common.h"
#include "mpj_error.h"
#include "mpj_angle.h"

namespace mpjsdk
{
	//! Motor rotation direction.
	enum class mpj_MotorDirection
	{
		Positive = 0, //!< Motor rotates to positive direction.
		Negative = 1 //!< Motor rotates to negative direction.
	};

	//! Motor constants.
	/*!
		The following fragment illustrates how to load motor constants for a specific motor:
		\code{.cpp}
		auto& consts = mpj_MotorConstants::getConstantsFor(mpj_KnownMotors::DXL_MX28T_DPJ_F);
		\endcode

		\sa \ref mpj_Motor::getConstants, \ref mpj_KnownMotors
	*/
	struct mpj_MotorConstants
	{
		//! Motor type name.
		const char* const name;
		//! Maximum value for discrete motor position.
		uint32_t maxPosition;
		//! Maximum angle in digrees.
		float maxAngle;
		//! Maximum motor speed in discrete motor units.
		/*!
		\see rpmPerSpeed for the speed definition.
		*/
		uint32_t maxSpeed;
		//! Maximum RPM (rotation-per-minute).
		float maxRpm;
		//! Motor speed RPM ratio.
		/*!
			Actual rpm is calculated by the following formula:
			\code{.cpp}
			rpm = speed * rpmPerSpeed.
			\endcode
		*/
		float rpmPerSpeed;
		//! Maximum value for P-gain.
		uint32_t maxPGain;
		//! Maximum value for I-gain.
		uint32_t maxIGain;
		//! Maximum value for D-gain.
		uint32_t maxDGain;
		//! Maximum value for Acceleration/Deceleration.
		uint32_t maxAccDec;
		//! Motor direction.
		mpj_MotorDirection motorDirection;
		//! Each offset for each motor axis; 0-th for pan and 1-st for tilt.
		float angleOffsets[2];

		//! Constrcutor to ease initialization of the motor constants.
		/*!
			\param name Motor type name.
			\param maxPosition Maximum value for discrete motor position.
			\param maxAngle Maximum angle in digrees.
			\param maxSpeed Maximum motor speed in discrete motor units.
			\param maxRpm Maximum RPM (rotation-per-minute).
			\param rpmPerSpeed Motor speed RPM ratio.
			\param maxPGain Maximum value for P-gain.
			\param maxIGain Maximum value for I-gain.
			\param maxDGain Maximum value for D-gain.
			\param maxAccDec Maximum value for Acceleration/Deceleration.
			\param mpj_MotorDirection Motor direction.
			\param panOffset Offset angle for Pan.
			\param tiltOffset Offset angle for Tilt.
		*/
		mpj_MotorConstants(
			const char* name, uint32_t maxPosition, float maxAngle, uint32_t maxSpeed, float maxRpm, float rpmPerSpeed,
			uint32_t maxPGain, uint32_t maxIGain, uint32_t maxDGain, uint32_t maxAccDec,
			mpj_MotorDirection mpj_MotorDirection, float panOffset = 0, float tiltOffset = 0) noexcept :
			name(name), maxPosition(maxPosition), maxAngle(maxAngle), maxSpeed(maxSpeed), maxRpm(maxRpm), rpmPerSpeed(rpmPerSpeed),
				maxPGain(maxPGain), maxIGain(maxIGain), maxDGain(maxDGain), maxAccDec(maxAccDec), motorDirection(mpj_MotorDirection)
		{
			angleOffsets[0] = panOffset;
			angleOffsets[1] = tiltOffset;
		}
		
		//! Load constants for the specific motor model.
		/*!
			\param name Motor model name. For the list of known motor models, see \ref mpj_KnownMotors.
		*/
		static const mpj_MotorConstants& getConstantsFor(const std::string& name);
	};

	//! Motor-position Angle Converter.
	/*!
		The following fragment illustrates how to calculate motor position from angle:
		\code{.cpp}
		mpj_AnglePosConverter conv(mpj_MotorConstants::getConstantsFor(mpj_KnownMotors::DXL_MX28T_DPJ_F));

		mpj_Angle angle(-90, 30); // Pan=-90, Tilt=30

		auto panPosition = conv.posFromAngle(angle, MotorSpec::Pan);
		auto tiltPosition = conv.posFromAngle(angle, MotorSpec::Tilt);
		\endcode

		\sa \ref mpj_Angle, \ref mpj_Motor::getConstants,\ref  mpj_KnownMotors
	*/
	class mpj_AnglePosConverter
	{
	public:
		//! Initialize the instance with \ref mpj_MotorConstants.
		/*!
			\param constants
				Motor constants.
				Please note that the constants must be valid during the lifetime of the instance.
		*/
		explicit mpj_AnglePosConverter(const mpj_MotorConstants& constants) : constants(constants)
		{
		}

		//! Set additional offset for pan/tilt angles.
		/*!
		\param offset Offset for pan/tilt angles.
		\sa \ref mpj_MotorConstants::angleOffsets
		*/
		void setAdditionalAngleOffset(const mpj_Angle& offset)
		{
			angleOffsets = offset;
		}

		//! Get additional offset for pan/tilt angles.
		/*!
		\return Offset for pan/tilt angles.
		\sa \ref mpj_MotorConstants::angleOffsets
		*/
		const mpj_Angle& getAdditionalAngleOffset()
		{
			return angleOffsets;
		}

		//! Convert motor position to angle.
		/*!
			\param pan
				Pan motor position.
			\param tilt
				Tilt motor position.
			\return
				Angle [-180 180] corresponding to the motor position.
		*/
		mpj_Angle angleFromPos(uint32_t pan, uint32_t tilt) const
		{
			return mpj_Angle(angleFromPos(pan, mpj_MotorSpec::Pan), angleFromPos(tilt, mpj_MotorSpec::Tilt));
		}

		//! Convert motor position (1-axis) to angle.
		/*!
			\param pos
				Motor position.
			\param spec
				Motor position's axis.
			\return
				Angle [-180 180] corresponding to the motor position.
		*/
		float angleFromPos(uint32_t pos, mpj_MotorSpec spec) const
		{
			auto p = constants.maxAngle * pos / constants.maxPosition; // [0 1024] -> [0 360]
			p -= constants.angleOffsets[static_cast<uint32_t>(spec)] + getOffsetFor(spec);
			p -= 180.f; // [0 360] -> [-180 180]
			return constants.motorDirection == mpj_MotorDirection::Positive ? p : -p;
		}

		//! Convert either one of pan/tilt angles to motor position.
		/*!
			\param angle
				Angle [-180 180].
			\param spec
				Motor position's axis.
			\param saturation
				true to cut-off out of range values.
			\return
				Motor position corresponding to the angle.
		*/
		uint32_t posFromAngle(const mpj_Angle& angle, mpj_MotorSpec spec, bool saturation = true) const
		{
			auto p = angle.valueBySpec(spec); // [-180 180]
			if (constants.motorDirection == mpj_MotorDirection::Negative)
				p = -p;
			p += constants.angleOffsets[static_cast<uint32_t>(spec)] + getOffsetFor(spec); // minor offsets
			p += 180.f; // [-180 180] -> [0 360]
			if (saturation)
			{
				if (p < 0.f) p = 0.f;
				else if (p > 360.f) p = 360.f;
			}
			return static_cast<uint32_t>(p / constants.maxAngle * constants.maxPosition); // [0 360] -> [0 1024]
		}

	private:
		// NOTE: the member is by the reference.
		const mpj_MotorConstants& constants;
		mpj_Angle angleOffsets;

		float getOffsetFor(mpj_MotorSpec spec) const
		{
			return spec == mpj_MotorSpec::Pan ? angleOffsets.pan : angleOffsets.tilt;
		}
	};

	//! Known motor models to load specific motor or motor related information.
	/*!
	There are also dynamically registered motors and you can enumerate all the available
	motor models by \ref mpj_MotorRegistry::getRegistry.
	\sa \ref mpj_MotorConstants::getConstantsFor, \ref mpj_Motor::getConstants, \ref mpj_MotorManager::create, \ref mpj_MotorRegistry
	*/
	class mpj_KnownMotors
	{
	public:
		//! <a href="http://www.robotis.us/dynamixel-mx-28t/">Dynamixel MX-28T</a>
		static constexpr char const DXL_MX28T[] = "DXL_MX28T";
		//! <a href="http://www.robotis.us/dynamixel-mx-28t/">Dynamixel MX-28T</a>
		static constexpr char const  DXL_MX28T_DPJ_F[] = "DXL_MX28T_DPJ_F";
		//! <a href="http://www.robotis.us/dynamixel-mx-64ar/">Dynamixel MX-64AR</a>
		static constexpr char const  DXL_MX64AR[] = "DXL_MX64AR";
		//! <a href="http://www.robotis.us/dynamixel-rx-24f-hn07-n101/">Dynamixel RX-24F</a>
		static constexpr char const  DXL_RX24F[] = "DXL_RX24F";
		//! <a href="http://www.robotis.us/ax-12a/">Dynamixel AX-12A</a>
		static constexpr char const  DXL_AX12A[] = "DXL_AX12A";
		//! A virtual motor, which is emulated completely by the software.
		static constexpr char const  Virtual[] = "Virtual";
		//! Keyboard controled virtual motor.
		static constexpr char const  Keyboard[] = "Keyboard";
	private:
		mpj_KnownMotors() = delete;
		mpj_KnownMotors(const mpj_KnownMotors&) = delete;
		mpj_KnownMotors& operator=(const mpj_KnownMotors&) = delete;
	};

	//! Single motor initialization parameters.
	/*!
		\sa \ref mpj_MotorParams
	*/
	struct mpj_MotorControlParams
	{
		//! Initialize parameters with the default values.
		mpj_MotorControlParams() :
			maxRpm(50),
			accDec(254),
			pGain(32),
			iGain(0),
			dGain(0)
		{
		}

		//! Maximum rpm (rotation-per-minute).
		float maxRpm;
		//! Acceleration/Deceleration value.
		int accDec;
		//! P-gain
		int pGain;
		//! I-gain
		int iGain;
		//! D-gain
		int dGain;

		bool operator==(const mpj_MotorControlParams& r) const
		{
			return
				maxRpm == r.maxRpm &&
				accDec == r.accDec &&
				pGain == r.pGain &&
				iGain == r.iGain &&
				dGain == r.dGain;
		}

		bool operator!=(const mpj_MotorControlParams& r) const
		{
			return !operator==(r);
		}

		//! Verify parameters.
		/*!
			\param consts Motor constants to which the parameters are verified against.
			\return true if all the parameters are in valid range; otherwise false.
		*/
		bool verifyParams(const mpj_MotorConstants& consts) const
		{
			return
				ok<float>(maxRpm, consts.maxRpm) &&
				ok<int>(accDec, consts.maxAccDec) &&
				ok<int>(pGain, consts.maxPGain) &&
				ok<int>(iGain, consts.maxIGain) &&
				ok<int>(dGain, consts.maxDGain);
		}

		//! Update parameters if they are out of range.
		/*!
			\param consts Motor constants to which the parameters are verified against.
		*/
		void updateParamsIfInvalid(const mpj_MotorConstants& consts)
		{
			maxRpm = sat<float>(maxRpm, consts.maxRpm);
			accDec = sat<int>(accDec, consts.maxAccDec);
			pGain = sat<int>(pGain, consts.maxPGain);
			iGain = sat<int>(iGain, consts.maxIGain);
			dGain = sat<int>(dGain, consts.maxDGain);
		}

		//! Throw exception if some of the parameters are out of range.
		/*!
			\param consts Motor constants to which the parameters are verified against.
		*/
		void throwIfInvalidParams(const mpj_MotorConstants& consts) const
		{
			if (!verifyParams(consts))
				throw mpj_InvalidParamException(_LINE_AND_FILE "Invalid motor control parameters.");
		}

	private:
		template<typename U, typename V = U> static bool ok(U v, V max) { return v >= static_cast<U>(0) && v <= static_cast<U>(max); }
		template<typename U, typename V = U> static U sat(U v, V max)
		{
			if (v < 0) return 0;
			if (v > static_cast<U>(max)) return static_cast<U>(max);
			return v;
		}
	};

	//! Motor initialization parameters used with \ref mpj_MotorManager::create function.
	/*!
		\code{.cpp}
		// load the motor configuration parameters as they are on the file
		auto params_raw = mpj_MotorParams::createFromJsonFile("C:\\somewhere\\motor.json");

		// load the motor configuration parameters and apply motor specific restriction
		auto& consts = mpj_MotorConstants::getConstantsFor(mpj_KnownMotors::DXL_MX28T_DPJ_F);
		auto params = mpj_MotorParams::createFromJsonFile("C:\\somewhere\\motor.json", consts);

		// of course, you can write back the parameters to the file again
		params.saveToJsonFile("C:\\somewhere\\motor-updated.json");
		\endcode

		\sa \ref mpj_MotorControlParams, \ref mpj_MotorConstants
	*/
	struct mpj_MotorParams
	{
	public:
		//! There are 2 servo motors, one for pan and another for tilt.
		/*!
			\sa \ref MotorSpec
		*/
		static const size_t SERVO_NUM = 2;

		//! Motor parameters.
		mpj_MotorControlParams params[SERVO_NUM];
		//! Convergence; the motor position differential threshold.
		float convergence;
		//! Valid range of pan angle.
		mpj_Range<float> panRange;
		//! Valid range of tilt angle.
		mpj_Range<float> tiltRange;
		//! Offsets for pan/tilt angle in degrees.
		mpj_Angle offset;

		//! Initialize parameters with the default values.
		mpj_MotorParams() :
			convergence(1.5),
			panRange(mpj_Angle::validPanRange),
			tiltRange(mpj_Angle::validTiltRange),
			offset(mpj_Angle(0, 0))
		{
		}

		bool operator==(const mpj_MotorParams& r) const
		{
			return
				params[0] == r.params[0] &&
				params[1] == r.params[1] &&
				convergence == r.convergence &&
				panRange == r.panRange &&
				tiltRange == r.tiltRange &&
				offset == r.offset;
		}

		bool operator!=(const mpj_MotorParams& r) const
		{
			return !operator==(r);
		}

		//! Verify parameters.
		/*!
			\return true if all the parameters are in valid range; otherwise false.
		*/
		bool verifyParams(const mpj_MotorConstants& consts) const
		{
			for (auto& p : params)
				if (!p.verifyParams(consts))
					return false;
			return convergence >= 0 &&
				panRange.isSubrangeOfOrEqualTo(mpj_Angle::validPanRange) &&
				tiltRange.isSubrangeOfOrEqualTo(mpj_Angle::validTiltRange);
		}

		//! Update parameters if they are out of range.
		/*!
			\param consts Motor constants to which the parameters are verified against.
		*/
		void updateParamsIfInvalid(const mpj_MotorConstants& consts)
		{
			convergence = std::max<float>(convergence, 0);
			panRange = mpj_Angle::validPanRange.saturateRange(panRange);
			tiltRange = mpj_Angle::validPanRange.saturateRange(tiltRange);
			offset.updateParamsIfInvalid();
		}

		//! Throw exception if some of the parameters are out of range.
		/*!
			\param consts Motor constants to which the parameters are verified against.
		*/
		void throwIfInvalidParams(const mpj_MotorConstants& consts) const
		{
			for (auto& p : params)
				p.throwIfInvalidParams(consts);
			if (!panRange.isSubrangeOfOrEqualTo(mpj_Angle::validPanRange))
				throw mpj_InvalidParamException(_LINE_AND_FILE "panRange is not valid.");
			if (!tiltRange.isSubrangeOfOrEqualTo(mpj_Angle::validTiltRange))
				throw mpj_InvalidParamException(_LINE_AND_FILE "tiltRange is not valid.");
			offset.throwIfInvalidParams();
		}

		//! Load parameters from the specified JSON file.
		/*!
			\param jsonFileName JSON file name.
		*/
		static mpj_MotorParams createFromJsonFile(const _FSPFX path & jsonFileName);

		//! Load parameters from the specified JSON file.
		/*!
			\param jsonFileName JSON file name.
			\param consts Constants, which define parameter maximum values.
		*/
		static mpj_MotorParams createFromJsonFile(const _FSPFX path & jsonFileName, const mpj_MotorConstants& consts);

		//! Load parameters from the specified JSON string.
		/*!
			\param json JSON string.
		*/
		static mpj_MotorParams deserializeFromJson(const std::string& json);

		//! Load parameters from the specified JSON string.
		/*!
			\param json JSON string.
			\param consts Constants, which define parameter maximum values.
		*/
		static mpj_MotorParams deserializeFromJson(const std::string& json, const mpj_MotorConstants& consts);

		//! Save parameters to the specified JSON file.
		/*!
			\param jsonFileName JSON file name.
		*/
		void saveToJsonFile(const _FSPFX path & jsonFileName) const;

		//! Convert parameters to JSON string.
		/*!
			\return JSON string.
		*/
		std::string serializeToJson() const;
	};

	//! Low-level access to the motor devices.
    /*!
		You had better use \ref mpj_MotorManager if you don't know much about the motor specific behaviors.

		\code{.cpp}
		// initializes DXL_MX28T_DPJ_F on COM2
		auto mm = mpj_Motor::create(mpj_KnownMotors::DXL_MX28T_DPJ_F, "COM2");
		\endcode

		\sa \ref mpj_MotorManager, \ref mpj_AnglePosConverter
    */
    class mpj_Motor
    {
    public:
		//! Create a motor instance.
        /*!
			\param modelName
				Motor model. mpj_KnownMotors for known motor models.
            \param comPort
                Port name. Or configuration string for certain motor.
        */
        static std::shared_ptr<mpj_Motor> create(const std::string& modelName, const std::string& comPort);

		//! Get motor related constants such as max-angle.
		/*!
			\return
				\ref mpj_MotorConstants contains motor specific constants.
		*/
		virtual const mpj_MotorConstants& getConstants() const = 0;

		//! Determine whether current motor torque is ON or not.
		/*!
			\param spec
				Select motor for pan or tilt.
			\return
				true if torque is enabled; otherwise false.
		*/
		virtual bool isMotorTorqueEnabled(mpj_MotorSpec spec) const = 0;

		//! Enable/disable motor torque.
		/*!
			\param spec
				Select motor for pan or tilt.
			\param enabled
				true to enable torque, false to disable it.
		*/
		virtual void setMotorTorqueEnabled(mpj_MotorSpec spec, bool enabled) = 0;

		//! Get current motor P-gain.
		/*!
			\param spec
				Select motor for pan or tilt.
			\return
				Motor's P-gain value.
		*/
		virtual uint32_t getMotorPGain(mpj_MotorSpec spec) const = 0;

		//! Set motor P-gain.
		/*!
			\param spec
				Select motor for pan or tilt.
			\param pGain
				Motor's P-gain value.
		*/
		virtual void setMotorPGain(mpj_MotorSpec spec, uint32_t pGain) = 0;

		//! Get current motor I-gain.
		/*!
			\param spec
				Select motor for pan or tilt.
			\return
				Motor's I-gain value.
		*/
		virtual uint32_t getMotorIGain(mpj_MotorSpec spec) const = 0;

		//! Set motor I-gain.
		/*!
			\param spec
				Select motor for pan or tilt.
			\param iGain
				Motor's I-gain value.
		*/
		virtual void setMotorIGain(mpj_MotorSpec spec, uint32_t iGain) = 0;

		//! Get current motor D-gain.
		/*!
			\param spec
				Select motor for pan or tilt.
			\return
				Motor's D-gain value.
		*/
		virtual uint32_t getMotorDGain(mpj_MotorSpec spec) const = 0;

		//! Set motor D-gain.
		/*!
			\param spec
				Select motor for pan or tilt.
			\param dGain
				Motor's D-gain value.
		*/
		virtual void setMotorDGain(mpj_MotorSpec spec, uint32_t dGain) = 0;

		//! Get motor Acceleration/Deceleration.
		/*!
			\param spec
				Select motor for pan or tilt.
			\return
				Motor's acc-dec value.
		*/
		virtual uint32_t getMotorAccDec(mpj_MotorSpec spec) const = 0;

		//! Set motor Acceleration/Deceleration.
		/*!
			\param spec
				Select motor for pan or tilt.
			\param accDec
				Motor's acc-dec value.
		*/
		virtual void setMotorAccDec(mpj_MotorSpec spec, uint32_t accDec) = 0;

		//! Get current motor temperature in celsius degrees.
		/*!
			\param spec
				Select motor for pan or tilt.
			\return
				Motor's temperature.
		*/
		virtual float getMotorTemperature(mpj_MotorSpec spec) const = 0;

		//! Get digital motor position.
		/*!
			\param spec
				Select motor for pan or tilt.
			\return
				Motor's position.
			\sa \ref getCurrentAngle
		*/
		virtual uint32_t getMotorPosition(mpj_MotorSpec spec) const = 0;

		//! Determine whether the motor is rotating or not.
		/*!
			\param spec
				Select motor for pan or tilt.
			\return
				true if motor is currently rotating; otherwise false.
		*/
		virtual bool isMotorRotating(mpj_MotorSpec spec) const = 0;

		//! Set digital motor goal positions/speeds at once.
		/*!
			\param panPosition Goal pan position.
			\param tiltPosition Goal tilt position.
			\param panSpeed Pan motor speed; 0 to stop. It must be equal to or smaller than mpj_MotorConstants::maxSpeed.
			\param tiltSpeed Tilt motor speed; 0 to stop.
			\sa \ref mpj_MotorManager::setGoalAngle, \ref mpj_MotorConstants
		*/
		virtual void setMotorGoalPosition(uint32_t panPosition, uint32_t tiltPosition, uint32_t panSpeed, uint32_t tiltSpeed) = 0;

		//! Set digital motor goal positions.
		/*!
			\param panPosition Goal pan position.
			\param tiltPosition Goal tilt position.
			\param params Parameters used to determine pan/tilt rotation speeds.
			\sa \ref mpj_MotorManager::setGoalAngle, \ref mpj_MotorParams, \ref mpj_MotorConstants
		*/
		void setMotorGoalPosition(uint32_t panPosition, uint32_t tiltPosition, const mpj_MotorParams& params)
		{
			auto& c = getConstants();
			auto panSpeedRatio = params.params[0].maxRpm / c.maxRpm;
			auto tiltSpeedRatio = params.params[1].maxRpm / c.maxRpm;
			if (panSpeedRatio < 0.f || panSpeedRatio > 1.f || tiltSpeedRatio < 0.f || tiltSpeedRatio > 1.f)
				throw mpj_InvalidParamException(_LINE_AND_FILE "mpj_MotorControlParams::maxRpm out of range: %f,%f", panSpeedRatio, tiltSpeedRatio);

			auto curPan = static_cast<double>(getMotorPosition(mpj_MotorSpec::Pan));
			auto curTilt = static_cast<double>(getMotorPosition(mpj_MotorSpec::Tilt));
			auto deltaPan = std::abs(panPosition - curPan);
			auto deltaTilt = std::abs(tiltPosition - curTilt);
			auto delta = std::sqrt(deltaPan * deltaPan + deltaTilt * deltaTilt);
			// NOTE: delta=0 to stop the move
			auto speedPan = delta ? static_cast<uint32_t>(deltaPan / delta * panSpeedRatio * c.maxSpeed) : 0;
			auto speedTilt = delta ? static_cast<uint32_t>(deltaTilt / delta * tiltSpeedRatio * c.maxSpeed) : 0;
			setMotorGoalPosition(panPosition, tiltPosition, speedPan, speedTilt);
		}

		virtual ~mpj_Motor() {}

	protected:
		mpj_Motor(const mpj_Motor&) = delete;
		mpj_Motor& operator=(const mpj_Motor&) = delete;
		mpj_Motor() {}
    };

	//! High-level control for motors.
	/*!
		\ref mpj_Motor for lower-level and motor specific control.

		To create an instance, use \ref mpj_MotorManager::create function.
		\code{.cpp}
		// initializes DXL_MX28T_DPJ_F on COM2
		auto mm = mpj_MotorManager::create(mpj_KnownMotors::DXL_MX28T_DPJ_F, "COM2");

		auto currentAngle = mm->getCurrentAngle();

		...
		\endcode

		Of course, you can load motor configuration from external JSON file:

		\code{.cpp}
		// initializes DXL_MX28T_DPJ_F on COM2 with the parameters loaded from a file
		auto mm = mpj_MotorManager::create(mpj_KnownMotors::DXL_MX28T_DPJ_F, "COM2", "c:\\somewhere\\motor.json");
		\endcode

		\sa \ref mpj_MotorParams
	*/
	class mpj_MotorManager
	{
	public:
		//! Create a new instance of mpj_MotorManager.
		/*!
			\param motorModel
				Motor model. \ref mpj_KnownMotors for known motor models.
			\param comPort
				COM port name.
			\sa \ref mpj_Motor, \ref mpj_MotorParams
		*/
		static std::shared_ptr<mpj_MotorManager> create(
			const char* motorModel,
			const char* comPort);

		//! Create a new instance of mpj_MotorManager.
		/*!
			\param motorModel
				Motor model. \ref mpj_KnownMotors for known motor models.
			\param comPort
				COM port name.
			\param motorParams
				Motor initialization parameters.
				It can be nullptr if you want to initialize
				the motor with the default parameters.
			\sa \ref mpj_Motor, \ref mpj_MotorParams
		*/
		static std::shared_ptr<mpj_MotorManager> create(
			const char* motorModel,
			const char* comPort,
			const mpj_MotorParams& motorParams);

		//! Creates a new instance of mpj_MotorManager by loading motor parameters from file.
		/*!
			\param motorModel
				Motor model. \ref mpj_KnownMotors for known motor models.
			\param comPort
				COM port name.
			\param motorParamsJsonFileName
				Motor parameters JSON file.
		*/
		static std::shared_ptr<mpj_MotorManager> create(
			const char* motorModel,
			const char* comPort,
			const _FSPFX path& motorParamsJsonFileName);

		//! Create a new instance of mpj_MotorManager.
		/*!
			\param motor
				Motor.
			\param motorParams
				Motor initialization parameters.
				It can be nullptr if you want to initialize
				the motor with the default parameters.
			\sa mpj_Motor, \ref mpj_MotorParams
		*/
		static std::shared_ptr<mpj_MotorManager> create(const std::shared_ptr<mpj_Motor>& motor, const mpj_MotorParams& motorParams);

		//! Create a new instance of mpj_MotorManager.
		/*!
			\param motor
				Motor.
			\sa \ref mpj_Motor
		*/
		static std::shared_ptr<mpj_MotorManager> create(const std::shared_ptr<mpj_Motor>& motor);

		//! Get underlying motor for low-level access.
		/*!
			It's strongly discouraged to access the actual motor unless you have enough knowledge about how they work.
			\return
				Motor instance.
		*/
		virtual mpj_Motor* getMotor() = 0;

		//! Get underlying motor for low-level access.
		/*!
			It's strongly discouraged to access the actual motor unless you have enough knowledge about how they work.
			\return
			Motor instance.
		*/
		virtual const mpj_Motor* getMotor() const = 0;

		//! Get current motor angle.
		/*!
			\return
				Current motor angle.
		*/
		virtual mpj_Angle getCurrentAngle() const = 0;

		//! Set motor's goal angle.
		/*!
			\param angle Motor's goal angle.
		*/
		virtual void setGoalAngle(const mpj_Angle& angle) = 0;
		
		//! Apply motor parameters.
		/*!
			\param motorParams Motor parameters to apply.
		*/
		virtual void applyMotorParams(const mpj_MotorParams& motorParams) = 0;

		//! Get motor parameters applied last.
		/*!
			\return
				Motor parameters applied by either constructor or \ref applyMotorParams.
		*/
		virtual const mpj_MotorParams& getMotorParamsLastApplied() = 0;

		//! Determine whether motor is working or not.
		/*!
			\return
				true if motor is working.
			\sa \ref mpj_Motor::isMotorRotating
		*/
		virtual bool isMotorRotating() const = 0;
	};


	//! Motor registry entry.
	/*!
	The entry is used to resolve \ref mpj_MotorConstants and \ref mpj_Motor for known named
	motor models.
	*/
	struct mpj_MotorRegistryEntry
	{
		//! Definition of function to obtain \ref mpj_MotorConstants.
		using GetConstantsFunc = std::function<const mpj_MotorConstants* ()>;
		//! Definition of function to create \ref mpj_Motor.
		/*!
		\param config Motor configuration such as motor serial port or motor configuration.
		*/
		using CreateMotorFunc = std::function<std::shared_ptr<mpj_Motor>(const std::string&)>;

		//! Function to obtain \ref mpj_MotorConstants.
		GetConstantsFunc getConstants;
		//! Function to create \ref mpj_Motor.
		CreateMotorFunc createMotor;

		//! Initialize an instance.
		/*!
		\param getConstants Function to obtain \ref mpj_MotorConstants.
		\param createMotor Function to create \ref mpj_Motor.
		*/
		mpj_MotorRegistryEntry(
			const GetConstantsFunc& getConstants, const CreateMotorFunc& createMotor) :
			getConstants(getConstants), createMotor(createMotor)
		{
			if (!getConstants)
				throw mpj_InvalidParamException(_LINE_AND_FILE "getConstants is empty.");
			if (!createMotor)
				throw mpj_InvalidParamException(_LINE_AND_FILE "createMotor is empty.");
		}
	};

	//! Motor registry.
	/*!
	Motor registry is used to resolve motor name to actual motor.
	You can of course, add new motor implementations by modifying the map returned by
	\ref getRegistry.
	*/
	class mpj_MotorRegistry
	{
	public:
		using EntryPtr = std::shared_ptr<const mpj_MotorRegistryEntry>;
		using Map = std::map<std::string, EntryPtr>;

		//! Get motor registry.
		static Map& getRegistry();

		//! Get motor registry entry for name.
		/*!
		\param name Motor name to find.
		\return Pointer to an entry for the name if found; otherwise nullptr.
		*/
		static const mpj_MotorRegistryEntry* getEntryFor(const std::string& name)
		{
			const auto& map = getRegistry();
			auto it = map.find(name);
			if (it != map.end())
				return it->second.get();
			return nullptr;
		}

	private:
		mpj_MotorRegistry() = delete;
		mpj_MotorRegistry(const mpj_MotorRegistry&) = delete;
		mpj_MotorRegistry& operator=(const mpj_MotorRegistry&) = delete;
	};
}

#endif // _mpj_motor_h_
