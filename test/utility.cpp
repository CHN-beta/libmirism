# include <mirism/utility.hpp>
using namespace mirism::literals;
using namespace mirism::stream_operators;

// atomic:
void test_atomic()
{
	// Use `Atomic` to safely access variables
	// common constructors and assignment operators are supported
	mirism::Atomic<int> a{3}, b = 5, c = b;
	// use `Atomic::get` or `Atomic::operator T()` to get the value
	b = b.get() * 2;
	c = static_cast<int>(c) + 3;

	// use `Atomic::apply` to apply a function to the value. Optionally, wait for finite or infinite time until a
	// condition function returns true.
	a.apply([](int& value){value *= 2;});	// return void
	a.apply([](int& value){value += 3;}, [](const int& value){return value > 5;});	// return void
	// return bool to indicate whether the condition is met or not.
	a.apply([](int& value){value += 3;}, [](const int& value){return value > 5;}, 3s);
	// the patch function could also return some value. In this case, the return type will be T or std::optional<T>,
	// indicating whether the condition is met or not. for example, this statement will return `std::optional<int>`:
	a.apply([](int& value){value += 3; return value;}, [](const int& value) {return value > 5;}, 3s);

	// use `Atomic::wait` to wait for a condition is met. Optionally, wait for at most a finite time, and return bool to
	// indicate whether the condition is met or not.
	a.wait([](const int& value){return value > 5;});	// wait until the condition is met, return void
	a.wait([](const int& value){return value > 5;}, 3s);	// wait for no more than 3 seconds, return bool

	// use Atomic::Guard to acquire lock in a scope.
	{
		auto locked_a = a.lock();
		// optionally, wait until a condition is met, and wait for at most for a finite time.
		// auto locked_a = a.lock([](const int& value) {return value > 10;});
		// here locked_a is with type std::optional<Atomic::Guard>
		// auto locked_a = a.lock([](const int& value) {return value > 10;}, 3s);

		// access value via `operator*()`, `operator->()` or `value()
		*locked_a = 8;
	}
}

// called_by:
// use CalledBy to make function only allow to be called from some class
struct test_called_by_1;
void test_called_by_2(int a, mirism::CalledBy<test_called_by_1>)
{
	std::cout << a << std::endl;
}
struct test_called_by_1
{
	static void call()
	{
		test_called_by_2(3, {});
	}
};

// common:
// Use `hash` to get hash of any number and any type of objects that is supported by `boost::hash_combine`
void test_common_hash()
{
	std::cout << mirism::hash(1, "wow"s, 3.14) << std::endl;
}

// Use `unused` to ignore any unused variable to avoid compiler warning
void test_common_unused()
{
	std::string s1;
	mirism::unused(s1);
}

// Use `uint128_t` to represent 128-bit unsigned integer
mirism::uint128_t test_common_u128 = 0;
static_assert(sizeof(test_common_u128) == 16, "uint128_t is not 16 bytes");

// Use `literals::operator""_re` to create an instance of `std::regex`
void test_common_regex()
{
	if (std::regex_match("hello", "[a-z]+"_re))
		std::cout << "match" << std::endl;
	else
		std::cout << "not match" << std::endl;
}

// Use `CaseInsensitiveStringLess` to create an ordered container of strings with case insensitive comparison
std::map<std::string, int, mirism::CaseInsensitiveStringLess> test_common_case_insensitive_string_less;

// Use `remove_member_pointer{,_t}` to get type that a member pointer points to. That is, for example, convert
// `T C::*` to `T`
struct test_common_remove_member_pointer
{
	int a;
};
static_assert(std::same_as<int,
	mirism::remove_member_pointer_t<decltype(&test_common_remove_member_pointer::a)>>, "failed");

// concepts:
// Use `decayed_type` to check if a type is a decayed type (that is, roughly speaking, not reference, const, volatile
// and array)
static_assert(mirism::decayed_type<int>, "failed");
static_assert(!mirism::decayed_type<int&>, "failed");
static_assert(!mirism::decayed_type<const int>, "failed");

// use `mirism::specialization_of` to test if a class is a specialization of a template.
// Only typename template parameter is supported.
// std::tuple<int, float, double> is a specialization of std::tuple
static_assert(mirism::specialization_of<std::tuple<int, float, double>, std::tuple>, "failed");
// std::tuple<int, float, double> is not a specialization of std::optional
static_assert(!mirism::specialization_of<std::tuple<int, float, double>, std::optional>, "failed");
// std::tuple<int, float, double> is a specialization of std::tuple, and first two template parameters are int and float
static_assert(mirism::specialization_of<std::tuple<int, float, double>, std::tuple, int, float>, "failed");
// do not support non-type template parameter
// static_assert(mirism::specialization_of<std::array<int, 3>, std::array, "failed");
// It is useful to announce that "I only accept the specialization of specific template as template parameter"
template<mirism::specialization_of<std::optional> T> struct i_only_accept_optional {};
template<mirism::specialization_of<std::tuple> T> struct i_only_accept_tuple {};

