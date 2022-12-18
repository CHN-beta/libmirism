# include <mirism/detail_/misc/common.impl.hpp>
# include <mirism/detail_/misc/called_by.hpp>

// use CalledBy to make function only allow to be called from some class (even indirectly)
class A;
void func1(int a, mirism::CalledBy<A>)
{
	std::cout << a;
}
struct A
{
	static void call()
	{
		func1(3, {});
	}
};

int main()
{
	// func1(3, {}); does not work
	A::call();
}
