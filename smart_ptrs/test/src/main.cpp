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
		std::cout << "deleting " <<  p << std::endl;
		d(p);
		free_count++;
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
