# include <mirism/detail_/misc/common.impl.hpp>
# include <mirism/detail_/misc/concepts.impl.hpp>
# include <mirism/detail_/misc/atomic.impl.hpp>
# include <mirism/detail_/misc/format.impl.hpp>
# include <mirism/detail_/misc/logger.impl.hpp>
# include <mirism/detail_/misc/pipe.impl.hpp>

int main()
{
	// use pipe to pass string and signal between threads
	// pipe should always be wrapped by std::shared_ptr
	auto pipe = std::make_shared<mirism::Pipe>();

	// push string and signal into pipe
	// if pipe is full, this function will be blocked for at most 10s
	if (!(pipe->push("hello") && pipe->push(" world") && pipe->push(mirism::Pipe::Signal::EndOfFile)))
		std::cout << "push failed.\n";

	// pop string and signal from pipe. if pipe is empty, this function will be blocked for at most 10s
	auto value = pipe->pop();

	// get first element from pipe without pop. if pipe is empty, this function will be blocked for at most 10s
	// auto value = pipe->front();

	// check if pipe is empty now
	if (pipe->empty())
		std::cout << "pipe is empty.\n";
	else
		std::cout << "pipe is not empty.\n";
}
