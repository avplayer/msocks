//
// Created by maxtorm on 2019/4/13.
//

#pragma once
#include <msocks/endpoint/basic_endpoint.hpp>
#include <shadowsocks/cipher_info.hpp>

namespace msocks
{

struct client_config
{
	client_config() : timeout(0)
	{
	};
	std::string local_address;
	uint16_t local_port = 0;
	std::string remote_address;
	uint16_t remote_port = 0;
	std::string method;
	std::string password;
	boost::posix_time::seconds timeout;
};

class client_endpoint final : public basic_endpoint
{
public:
	client_endpoint(io_context& ioc, client_config cfg) :
		basic_endpoint(ioc),
		cfg_(std::move(cfg))
	{}

	void start();

private:
	client_config cfg_;

};

}

