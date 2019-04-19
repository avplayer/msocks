#include <spdlog/spdlog.h>

#include <msocks/endpoint/server_endpoint.hpp>
#include <msocks/endpoint/client_endpoint.hpp>
#include <msocks/session/pool.hpp>
#include <boost/asio/io_context.hpp>

#include <botan/sha2_32.h>
#include <botan/md5.h>

void log_setup()
{
	spdlog::set_pattern("[%l] %v");
}


int main(int argc, char* argv[])
{
	try
	{
		io_context ioc;
		std::string password(argv[4]);
		if (!strcmp(argv[1], "s"))
		{
			msocks::pool<msocks::server_session> pool(ioc);
			msocks::server_endpoint_config config;
			config.no_delay = true;
			config.server_address = argv[2];
			config.server_port = std::stoi(argv[3]);
			config.speed_limit = std::stoi(argv[5]);
			config.method = "chacha20";
			config.password = password;
			config.timeout = boost::posix_time::seconds(2);
			msocks::server_endpoint server(ioc, pool, std::move(config));
			server.start();
			ioc.run();
		}
		else
		{
			msocks::client_config config;
			config.local_address = "127.0.0.1";
			config.local_port = 1081;
			config.remote_address = argv[2];
			config.remote_port = std::stoi(argv[3]);
			config.method = "chacha20";
			config.password = password;
			config.timeout = boost::posix_time::seconds(2);
			msocks::client_endpoint client(ioc, std::move(config));
			client.start();
			ioc.run();
		}
	}
	catch (boost::exception & e)
	{
		spdlog::error("{}",boost::diagnostic_information(e));
	}
	system("pause");
}

