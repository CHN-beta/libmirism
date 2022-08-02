# include <mirism/detail_/misc/common.impl.hpp>
# include <mirism/detail_/misc/concepts.impl.hpp>
# include <mirism/detail_/misc/atomic.impl.hpp>
# include <mirism/detail_/misc/string.impl.hpp>
# include <mirism/detail_/misc/format.impl.hpp>

using namespace mirism::literals;
using namespace mirism::stream_operators;

int main()
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
