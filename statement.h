#ifndef __STATEMENT_H__
#define __STATEMENT_H__

#include <string>
#include <sqlite3.h>

namespace ml {
	namespace db {

class Record_receiver;

class Statement {
public:
	Statement(sqlite3* db, const std::string& statement);
	Statement(Statement&& source);
	virtual ~Statement() throw();

	void bind_parameter(int one_based_index, int value);
	void bind_parameter(int one_based_index, const std::string& value);

	int execute();

	int execute(Record_receiver& receiver);

	/*
	 * public use of this function is not intended.
	 * this is here for testing purpose.
	 */
	static int (* finalize)(sqlite3_stmt*);
private:
	Statement(const Statement&) {
	}
	Statement& operator=(const Statement&) {
		return *this;
	}
	int execute(Record_receiver* receiver);
	bool step_and_receive(Record_receiver* receiver, sqlite3_stmt* record)
		const;
	sqlite3_stmt* statement_handle;
};

	} // db
} // ml

#endif // __STATEMENT_H__
