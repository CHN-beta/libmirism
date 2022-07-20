# include <mirism/detail_/misc/common.impl.hpp>
# include <iostream>

using namespace mirism::literals;

// Use `hash` to get hash of any number and any type of objects that is supported by `boost::hash_combine`
void test_hash()
{
	std::cout << mirism::hash(1, "wow"s, 3.14) << std::endl;
}

// Use `unused` to ignore any unused variable to avoid compiler warning
void test_unused()
{
	std::string s1;
	mirism::unused(s1);
}

// Use `uint128_t` to represent 128-bit unsigned integer
mirism::uint128_t test_u128 = 0;
static_assert(sizeof(test_u128) == 16, "uint128_t is not 16 bytes");

// Use `literals::operator""_re` to create an instance of `std::regex`
void test_regex()
{
	if (std::regex_match("hello", "[a-z]+"_re))
		std::cout << "match" << std::endl;
	else
		std::cout << "not match" << std::endl;
}

// Use `CaseInsensitiveStringLess` to create an ordered container of strings with case insensitive comparison
std::map<std::string, int, mirism::CaseInsensitiveStringLess> TestCaseInsensitiveStringLess;

// Use `remove_member_pointer{,_t}` to get type that a member pointer points to. That is, for example, convert
// `T C::*` to `T`
struct TestRemoveMemberPointer
{
	int a;
};
static_assert(std::same_as<int, mirism::remove_member_pointer_t<decltype(&TestRemoveMemberPointer::a)>>, "failed");

int main()
{
	test_hash();
	test_unused();
	test_regex();
}
