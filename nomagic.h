#ifndef __NOMAGIC_H__
#define __NOMAGIC_H__

#include <cstdlib>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

namespace nomagic {

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
			int v(get_status(stat_loc));		
			if (v != 0)
				cerr << message << ":(" << v << ")" <<  endl;
		}
	}
}

} // nomagic

#endif // __NOMAGIC_H__
