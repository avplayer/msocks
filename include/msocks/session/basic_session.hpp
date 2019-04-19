//
// Created by maxtorm on 2019/4/14.
//

#pragma once


#include <boost/asio/ip/tcp.hpp>
#include <botan/stream_cipher.h>

#include <shadowsocks/stream.h>

using namespace boost::asio;
using namespace boost::system;
namespace msocks
{

class basic_session : public noncopyable
{
public:

	const std::string& uuid() const noexcept;

protected:
	basic_session(io_context& ioc);

	io_context& ioc_;
	std::string uuid_;
	std::array<uint8_t,4096> buffer_local_;
	std::array<uint8_t,4096> buffer_remote_;


};

}
