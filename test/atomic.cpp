# include <mirism/atomic.impl.hpp>
# include <iostream>

int main()
{
	mirism::Atomic<int> a(0);
	a = 3;
	std::cout << a.read() << std::endl;
}