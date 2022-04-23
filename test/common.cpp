# include <mirism/common.impl.hpp>
# include <iostream>
using namespace mirism::literals;

int main()
{
	std::cout << mirism::hash("abcd"s, 1, 2, 3) << std::endl;
	std::regex reg = "abcd"_re;
	std::optional<decltype("abcd"_ss)> str;
}
