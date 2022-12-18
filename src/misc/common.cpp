# pragma once
# include <mirism/detail_/misc/common.hpp>

namespace mirism
{
	template<typename... Ts> inline std::size_t hash(Ts&&... objs)
	{
		std::size_t result = 0;
		(boost::hash_combine(result, objs), ...);
		return result;
	}
	inline void unused(auto&&...) {}

	inline std::regex literals::operator""_re(const char* str, std::size_t len)
		{return std::regex{str, len};}


	void block_forever()
	{
		std::promise<void>().get_future().wait();
		std::unreachable();
	}
}
