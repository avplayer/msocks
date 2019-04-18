#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <boost/endian/conversion.hpp>
#include <botan/auto_rng.h>

#include <msocks/session/client_session.hpp>
#include <msocks/utility/socket_pair.hpp>
#include <msocks/utility/local_socks5.hpp>

#include <spdlog/spdlog.h>

namespace msocks
{

void client_session::start(yield_context yield)
{
	try
	{
		auto target_address = utility::async_local_socks5(ioc_, local_, yield);

		ip::tcp::endpoint ep(ip::make_address(attribute_.remote_address), attribute_.remote_port);
        remote_.next_layer().open(ep.protocol());
		remote_.next_layer().async_connect(ep, yield);
        async_write(remote_, buffer(target_address), yield);
		spawn(
			ioc_,
			[this, p = shared_from_this()](yield_context yield)
		{
			fwd_local_remote(yield);
		});
		spawn(
			ioc_,
			[this, p = shared_from_this()](yield_context yield)
		{
			fwd_remote_local(yield);
		});
	}
	catch (system_error& e)
	{
		spdlog::info("[{}] error: {}", uuid(), e.what());
	}
}

void client_session::fwd_remote_local(yield_context yield)
{
	try
	{
		error_code ec;
		utility::socket_pair(
			remote_, local_,
			ioc_,
			buffer(buffer_remote_),
			yield[ec]);

	}
	catch (system_error & ignored)
	{
	}
}

void client_session::fwd_local_remote(yield_context yield)
{
	try
	{
		error_code ec;
		utility::socket_pair(
			local_, remote_,
			ioc_,
			buffer(buffer_local_),
			yield[ec]);

	}
	catch (system_error & ignored)
	{
	}
}

void client_session::go()
{
	spawn(
		ioc_,
		[this, p = shared_from_this()](yield_context yield)
	{
		start(yield);
	}
	);
}

}