// use `mirism::complete_type` to check if a type is complete until now (that is, when the compiler reached this line).
// however, if you check both before and after a type get completed, there will be a wierd complile error.
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
struct TestConstevalInvokable {consteval void operator()(int a, int b) const {test_consteval_invokable(a, b);}};
static_assert(!mirism::consteval_invokable<TestConstevalInvokable, 1, -2>, "failed");
static_assert(mirism::consteval_invokable<TestConstevalInvokable, 1, 2>, "failed");

// use `enumarable` to check if a type is an enum type
enum class TestEnum {a, b, c};
static_assert(mirism::enumerable<TestEnum>, "failed");
static_assert(!mirism::enumerable<int>, "failed");

// format:
void test_format()
{
	// use `operator""_f` to call fmt::format
	auto str = "{} {}"_f("hello", "world");

	// std::varant could be formatted as well
	auto var = std::variant<int, std::string>{1};
	std::cout << "{}\n"_f(var);

	// use stream operator or format to print and read enum
	enum class Color {Red, Green, Blue};
	auto color = Color::Red;
	std::cout << "{}\n"_f(color);	// get "Red"
	std::cout << "{:f}\n"_f(color);	// get "main()::Color::Red"
	std::stringstream ss("Red");
	ss >> color;	// set color to Color::Red

	// smart pointers and std::optional could also be formatted
	std::optional<int> opt;
	str = "{}"_f(opt);	// get "(null)"
	opt = 42;
	str = "{}"_f(opt);	// get "(42)"
	std::optional<std::ostream*> opt_ostream;
	str = "{}"_f(opt_ostream);	// get "(null)"
	opt_ostream = &std::cout;
	str = "{}"_f(opt_ostream);	// get "(non-null unformattable value)"
	std::shared_ptr<int> ptr;
	str = "{}"_f(ptr);	// get "(null)"
	ptr = std::make_shared<int>(42);
	str = "{}"_f(ptr);	// get "(0x000000008f8f8f8f 42)"
}

// logger:
struct my_struct : public mirism::Logger::ObjectMonitor<my_struct> {};
int test_logger(int arg1, float arg2)
{
	// create a Logger::Guard at start of a function with parameters.
	// if log level >= Debug, some information will be print into the log, include function name, parameters, start and
	// return time, and so on.
	mirism::Logger::Guard log{arg1, arg2};

	// use Logger::Guard to log something.
	log.log<mirism::Logger::Level_t::Info>("{} {}"_f("hello", "world"));

	// use operator() to print a simple message, indicating the program reached here.
	// only log level >= Debug will be printed.
	log();

	// if telegram connection is initialized, log will be sent to telegram, in a sync or async way.
	mirism::Logger::telegram_notify_async("KFC crasy thursday v me 50");

	// use `rtn` to log the returned value.
	// only log level >= Debug will be printed.
	return log.rtn(3);
}

// smartref:
mirism::SmartRef<int> f1() {return 1;}	// store a copy of int
mirism::SmartRef<int> f2(const int& val) {return val;}	// ok, stores reference to val
mirism::SmartRef<std::string> f3(std::string&& val) {return std::move(val);}
	// ok, stores a std::string, that is moved from val
void test_smartref()
{
	std::cout << *f1() << *f2(1) << *f3("hello") << std::endl;
}

// string:
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

// a `BasicVariableString` is in a similar way to `BasicFixedString`, but with a variable size, at most N.
template <mirism::BasicVariableString<char, 16> S> struct TestBasicVariableString	// accept at most 16 characters
{
	TestBasicVariableString()
	{
		std::cout << "hi, you created " << S << std::endl;
	}
};
void test_basic_variable_string()
{
	// same as TestBasicVariableString<"hello"_fs> a, b, c;
	TestBasicVariableString<"hello"> a, b, c;	// wow, c++ now supports string as template parameter!
}

int main()
{
	test_atomic();

	// test_called_by_2(3, {}); error: no matching function for call to 'CalledBy_test2(int)'
	test_called_by_1::call();

	test_common_hash();
	test_common_unused();
	test_common_regex();
	test_format();
	test_logger(1, 1.1);
	test_smartref();
	test_basic_fixed_string();
	test_basic_variable_string();
}
