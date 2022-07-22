# include <mirism/detail_/misc/common.impl.hpp>
# include <mirism/detail_/misc/concepts.impl.hpp>
# include <mirism/detail_/misc/atomic.impl.hpp>
# include <mirism/detail_/misc/string.impl.hpp>

using namespace mirism::literals;

// a `mirism::BasicStaticString` is a string that storing all its characters in template parameters.
// using `mirism::literals::operator""_ss` to create a instance, and use the static member variable `Array` and
// `StringView` to access the string as an array or a string view.
// using `mirism::specialization_of_static_string` to test if a type is a specialization of `mirism::BasicStaticString`.
// using `mirism::stream_operators::operator<<` to print a `mirism::BasicStaticString`.
// `StaticString` and `specialization_of_static_string` are char versions of basic ones.
template <typename S> struct TestBasicStaticString;
template <mirism::specialization_of_basic_static_string S> struct TestBasicStaticString<S>
{
	constexpr static bool happy = false;
};
template <mirism::specialization_of_basic_static_string<char8_t> S> struct TestBasicStaticString<S>
{
	constexpr static bool happy = true; // all char8_t string is happy
};
template <> struct TestBasicStaticString<decltype("happy"_ss)> // "happy" also happy
{
	constexpr static bool happy = true;
};
static_assert(TestBasicStaticString<decltype("happy"_ss)>::happy, "error");
static_assert(TestBasicStaticString<decltype(u8"sad"_ss)>::happy, "error");
static_assert(!TestBasicStaticString<decltype("sad"_ss)>::happy, "error");

// a `mirism::BasicFixedString` is a string that have a fixed size.
// it is somehow similar to `mirism::BasicStaticString`, but strings with same size share the same type.
// by using the constexpr constructor `BasicFixedString(const Char (&str)[N])`, it is possible to use "plain string" as
// non-type template parameter, this is why I create it.
// also have `FixedString`.
template <mirism::BasicFixedString S> struct TestBasicFixedString
{
	TestBasicFixedString()
	{
		std::cout << "hi, you created " << S << std::endl;
	}
};
void test_basic_fixed_string()
{
	// same as TestFixedString<"hello"_fs> a, b, c;
	TestBasicFixedString<"hello"> a, b, c;	// wow, c++ now supports string as template parameter!
	static_assert(mirism::specialization_of_basic_fixed_string<decltype("hello"_fs), char>, "failed");
}

int main()
{
	test_basic_fixed_string();
}
