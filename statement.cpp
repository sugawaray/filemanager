#include <sqlite3.h>
#include "except.h"
#include "record_receiver.h"
#include "statement.h"

using std::string;
using namespace ml::db;

int (* Statement::finalize)(sqlite3_stmt*) = sqlite3_finalize;

Statement::Statement(sqlite3* db_handle, const string& statement)
{
	if (sqlite3_prepare_v2(db_handle, statement.c_str(),
		statement.length(), &statement_handle, 0) != SQLITE_OK)
		throw Prepare_error();
}

Statement::Statement(Statement&& source)
{
	statement_handle = source.statement_handle;
	source.statement_handle = 0;
}

Statement::~Statement() throw()
{
	if (statement_handle != 0)
		finalize(statement_handle);
}

void Statement::bind_parameter(int i, int value)
{
	sqlite3_bind_int(statement_handle, i, value);
}

void Statement::bind_parameter(int i, const string& value)
{
	sqlite3_bind_text(statement_handle, i, value.c_str(), -1,
		SQLITE_TRANSIENT);
}

int Statement::execute()
{
	return execute(0);
}

int Statement::execute(Record_receiver& receiver)
{
	return execute(&receiver);
}

inline bool Statement::step_and_receive(Record_receiver* receiver,
	sqlite3_stmt* record) const
{
	return	sqlite3_step(record) == SQLITE_ROW &&
		receiver != 0 &&
		receiver->receive(statement_handle);
}

int Statement::execute(Record_receiver* receiver)
{
	for ( ; step_and_receive(receiver, statement_handle); )
		;
	sqlite3_reset(statement_handle);
	return 0;
}
