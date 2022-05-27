# pragma once
# include <mirism/logger.hpp>

namespace mirism::mirror
{
	// Define the rule to process a set of mirrored sites.
	class Base
	{
		public: virtual ~Base() = default;

		public: virtual std::optional<ServerRequest> server_process(ServerRequest request) = 0;
		public: virtual std::optional<ServerResponse> server_process(ServerResponse response) = 0;

		// The function should not be 
		public: virtual std::set<std::string> get_sites() const = 0;
	};
}