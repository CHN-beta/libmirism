# pragma once
# include <mirism/instance.hpp>

namespace mirism::client
{
	// Same comments as in mirism::client::Request.
	struct Response
	{
		std::string Status;
		std::multimap<std::string, std::string> Headers;
		std::shared_ptr<Pipe> Body;
		std::optional<Instance::EndPoint> Remote;
	};
}
