#ifndef __UTILS_UTILS_H__
#define __UTILS_UTILS_H__

namespace utils {

template<class F>
class Auto_caller_class {
public:
	explicit Auto_caller_class(F f)
		:	f(f),
			valid(true) {
	}
	Auto_caller_class(Auto_caller_class&& op)
		:	f(op.f),
			valid(op.valid) {
		op.valid = false;
	}
	Auto_caller_class& operator=(Auto_caller_class&& op) {
		f = op.f;
		valid = op.valid;
		op.valid = false;
		return *this;
	}
	~Auto_caller_class() throw() {
		try {
			if (valid)
				f();
		}
		catch (...) {
		}
	}
private:
	Auto_caller_class(const Auto_caller_class&) {
	}
	Auto_caller_class& operator=(const Auto_caller_class&) {
		return *this;
	}

	F f;
	bool valid;
};

template<class F>
inline Auto_caller_class<F> Auto_caller(F f)
{
	return Auto_caller_class<F>(std::move(f));
}

} // utils

#endif // __UTILS_UTILS_H__
