#pragma once

#include <boost/asio/spawn.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <msocks/session/basic_session.hpp>
#include <shadowsocks/stream.h>

using namespace boost::system;
using namespace boost::asio;

namespace msocks
{

struct client_session_attribute
{
	client_session_attribute() : timeout(0)
	{};
	std::string remote_address;
	uint16_t remote_port = 0;
	std::string password;
	std::string method;
	boost::posix_time::seconds timeout;
};

class client_session final : public basic_session, public std::enable_shared_from_this<client_session>
{
public:
	client_session(io_context &ioc, ip::tcp::socket &&local, const client_session_attribute &attribute)
		: basic_session(ioc),
		  local_(std::move(local)),
		  remote_(ip::tcp::socket{ioc},
		          shadowsocks::cipher_context(shadowsocks::cipher_info(attribute.method, attribute.password))),
		  attribute_(attribute)
	{}
	
	void go();
private:
	
	void start(yield_context yield);
	
	void fwd_local_remote(yield_context yield);
	void fwd_remote_local(yield_context yield);
	
	ip::tcp::socket local_;
	
	shadowsocks::stream<ip::tcp::socket> remote_;
	
	const client_session_attribute &attribute_;
};

}
