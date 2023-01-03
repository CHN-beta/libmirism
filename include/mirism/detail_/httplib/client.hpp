# pragma once
// # include <mirism/framework.hpp>
# include <mirism/detail_/framework/client.hpp>

namespace mirism::client
{
	class Httplib : public Base, public Logger::ObjectMonitor<Httplib>
	{
		public: std::unique_ptr<http::Response_t> operator()(std::unique_ptr<http::Request_t> request) override;
		public: Atomic<bool> DisableServerCertificateVerification = false;
	};
}
