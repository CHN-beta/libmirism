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

// using `mirism::literals::operator""_re` to create an instance of `std::regex`
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

// use `operator""_f` to call fmt::format
void test_operator_f()
{
	std::cout << "{} {}\n"_f("hello", "world");
}

// use `mirism::specialization_of` to test if a class is a specialization of an class.
// Only typename template parameter is supported.
template <typename T1, typename T2, typename T3> struct TestSpecializationOf {};
static_assert(mirism::specialization_of<TestSpecializationOf<int, float, double>, TestSpecializationOf>, "failed");
static_assert
(
	mirism::specialization_of<TestSpecializationOf<int, float, double>, TestSpecializationOf, int, float>,
	"failed"
);	// first two parameter is int then float
static_assert
	(!mirism::specialization_of<TestSpecializationOf<int, float, double>, TestSpecializationOf, float>, "failed");
static_assert(mirism::specialization_of<std::optional<int>, std::optional>, "failed");
// this does not work, since it have non-type template parameter
// static_assert(mirism::specialization_of<std::array<int, 3>, std::array>, "failed");
template <mirism::specialization_of<std::optional> T> struct TestSpecializationOf2 {};

// use `mirism::complete_type` to check if a type is complete until now (that is, when the compiler reached this line).
// however, if you check both before and after a type get complete, there will be a wierd complile error.
struct TestCompleteType;
// static_assert(!mirism::complete_type<TestCompleteType>, "failed");	// uncomment this causes the next assert failed
struct TestCompleteType {int a;};
static_assert(mirism::complete_type<TestCompleteType>, "failed");

// use `mirism::{{im,ex}plicitly_,}convertible_{to,from}` to check if a type is convertible to another type.
static_assert(mirism::implicitly_convertible_to<int, float>, "failed");

// use `mirism::consteval_invokable` to check a consteval function could be invoked by these parameters or not.
// However, you should manually wrap it as operator() of a class.
consteval void test_consteval_invokable(int a, int b)
{
	if (a + b < 0)
		throw "";
}
struct TestConstevalInvokable {consteval void operator()(int a, int b) const{test_consteval_invokable(a, b);}};
static_assert(!mirism::consteval_invokable<TestConstevalInvokable, 1, -2>, "failed");
static_assert(mirism::consteval_invokable<TestConstevalInvokable, 1, 2>, "failed");

// use `mirism::staticize` to convert a virtual function to a static member function, that is, you could safely call it
// without actually create an object. Of course, you should not use `this` in the function.
struct TestStaticizeBase
{
	virtual void f() const {std::cout << "base\n";}
};
struct TestStaticize : TestStaticizeBase
{
	virtual void f() const override {std::cout << "derived\n";}
};
void test_staticize()
{
	auto f1 = mirism::staticize<&TestStaticizeBase::f>;
	auto f2 = mirism::staticize<&TestStaticize::f>;
	f1();
	f2();
}

// use `mirism::CaseInsensitiveStringLess` to create a ordered container of strings with case insensitive comparison.
std::map<std::string, int, mirism::CaseInsensitiveStringLess> TestCaseInsensitiveStringLess;

int main()
{
	test_hash();
	test_unused();
	test_regex();
	test_fixed_string();
	test_operator_f();

}
