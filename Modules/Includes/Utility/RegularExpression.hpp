#ifndef _UTILITY_REGULAR_EXPRESSSION_HPP_
#define _UTILITY_REGULAR_EXPRESSSION_HPP_

#include <string>
#include <vector>

namespace Utility
{
	typedef std::pair<std::string::const_iterator, std::string::const_iterator> Regex_Match;
	typedef std::vector<Regex_Match> Regex_MatchArray;

	class Regex
	{
		private:

		public:
			// Returns true if the expression is found
			// Results are stored on OutList
			static bool Find(std::string::const_iterator Begin, std::string::const_iterator End,const std::string& Expression,Regex_MatchArray& Out);
			static bool Find(const std::string& Source,const std::string& Expression,Regex_MatchArray& Out);

			// Returns true if the source matchs the expression
			static bool Match(std::string::const_iterator Begin, std::string::const_iterator End,const std::string& Expression);
			static bool Match(const std::string& Source, const std::string& Expression);

	};
}

#endif
