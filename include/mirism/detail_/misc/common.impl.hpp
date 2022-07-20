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
	[[gnu::always_inline]] inline void unused(auto&&...) {}

	inline std::regex literals::operator""_re(const char* str, std::size_t len)
		{return std::regex(str, len);}

	template <typename T> constexpr inline bool CaseInsensitiveStringLess::operator()(const T& lhs, const T& rhs) const
	{
		return std::lexicographical_compare
		(
			lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
			[](char lhs, char rhs) {return std::tolower(lhs) < std::tolower(rhs);}
		);
	}
}
