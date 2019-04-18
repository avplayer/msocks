#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/streambuf.hpp>
#include <shadowsocks/stream.h>

using namespace boost::asio;
using namespace boost::system;

namespace msocks::utility
{
template <typename SourceStream, typename SinkStream, typename CompletionToken>
async_result<yield_context, void(error_code)>::return_type
socket_pair(
	SourceStream & source,
	SinkStream & sink,
	io_context& ioc,
	mutable_buffer m_buf,
	CompletionToken&& token
)
{
	using result_type = async_result<CompletionToken, void(error_code)>;
	using handler_type = typename result_type::completion_handler_type;
	handler_type handler(std::forward<CompletionToken>(token));
	result_type result(handler);
	spawn(
		ioc,
		[
			&source,
			&sink,
			&ioc,
			m_buf,
			handler
		](yield_context yield) mutable
	{
		error_code ec;
		while (true)
		{
			auto n_read = source.async_read_some(m_buf, yield[ec]);
			if (ec)
			{
				break;
			}
			
			if constexpr(std::is_same<shadowsocks::stream<ip::tcp::socket>, SinkStream>::value)
            {
                sink.async_write(buffer(m_buf, n_read), yield[ec]);
            }
            else
            {
                async_write(sink, buffer(m_buf, n_read), yield[ec]);
            }
            
			if (ec)
			{
				break;
			}
		}
		post(ioc, std::bind(handler, ec));
	});
	return result.get();
}

}
