# pragma once
# include <mirism/framework.hpp>

namespace mirism::client
{
	class Httplib_t : public Base_t, public Logger_t::ObjectMonitor_t<Httplib_t>
	{
		public: std::unique_ptr<http::Response_t> operator()(std::unique_ptr<http::Request_t> request) override;
		public: Atomic_t<bool> DisableServerCertificateVerification = false;
	};
}
