# pragma once
# include <mirism/instance.hpp>
# include <mirism/server/base.hpp>
# include <mirism/client/base.hpp>
# include <mirism/handler/base.hpp>

namespace mirism
{
	inline Instance::Instance
	(
		std::unique_ptr<server::Base> server, std::unique_ptr<client::Base> client,
		std::unique_ptr<handler::Base> handler
	)
	: Server_(std::move(server)), Client_(std::move(client)), Handler_(std::move(handler))
	{
		Logger::Guard guard(Server_, Client_, Handler_);
		if (!Server_ || !Client_ || !Handler_)
			guard.log<Logger::Guard::Error>("nullptr is not allowed, will run anyway");
	}
	inline void Instance::run(bool async)
		{(*Server_)(async, [this](auto request){return (*Handler_)(request, *Client_);});}
}
