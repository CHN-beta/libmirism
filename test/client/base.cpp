# include <mirism/detail_/misc/common.impl.hpp>
# include <mirism/detail_/misc/concepts.impl.hpp>
# include <mirism/detail_/misc/atomic.impl.hpp>
# include <mirism/detail_/misc/format.impl.hpp>
# include <mirism/detail_/misc/logger.impl.hpp>
# include <mirism/detail_/misc/pipe.impl.hpp>
# include <mirism/detail_/misc/string.impl.hpp>
# include <mirism/detail_/instance.impl.hpp>

using namespace mirism::literals;

// here we simply implement a simple client for testing purposes
class my_client : public mirism::client::Base, public mirism::Logger::ObjectMonitor<my_client>
{
	public: std::unique_ptr<mirism::Instance::Response> operator()
		(std::unique_ptr<mirism::Instance::Request> request = nullptr) override
	{
		// return 200 OK for Get index.html, otherwise return 404 Not Found
		if
		(
			request && request->Method == mirism::Instance::HttpMethod::Get
				&& (request->Path == "/index.html" || request->Path.starts_with("/index.html?"))
		)
		{
			auto response = std::make_unique<mirism::Instance::Response>();
			response->Status = 200;
			response->Body = std::make_shared<mirism::Pipe>();
			response->Body->push("<html><body>KFC crasy thursday v me 50</body></html>");
			return response;
		}
		else
		{
			auto response = std::make_unique<mirism::Instance::Response>();
			response->Status = 404;
			return response;
		}
	}
};

int main()
{
	mirism::Logger::init(std::experimental::observer_ptr<std::ostream>(&std::cout), mirism::Logger::Level::Debug);

	auto client = std::make_unique<my_client>();
	auto request = std::make_unique<mirism::Instance::Request>();
	request->Method = mirism::Instance::HttpMethod::Get;
	request->Path = "/index.html";
	std::cout << "request: {}\n"_f(request);
	auto response = client->operator()(std::move(request));
	std::cout << "response: {}\n"_f(response);
}
