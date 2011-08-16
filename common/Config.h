#ifndef __CONFIG_SETTING__
#define __CONFIG_SETTING__

#include "globledefine.h"
#include <boost/tokenizer.hpp>
#include <fstream>
#include <string>

class Config
{
public:
	Config(STRING str)
	{
		IFSTREAM fs(str);
		ZCHAR buffer[MAX_LINE_SIZE];
		boost::char_separator<ZCHAR> sep(L"=");
		while (!fs.eof())
		{
			memset(buffer, 0, sizeof(ZCHAR) * MAX_LINE_SIZE);
			fs.getline(buffer, MAX_LINE_SIZE);
			STRING tmp(buffer);
			boost::tokenizer<boost::char_separator<ZCHAR>, STRING::const_iterator, STRING> tok(tmp, sep);
			m_data[*tok.begin()] = *(++tok.begin());
		}
	}
	STRING Get(STRING key)
	{
		return m_data[key];
	}
private:
	enum { MAX_LINE_SIZE = 128 };
	std::map<STRING, STRING> m_data;
};

#endif	