#include "mainthread.h"
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/logger/logger.hpp>

#pragma comment(lib, "boost.lib")

void MainThread::ini()
{
	boost::thread thrd(boost::bind(&MainThread::run, this));
}
void MainThread::run()
{
	boost::asio::io_service io_service;
	
	services::logger logger(io_service, "log");
	logger.use_file("log.txt");
	
	logger.log("123");
	logger.log("123123");
	logger.log("12312111");
}

