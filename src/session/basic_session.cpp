//
// Created by maxtorm on 2019/4/14.
//
#include <msocks/session/basic_session.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
using namespace boost::uuids;
namespace msocks
{

const std::string &basic_session::uuid() const noexcept
{
	return uuid_;
}

basic_session::basic_session(io_context &ioc) :
	ioc_(ioc), 
	uuid_(to_string(random_generator_mt19937()()))
{
}

}
