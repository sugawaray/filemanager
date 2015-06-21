#ifndef __FM_H__
#define __FM_H__

#include <iostream>

namespace fs {
	class Absolute_path;
} // fs

namespace fm {
	using ::fs::Absolute_path;

extern std::istream* fmcin;
extern std::ostream* fmcout;
extern std::ostream* fmcerr;

const char* const db_filepath("./.fm/map");

enum Result {
	Success,	Failure,	Found,	Not_found,	Not_exist,
	Type_file,	Type_dir,	Impossible,	Not_empty,
	Not_managed
};

} // fm

#endif // __FM_H__
