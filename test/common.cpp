# include <mirism/detail_/common.impl.hpp>
# include <iostream>
using namespace mirism::literals;
using namespace mirism::stream_operators;

// using `mirism::hash` to get hash of any number and any type of objects that is supported by `boost::hash_combine`
void test_hash()
{
	std::cout << mirism::hash(1, "wow"s, 3.14) << std::endl;
}

// using `mirism::unused` to ignore any unused variable to avoid compiler warning
void test_unused()
{
	std::string s1;
	mirism::unused(s1);
}

// using `mirism::uint128_t` to represent 128-bit unsigned integer
mirism::uint128_t test_u128 = 0;
static_assert(sizeof(test_u128) == 16, "uint128_t is not 16 bytes");

// using `mirism::literals::operator""_re` to create a regular expression
void test_regex()
{
	if (std::regex_match("hello", "[a-z]+"_re))
		std::cout << "match" << std::endl;
	else
		std::cout << "not match" << std::endl;
}

// a `mirism::StaticString` is a string that storing all its characters in template parameters.
// using `mirism::literals::operator""_ss` to create a instance, and use the static member variable `Array` and
// `StringView` to access the string as an array or a string view.
// due to the instance does not store any information other than its template parameters, it is usually used with
// typename template parameter and `decltype`.
template <mirism::StaticString S> struct TestStaticString
{
	constexpr static bool happy = false;
};
template <> struct TestStaticString<decltype("happy"_ss)>
{
	constexpr static bool happy = true;
};


int main()
{
	test_hash();
	test_unused();
	test_regex();
}
