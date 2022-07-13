# include <mirism/detail_/common.impl.hpp>
# include <iostream>
using namespace mirism::literals;

template <mirism::FixedString S> void print_str()
{
	std::cout << S << std::endl;
}

int main()
{
	std::cout << mirism::hash("abcd"s, 1, 2, 3) << std::endl;
	std::regex reg = "abcd"_re;
	std::optional<decltype("abcd"_ss)> str;
	print_str<"abcd">();
}
