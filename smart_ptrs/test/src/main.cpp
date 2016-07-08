#define BOOST_TEST_MODULE UNIQ_PTR_TESTS
#include <boost/test/unit_test.hpp>
#include <smart_ptr/unique_ptr.hpp>
#include <iostream>

namespace {
template <typename T>
class test_deleter {
public:
	test_deleter() : free_count(0), final_free_count(-1) {}

	~test_deleter() {
		if ( 0 <= final_free_count) assert(free_count == final_free_count);
	}
	void operator()(T* p) {
		d(p);
		free_count++;
	}

	test_deleter<T>& operator=(const test_deleter<T>& other) {
		if ( 0 <= final_free_count) assert(free_count == final_free_count);
		final_free_count = other.final_free_count;
		free_count = other.free_count;
	}

	void set_final_free_count(int fc) {
		final_free_count = fc;
	}

	void reset_free_count() {
		free_count = 0;
	}

	int get_free_count() {
		return free_count;
	}
private:
	sagar::default_deleter<T> d;
	int free_count, final_free_count;
};

typedef test_deleter<int> int_deleter;

class foo {
public:
	foo() : bar(0) {}
	void set_bar(int b) {bar = b;}
	int get_bar() {return bar;}
private:
	int bar;
};
}

BOOST_AUTO_TEST_CASE(SimpleTest) {
	int* p = new int{10};
	{
		sagar::unique_ptr<int, int_deleter> u(p);
		u.get_deleter().set_final_free_count(1);
	}
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(GetVerify) {
	int* p = new int{10};
	sagar::unique_ptr<int> u(p);
	BOOST_CHECK_EQUAL(u.get(), p);
}

BOOST_AUTO_TEST_CASE(ReleaseVerify) {
	{
		sagar::unique_ptr<int, int_deleter> u(new int{10});
		u.get_deleter().set_final_free_count(0);
		delete u.release();
	}
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(ResetVerify) {
	sagar::unique_ptr<int, int_deleter> u(new int{10});
	int_deleter& d = u.get_deleter();
	d.reset_free_count();
	d.set_final_free_count(2);
	u.reset(new int{20});
	BOOST_CHECK_EQUAL(d.get_free_count(), 1);
}

BOOST_AUTO_TEST_CASE(PointerOpsVerify) {
	sagar::unique_ptr<foo> u(new foo());
	(*u).set_bar(10);
	BOOST_CHECK_EQUAL(10, u->get_bar());
}

BOOST_AUTO_TEST_CASE(BoolTestVerify) {
	sagar::unique_ptr<int> u{};
	if (!u) BOOST_CHECK(true);
	u.reset(new int{0});
	if (u) BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(AssignVerify) {
	sagar::unique_ptr<int, int_deleter> u{new int{10}};
	sagar::unique_ptr<int, int_deleter> v{new int{20}};
	u.get_deleter().set_final_free_count(1);
	v.get_deleter().set_final_free_count(0);
	u = v;
	u.get_deleter().set_final_free_count(1);
	BOOST_CHECK_EQUAL(20, *u);
}
