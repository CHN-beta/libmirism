# include <mirism/detail_/misc/common.impl.hpp>
# include <mirism/detail_/misc/concepts.impl.hpp>
# include <mirism/detail_/misc/atomic.impl.hpp>
# include <mirism/detail_/misc/string.impl.hpp>

using namespace mirism::literals;

// a `mirism::StaticString` is a string that storing all its characters in template parameters.
// using `mirism::literals::operator""_ss` to create a instance, and use the static member variable `Array` and
// `StringView` to access the string as an array or a string view.
// using `mirism::specialization_of_static_string` to test if a type is a specialization of `mirism::StaticString`.
// using `mirism::stream_operators::operator<<` to print a `mirism::StaticString`.
template <typename S> struct TestStaticString;
template <mirism::specialization_of_static_string S> struct TestStaticString<S>
{
	constexpr static bool happy = false;
};
template <mirism::specialization_of_static_string<char8_t> S> struct TestStaticString<S>
{
	constexpr static bool happy = true; // all char8_t string is happy
};
template <> struct TestStaticString<decltype("happy"_ss)> // "happy" also happy
{
	constexpr static bool happy = true;
};
static_assert(TestStaticString<decltype("happy"_ss)>::happy, "error");
static_assert(TestStaticString<decltype(u8"sad"_ss)>::happy, "error");
static_assert(!TestStaticString<decltype("sad"_ss)>::happy, "error");

// a `mirism::FixedString` is a string that have a fixed size.
// it is somehow similar to `mirism::StaticString`, but strings with same size share the same type.
// by using the constexpr constructor `FixedString(const Char (&str)[N])`, it is possible to use "plain string" as
// non-type template parameter, this is why I create it.
template <mirism::FixedString S> struct TestFixedString
{
	TestFixedString()
	{
		std::cout << "hi, you created " << S << std::endl;
	}
};
void test_fixed_string()
{
	// same as TestFixedString<"hello"_fs> a, b, c;
	TestFixedString<"hello"> a, b, c;	// wow, c++ now supports string as template parameter!
	static_assert(mirism::detail_::specialization_of_fixed_string_<decltype("hello"_fs), char>, "failed");
}

int main()
{
	test_fixed_string();
}
