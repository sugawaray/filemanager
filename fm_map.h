#ifndef __FM_FM_MAP_H__
#define __FM_FM_MAP_H__

#include <utility>
#include "rdb_map.h"

namespace fm {

class Fm_map : public virtual ml::Rdb_map {
public:
	virtual ~Fm_map() throw() {
	}

	/* returns one of the following values:
		Type_dir, Type_file, Not_exist, Impossible */
	virtual int get_file_type(const std::string& path) = 0;
};

} // fm

#endif // __FM_FM_MAP_H__
