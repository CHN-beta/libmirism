// common.tpp:
//	uint128_t;
//	namespace literals;
//	std::regex operator""_re(...);
//	namespace stream_operators;
//	template <typename T> ... remove_member_pointer_t;
//	void block_forever();
//	std::size_t hash(Ts&&... objs);
//	void unused(auto&&...);
//	struct CaseInsensitiveStringLess;

# pragma once
# include <boost/functional/hash.hpp>
# include <mirism/detail_/utility/common.hpp>

namespace mirism
{
	inline void unused(auto&&...) {}
	std::size_t hash(auto&&... objs)
	{
		std::size_t result = 0;
		(boost::hash_combine(result, objs), ...);
		return result;
	}

	template <typename T> constexpr bool CaseInsensitiveStringLess_t::operator()(const T& lhs, const T& rhs) const
	{
		return std::lexicographical_compare
		(
			lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
			[](char lhs, char rhs){return std::tolower(lhs) < std::tolower(rhs);}
		);
	}
}
