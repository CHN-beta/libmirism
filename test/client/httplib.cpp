# include <mirism/detail_/misc/common.impl.hpp>
# include <mirism/detail_/misc/concepts.impl.hpp>
# include <mirism/detail_/misc/atomic.impl.hpp>
# include <mirism/detail_/misc/format.impl.hpp>
# include <mirism/detail_/misc/logger.impl.hpp>
# include <mirism/detail_/misc/pipe.impl.hpp>
# include <mirism/detail_/misc/string.impl.hpp>
# include <mirism/detail_/instance.impl.hpp>
# include <mirism/detail_/client/httplib.impl.hpp>

using namespace mirism::literals;
using namespace mirism::stream_operators;

int main()
{
	mirism::Logger::init(std::experimental::observer_ptr<std::ostream>(&std::cout), mirism::Logger::Level::Debug);
	mirism::Logger::Guard log;

	auto request = std::make_unique<mirism::Instance::Request>();
	request->Scheme = mirism::Instance::HttpScheme::Https;
	request->Method = mirism::Instance::HttpMethod::Get;
	request->Path = "/yhirose/cpp-httplib/master/CMakeLists.txt";
	request->Headers.emplace("Host", "raw.githubusercontent.com");

	log.log<mirism::Logger::Level::Debug>("Request: {}"_f(request));

	auto client = std::make_shared<mirism::client::Httplib>();
	auto response = (*client)(std::move(request));
	log.log<mirism::Logger::Level::Debug>("Response: {} body: {}"_f(response, response->Body->front()));
}
