# include <iostream>
# include <mirism/utility/common.hpp>

int main()
{
	using namespace mirism::literals;

	std::cout << mirism::hash(1, "wow"s, 3.14) << std::endl;

	std::string s1;
	mirism::unused(s1);

	mirism::uint128_t test_u128 = 0;
	static_assert(sizeof(test_u128) == 16, "uint128_t is not 16 bytes");

	if (std::regex_match("hello", "[a-z]+"_re))
		std::cout << "match" << std::endl;
	else
		std::cout << "not match" << std::endl;

	std::map<std::string, int, mirism::CaseInsensitiveStringLessComparator> test_case_insensitive_string_less;
	test_case_insensitive_string_less.insert({"Hello", 1});
	if (test_case_insensitive_string_less.contains("hello"s))
		std::cout << "contains" << std::endl;
	else
		std::cout << "not contains" << std::endl;

	struct test_common_remove_member_pointer
	{
		int a;
	};
	static_assert(std::same_as<int,
		mirism::RemoveMemberPointerType<decltype(&test_common_remove_member_pointer::a)>>, "failed");
	
	// mirism::block_forever();

	static_assert(std::same_as<const std::string, mirism::MoveQualifiersType<const int, std::string>>, "failed");

	static_assert(std::same_as<int, mirism::FallbackIfNoTypeDeclaredType<std::enable_if<false>, int>>, "failed");
}
