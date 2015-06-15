#ifndef __MAP_H__
#define __MAP_H__

#include <iterator>
#include <string>
#include <vector>

namespace ml {

class Map {
public:
	typedef std::vector<std::string>::const_iterator CIter;
	typedef std::back_insert_iterator<std::vector<std::string> >
		Inserter;
	virtual ~Map() throw() {
	}

	virtual void set(const std::string& target, CIter begin, CIter end) = 0;
	virtual void remove(const std::string& target) = 0;
	virtual void copy(const std::string& source,
		const std::string& destination) = 0;
	virtual void move(const std::string& source,
		const std::string& destination) = 0;
	virtual void get(const std::vector<std::string>& categories,
		Inserter inserter) = 0;
	virtual void get_categories(Inserter inserter) = 0;
	virtual void get_categories(const std::string& target,
		Inserter inserter) = 0;
};

} // ml

#endif // __MAP_H__
