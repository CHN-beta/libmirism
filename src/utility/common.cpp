# include <future>
# include <utility>
# include <mirism/utility/common.hpp>

namespace mirism
{
	std::regex literals::operator""_re(const char* str, std::size_t len)
		{return std::regex{str, len};}
	void block_forever()
	{
		std::promise<void>().get_future().wait();
		std::unreachable();
	}
}
