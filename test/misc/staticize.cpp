# include <mirism/detail_/misc/common.impl.hpp>
# include <mirism/detail_/misc/staticize.impl.hpp>

struct Base
{
	virtual void f() const
	{
		std::cout << "Base::f()\n";
	}
};
struct Derived : Base
{
	virtual void f() const override
	{
		std::cout << "Derived::f()\n";
	}
};

int main()
{
	// use `staticize` to get actual pointer of a const virtual function and call it without actually constructing an
	// object. it is users responsibility to ensure `this` is not used in the virtual function
	auto f1 = mirism::staticize<&Base::f>(), f2 = mirism::staticize<&Derived::f>();
	f1();
	f2();
}
