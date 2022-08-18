# include <mirism/detail_/misc/common.impl.hpp>
# include <mirism/detail_/misc/smartref.impl.hpp>

mirism::SmartRef<int> f1() {return 1;}	// store a copy of int
mirism::SmartRef<int> f2(const int& val) {return val;}	// ok, stores reference to val
mirism::SmartRef<std::string> f3(std::string&& val) {return std::move(val);}
	// ok, stores a std::string, that is moved from val

int main()
{
	std::cout << *f1() << *f2(1) << *f3("hello") << std::endl;
}
