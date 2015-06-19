#ifndef __NOMAGIC_H__
#define __NOMAGIC_H__

#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <sys/wait.h>
#include <unistd.h>

/*
"namespace d" means that anything in the namespace d are the implementation
details.
*/

namespace nomagic {

namespace d {

inline int get_status(int stat_loc)
{
	int v(0);
	if (WIFSIGNALED(stat_loc))
		v = 1 << 8;
	else if (WIFEXITED(stat_loc))
		v = WEXITSTATUS(stat_loc);
	else
		v = 1 << 16;
	return v;
}

} // d

/* This function calls 'fn' in the child process.
   It outputs the message if the child process quits with the exit status
   other than 0. */
template<class F>
inline void run(const char* message, F fn)
{
	using std::cerr;
	using std::endl;
	using std::exit;

	pid_t pid(fork());
	if (pid == 0) {
		fn();
		exit(0);
	} else {
		int stat_loc;
		if (waitpid(pid, &stat_loc, 0) == pid) {
			int v(d::get_status(stat_loc));		
			if (v != 0)
				cerr << message << ":(" << v << ")" <<  endl;
		}
	}
}

namespace d {

template<class F>
class Fnwrapper {
public:
	Fnwrapper(const char* message, F f)
		:	m(message), f(f) {
	}

	void operator()() const {
		f(m);
	}

private:
	const char* m;
	F f;
};

template<class F>
inline Fnwrapper<F> fnwrapper(const char* message, F f)
{
	return Fnwrapper<F>(message, f);
}

} // d

/* This run function is different from the previous one.
   It calls 'fn' passing 'message' as the 1st parameter. */
inline void run(const char* message, void (fn)(const char*))
{
	run(message, d::fnwrapper(message, fn));
}

namespace d {

const char* const nullmsg = "(no message)";

} // d

/* This class outputs a message when it's destructor is called if
   there are any failures at assert methods. */
class Test {
public:
	typedef std::ostream_iterator<std::string> Os;

	Test(const char* message) {
		if (!message)
			message = d::nullmsg;
		main.assign(message);
	}

	~Test() throw() {
		try {
			if (mlist.empty())
				return;
			using std::cerr;
			cerr << main << std::endl;
			std::copy(mlist.begin(), mlist.end(), Os(cerr, "\n"));
		}
		catch (...) {
		}
	}

	// This is a Assert method.
	void a(bool expr, const char* message) {
		if (!expr) {
			if (!message)
				message = d::nullmsg;
			mlist.push_back(message);
		}
	}
private:
	std::string main;
	std::vector<std::string> mlist;
};

} // nomagic

#endif // __NOMAGIC_H__
