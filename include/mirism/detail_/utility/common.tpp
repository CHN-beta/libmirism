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

	template <typename String> constexpr bool CaseInsensitiveStringLessComparator::operator()
		(const String& s1, const String& s2) const
	{
		return std::lexicographical_compare
		(
			s1.begin(), s1.end(), s2.begin(), s2.end(),
			[](char c1, char c2){return std::tolower(c1) < std::tolower(c2);}
		);
	}
}
