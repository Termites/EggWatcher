#include <Utility/RegularExpression.hpp>
#include <boost/regex.hpp>

bool Utility::Regex::Find(std::string::const_iterator Begin, std::string::const_iterator End,const std::string& Expression,Utility::Regex_MatchArray&Out)
{
	boost::regex R(Expression.c_str());
	boost::match_results<std::string::const_iterator> What;

	if (boost::regex_search(Begin,End,What,R))
	{
		Out.resize(What.size());
		for (size_t i=0;i<What.size();++i)
		{
			Out[i].first = What[i].first;
			Out[i].second = What[i].second;
		}
		return true;
	}

	return false;
}

bool Utility::Regex::Find(const std::string& Source,const std::string& Expression,Utility::Regex_MatchArray&Out)
{
	boost::regex R(Expression.c_str());
	boost::match_results<std::string::const_iterator> What;

	if (boost::regex_search(Source,What,R))
	{
		Out.resize(What.size());
		for (size_t i=0;i<What.size();++i)
		{
			Out[i].first = What[i].first;
			Out[i].second = What[i].second;
		}
		return true;
	}

	return false;
}

bool Utility::Regex::Match(std::string::const_iterator Begin, std::string::const_iterator End,const std::string& Expression)
{
	boost::regex R(Expression);
	boost::match_results<std::string::const_iterator> What;

	if (boost::regex_match(Begin,End,What,R))
		return true;

	return false;
}

bool Utility::Regex::Match(const std::string& Source,const std::string& Expression)
{
	boost::regex R(Expression);
	boost::match_results<std::string::const_iterator> What;

	if (boost::regex_match(Source,What,R))
		return true;

	return false;
}
