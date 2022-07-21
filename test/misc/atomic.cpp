# include <mirism/detail_/misc/common.impl.hpp>
# include <mirism/detail_/misc/concepts.impl.hpp>
# include <mirism/detail_/misc/atomic.impl.hpp>

using namespace mirism::literals;

int main()
{
	// Use `Atomic` to safely access variables
	// common constructors and assignment operators are supported
	mirism::Atomic<int> a(3), b = 5, c = b;
	// use `Atomic::get` or `Atomic::operator T()` to get the value
	b = b.get() * 2;
	c = (int)c + 3;

	// use `Atomic::apply` to apply a function to the value. Optionally, wait for finite or infinite time until a
	// condition function returns true.
	a.apply([](int& value) {value *= 2;});	// return void
	a.apply([](int& value) {value += 3;}, [](const int& value) {return value > 10;});	// return void
	// return bool to indicate whether the condition is met or not.
	a.apply([](int& value) {value += 3;}, [](const int& value) {return value > 10;}, 3s);
	// the patch function could also return some value. In this case, the return type will be T or std::optional<T>,
	// indicating whether the condition is met or not. for example, this statement will return `std::optional<int>`:
	a.apply([](int& value) {value += 3; return value;}, [](const int& value) {return value > 10;}, 3s);

	// use `Atomic::wait` to wait for a condition is met. Optionally, wait for at most a finite time, and return bool to
	// indicate whether the condition is met or not.
	a.wait([](const int& value) {return value > 10;});	// wait until the condition is met, return void
	a.wait([](const int& value) {return value > 10;}, 3s);	// wait for no more than 3 seconds, return bool

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
