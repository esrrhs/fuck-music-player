#ifndef __LOGGER__
#define __LOGGER__

#include <boost/asio.hpp>
#include <ex/logger/logger.hpp>
#include "globledefine.h"

class Logger
{
public:
	void log(const std::string & file, const std::string & tag, const std::string & str)
	{
		if (m_filemap[file] == NULL)
		{
			m_filemap[file] = new boost::asio::io_service;
			m_tagmap[tag] = new services::logger(*m_filemap[file], tag);
			m_tagmap[tag]->use_file(file);
		}
		else if (m_tagmap[tag] == NULL)
		{
			m_tagmap[tag] = new services::logger(*m_filemap[file], tag);
		}
		m_tagmap[tag]->log(str);
	}
private:
	std::map<std::string, boost::asio::io_service*> m_filemap;
	std::map<std::string, services::logger*> m_tagmap;
};

#endif	