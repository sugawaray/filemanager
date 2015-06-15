#ifndef __RECORD_RECEIVER_H__
#define __RECORD_RECEIVER_H__

#include <sqlite3.h>

namespace ml {
	namespace db {

class Record_receiver {
public:
	virtual ~Record_receiver() {}
	virtual bool receive(sqlite3_stmt* record) = 0;
};

	} // db
} // ml

#endif // __RECORD_RECEIVER_H__
