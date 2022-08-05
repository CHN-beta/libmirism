# include <mirism/detail_/misc/common.impl.hpp>
# include <mirism/detail_/misc/concepts.impl.hpp>
# include <mirism/detail_/misc/atomic.impl.hpp>
# include <mirism/detail_/misc/format.impl.hpp>
# include <mirism/detail_/misc/logger.impl.hpp>

using namespace mirism::literals;

// inherit Logger::ObjectMonitor<my_struct> to log my_struct's life cycle
struct my_struct : public mirism::Logger::ObjectMonitor<my_struct> {};

int do_something(int arg1, float arg2)
{
	// create a Logger::Guard at start of a function with parameters.
	// if log level >= Debug, some information will be print into the log, include function name, parameters, start and
	// return time, and so on.
	mirism::Logger::Guard log{arg1, arg2};

	// use Logger::Guard to log something.
	log.log<mirism::Logger::Level::Info>("{} {}"_f("hello", "world"));

	// use operator() to print a simple message, indicating the program reached here.
	// only log level >= Debug will be printed.
	log();

	// if telegram connection is initialized, log will be sent to telegram, in a sync or async way.
	mirism::Logger::telegram_notify_async("KFC crasy thursday v me 50");

	// use `rtn` to log the returned value.
	// only log level >= Debug will be printed.
	return log.rtn(3);
}

int main()
{
	// init logger before use it
	mirism::Logger::init(std::experimental::observer_ptr<std::ostream>(&std::cout), mirism::Logger::Level::Debug);

	// optionally init telegram connection
	// mirism::Logger::telegram_init("token", "chat_id");

	my_struct obj;
	do_something(1, 2);

	// print current threads and monitoring objects
	std::cout << "threads: {} objects: {}\n"_f(mirism::Logger::Threads.get(), mirism::Logger::Objects.get());
}
