//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_common.h
//! \brief String utility functions
//---------------------------------------------------------------------------

#ifndef _mpj_string_h_
#define _mpj_string_h_

#include <algorithm>
#include <string>
#include <functional>
#include <filesystem>
#include <vector>
#include <set>
#include <regex>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem> // Header file for pre-standard implementation

namespace mpjsdk
{
	//! String utility functions.
	class mpj_StringUtils
	{
	public:
		//! Format string with sprintf style format specification.
		/*!
			\param format sprintf style format.
			\param args Format arguments.
		*/
		template<typename ... Args>
		static std::string format(const std::string& format, Args ... args)
		{
            std::string rv(" ");
            return rv;
		}

		//! Load data from file to newly allocated buffer by callback function.
		/*!
		\param fileName File path.
		\param allocateBuffer Function to allocate buffer of size (in bytes).
		*/
		static void loadFromFile(const _FSPFX path& fileName, std::function<void* (size_t size)> allocateBuffer);

		//! Load text data from file.
		/*!
		\param fileName File path.
		\return Loaded text data.
		*/
		static std::string loadFromFile(const _FSPFX path& fileName);

		//! Load text data from file.
		/*!
		\param fileName File path.
		\param onLine Function called on each line.
		The line data does not contain line terminator (such as LF, CR, or CRLF).
		The callback function also accepts input file name and line number.
		*/
		static void loadFromFile(
			const _FSPFX path& fileName,
			std::function<void(const char* line, const _FSPFX path& fileName, size_t lineNumber)> onLine);

		//! Save data to file.
		/*!
		\param fileName File path.
		\param buffer Buffer containing data.
		\param length Size of buffer; for text data, don't include the last null-terminator.
		*/
		static void saveToFile(const _FSPFX path& fileName, const void* buffer, size_t length);
		//! Save data to file.
		/*!
		\param fileName File path.
		\param str Text to save.
		*/
		static void saveToFile(const _FSPFX path& fileName, const std::string& str);

		static std::vector<std::string> split(const std::string& str, const std::regex& reSplitter)
		{
			std::vector<std::string> list;
			std::smatch m;
			size_t p = 0;
			while (std::regex_search(str.begin() + p, str.end(), m, reSplitter))
			{
				auto pmatch = m.position();
				list.push_back(str.substr(p, pmatch));
				p += pmatch + m.length();
			}
			if (p < str.size())
				list.push_back(str.substr(p));
			return list;
		}

		static std::set<std::string> split2set(const std::string& str, const std::regex& reSplitter)
		{
			std::set<std::string> list;
			std::smatch m;
			size_t p = 0;
			while (std::regex_search(str, m, reSplitter))
			{
				auto pos = m.position();
				list.insert(str.substr(p, pos - p));
				p = pos + m.length();
			}
			if (p < str.size())
				list.insert(str.substr(p));
			return std::move(list);
		}

#if defined(_WIN32) || defined(DOXYGEN)
		//! [Windows only] Convert multibyte string to wide string.
		/*!
		\param str String to convert.
		\param codePage Code page. The default is CP_ACP.
		The function internally uses [MultiByteToWideChar function](https://msdn.microsoft.com/en-us/library/windows/desktop/dd319072(v=vs.85).aspx).
		*/
		static std::wstring convert(const std::string& str, uint32_t codePage = 0);
		//! [Windows only] Convert wide (UNICODE) string to multibyte string.
		/*!
		\param str String to convert.
		\param codePage Code page. The default is CP_ACP.
		The function internally uses [WideCharToMultiByte function](https://msdn.microsoft.com/en-us/library/windows/desktop/dd374130(v=vs.85).aspx).
		*/
		static std::string convert(const std::wstring& str, uint32_t codePage = 0);
#endif

	private:
		mpj_StringUtils() = delete;
		mpj_StringUtils(const mpj_StringUtils&) = delete;
		mpj_StringUtils& operator=(const mpj_StringUtils&) = delete;

	};
}

#endif // _mpj_string_h_
