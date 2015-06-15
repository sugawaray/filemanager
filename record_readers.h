#ifndef __RECORD_READERS_H__
#define __RECORD_READERS_H__

#include <sqlite3.h>
#include "record_receiver.h"

namespace ml {
	namespace db {

template<class T, class U>
class Record_collector : public Record_receiver {
public:
	Record_collector(T inserter, const U& converter)
	:	inserter(inserter),
		converter(converter) {
	}
	bool receive(sqlite3_stmt* record) {
		*inserter = converter(record);
		++inserter;
		return true;
	}
private:
	T inserter;
	const U& converter;
};

template<class T, class U>
inline Record_collector<T, U> Record_collector_fun(T inserter,
	const U& converter)
{
	return Record_collector<T, U>(inserter, converter);
}

template<class T, class U>
class Record_reader : public Record_receiver {
public:
	Record_reader(T& target, const U& converter)
		:	target(target),
			converter(converter) {
	}
	bool receive(sqlite3_stmt* record) {
		target = converter(record);
		return false;
	}
private:
	T& target;
	const U& converter;
};

template<class T, class U>
inline Record_reader<T, U> Record_reader_fun(T& target, const U& converter)
{
	return Record_reader<T, U>(target, converter);
}

	} // db
} // ml

#endif // __RECORD_READERS_H__
