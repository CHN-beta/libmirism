# include <mirism/mirism.hpp>
# include <mirism/backend/httplib.hpp>

int main()
{
	mirism::Logger::init(std::experimental::observer_ptr<std::ostream>(&std::cout), mirism::Logger::Level::Debug);
	mirism::server::Httplib<false> httplib("localhost", 8080);
	httplib(false, [](auto request)
	{
		mirism::Instance::Response response;
		response.Status = 200;
		response.Headers.emplace("Content-Type", "text/plain");
		response.Body = std::make_shared<mirism::Pipe>();
		response.Body->push("Hello, World!");
		response.Body->push(mirism::Pipe::Signal::EndOfFile);
		return response;
	});
}
