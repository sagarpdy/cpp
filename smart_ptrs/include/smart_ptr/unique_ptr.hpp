namespace sagar {

template <typename T>
class default_deleter {
	public:
	void operator()(T* p) {
		delete p;
	}
};

template <typename T, class D=default_deleter<T>>
class unique_ptr {
public:
	typedef T* pointer;

	unique_ptr() : mP(pointer()) {}
	unique_ptr(pointer p) : mP(p){}

	~unique_ptr() {
		if (mP) deleter(mP);
		mP = pointer();
	}

	pointer get() {
		return mP;
	}

	pointer release() {
		pointer r(mP);
		mP = pointer();
		return r;
	}

	void reset(pointer p = pointer()) {
		if (mP) deleter(mP);
		mP = p;
	}

	D& get_deleter() {
		return deleter;
	}

	T& operator*() {
		return *get();
	}

	T* operator->() {
		return get();
	}

	operator bool() {
		return (get() != pointer());
	}

	unique_ptr<T,D>& operator=(unique_ptr<T,D>& other) {
		reset(other.release());
		deleter = other.get_deleter();
		return *this;
	}
private:
	pointer mP;
	D deleter;
};
};
