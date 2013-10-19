#ifndef _MODULE_UTILITY_STRINGCONVERSION_HPP_
#define _MODULE_UTILITY_STRINGCONVERSION_HPP_

#include <string>
#include <sstream>

namespace Utility
{
	template <typename T>
	std::string ConvertToString( const T& Value)
	{
		std::stringstream SS(Value);

		return SS.str();
	}

	template <typename T>
	T ConvertFromString(const std::string& Value)
	{
		T Out;
		std::stringstream SS(Value);
		SS>>Out;

		return Out;
	}
}

#endif
