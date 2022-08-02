# include <mirism/detail_/misc/common.impl.hpp>
# include <mirism/detail_/misc/concepts.impl.hpp>
# include <mirism/detail_/misc/atomic.impl.hpp>
# include <mirism/detail_/misc/format.impl.hpp>
# include <mirism/detail_/misc/logger.impl.hpp>
# include <mirism/detail_/misc/pipe.impl.hpp>
# include <mirism/detail_/misc/string.impl.hpp>
# include <mirism/detail_/content/base.impl.hpp>
# include <mirism/detail_/content/binary.impl.hpp>

using namespace mirism::literals;

int main()
{
	mirism::content::Binary content;
	auto in = std::make_shared<mirism::Pipe>();
	auto out = std::make_shared<mirism::Pipe>();
	in->push("hello world");
	in->push(mirism::Pipe::Signal::EndOfFile);
	content.set_pipe_read(in);
	content.set_pipe_write(out);
	content.process();
	auto chunk = out->pop();
	std::cout << "{}"_f(chunk);
}
