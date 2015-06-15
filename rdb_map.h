#ifndef __ML_RDB_MAP_H__
#define __ML_RDB_MAP_H__

#include <string>
#include "map.h"

namespace ml {
	template<class T> class Receiver;
	class Sql_value_spec;

class Rdb_map : public virtual Map {
public:
	using Map::remove;

	virtual ~Rdb_map() throw() {
	}
	virtual void remove(const Sql_value_spec& value_spec) = 0;
	virtual void get_values(const Sql_value_spec& value_spec,
		Receiver<std::string>& receiver) = 0;
};

} // ml

#endif // __ML_RDB_MAP_H__
