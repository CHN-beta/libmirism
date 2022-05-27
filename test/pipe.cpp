# include <mirism/common.impl.hpp>
# include <mirism/atomic.impl.hpp>
# include <mirism/pipe.impl.hpp>
# include <mirism/logger.impl.hpp>
# include <iostream>

int main()
{
	auto p = std::make_shared<mirism::Pipe>();
	p->push("Hello");
	p->push("World");
	p->push(mirism::Pipe::Signal::EndOfFile);
	while (true)
	{
		auto value = p->pop();
		if (value)
			{
				if (value->index() == 0)
					std::cout << std::get<std::string>(*value) << std::endl;
				else
					std::cout << "Signal" << std::endl;
			}
		else
			break;
	}
}
