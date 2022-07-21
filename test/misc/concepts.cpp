# include <mirism/detail_/misc/common.impl.hpp>
# include <mirism/detail_/misc/concepts.impl.hpp>

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

int main() {}
