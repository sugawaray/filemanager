#ifndef __FS_PATH_H__
#define __FS_PATH_H__

#include <deque>
#include <string>
#include "absolute_path.h"

namespace fs {

class Path {
public:
	Path(const std::string& stringrep);

	bool is_absolute() const throw();
	std::string stringrep() const throw();

	Absolute_path to_absolute_path(const Absolute_path& current_dir) const;
private:
	Path() {
	}

	void validate();

	std::string s;
	std::deque<std::string> seq;
};

} // fs

#endif // __FS_PATH_H__
