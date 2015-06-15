#ifndef __FS_ABSOLUTE_PATH_H__
#define __FS_ABSOLUTE_PATH_H__

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include "except.h"

namespace fs {

class Absolute_path {
public:
	Absolute_path();
	Absolute_path(const std::string& stringrep);
	Absolute_path(const char* stringrep);

	template<class Inserter>
	void get_components(Inserter inserter) const throw();

	std::string to_string() const throw();

	/* returns a string representation without a tailing slash when
	   the path is not the root or returns "/" when the path is the
	   root.  */
	std::string to_filepath_string() const throw();

	Absolute_path parent() const throw(std::runtime_error, Invalid_path);
	Absolute_path child(const std::string& component) const;
	std::string basename() const throw(std::runtime_error);
private:
	typedef std::vector<std::string>::const_iterator CIter;
	Absolute_path(CIter first, CIter last);
	void init(const std::string& stringrep);

	void throw_when_invalid() const;

	class Is_invalid;
	std::vector<std::string> comps;	// components
};

inline void Absolute_path::throw_when_invalid() const
{
	if (comps.empty())
		throw std::runtime_error("fs::Absolute_path");
}

template<class Inserter>
inline void Absolute_path::get_components(Inserter ins) const throw()
{
	throw_when_invalid();
	std::copy(comps.begin(), comps.end(), ins);
}

} // fs

#endif // __FS_ABSOLUTE_PATH_H__
