#ifndef __FM_FILESYSTEM_H__
#define __FM_FILESYSTEM_H__

#include <stdexcept>
#include <string>
#include "absolute_path.h"

namespace fm {
namespace fs {

extern int find_fm_dir(const ::fs::Absolute_path& path, int limit,
	::fs::Absolute_path& result);

/* returns a converted path for the specified path which the system internally
   uses. Throws std::invalid_argument if the specified path isn't
   a descendant of fm filesystem. */
extern std::string get_fm_path(const ::fs::Absolute_path& fm_dirpath,
	const ::fs::Absolute_path& path) throw(std::invalid_argument);

/* convert fm_path to an absolute path based on the specified fm directory. */
extern ::fs::Absolute_path convert_fm_path(
	const ::fs::Absolute_path& fm_dirpath, const std::string& fm_path)
		throw(std::invalid_argument);

class Filesystem {
public:
	Filesystem(const ::fs::Absolute_path& fm_dir)
	:	fm_dir(fm_dir) {
	}

	bool is_descendant(const ::fs::Absolute_path& path) const;

	/* Returns the path relative to the parent directory of the .fm
	   directory. Throws an exception if the specified path is not under
	   the parent directory of the .fm directory.
	   The argument path should be specified by an absolute path. */
	std::string get_fm_path(const ::fs::Absolute_path& path)
		throw(std::invalid_argument) {
		return fs::get_fm_path(fm_dir, path);
	}

	::fs::Absolute_path convert_fm_path(const std::string& fm_path)
		throw(std::invalid_argument) {
		return fs::convert_fm_path(fm_dir, fm_path);
	}

	std::string get_dbfilepath() const;
private:
	::fs::Absolute_path fm_dir;
};

} // fs
} // fm

#endif // __FM_FILESYSTEM_H__
