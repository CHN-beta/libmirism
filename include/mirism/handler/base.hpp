# include <mirism/client/base.hpp>

namespace mirism::handler
{
	// exact rules to handle the requests
	class Base
	{
		public: virtual ~Base() = default;
		public: virtual std::optional<Instance<>::ClientResponse>
			operator()(Instance<>::ClientRequest, client::Base&) = 0;
	};
}
