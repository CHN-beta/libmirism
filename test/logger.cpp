# include <mirism/mirism.hpp>

int main()
{
	mirism::Logger::init(std::experimental::observer_ptr<std::ostream>(&std::cout), mirism::Logger::Level::Debug);
	mirism::Logger::Guard log;
	log();
	struct A : public mirism::Logger::ObjectMonitor<A> {} a;
}
