#ifndef _mpj_serialports_h_
#define _mpj_serialports_h_

#include <map>
#include <string>

namespace mpjsdk
{
	class mpj_SerialPortEnumerator
	{
	public:
		static std::map<std::string, std::string> enumerateSerialPorts();

	private:
		mpj_SerialPortEnumerator() = delete;
		mpj_SerialPortEnumerator(const mpj_SerialPortEnumerator&) = delete;
		mpj_SerialPortEnumerator& operator=(const mpj_SerialPortEnumerator&) = delete;
	};
}

#endif // _mpj_enumserialports_h_
