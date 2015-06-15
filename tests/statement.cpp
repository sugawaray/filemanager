#include <cstdio>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>
#include <tests/fixture/db.h>
#include "statement.h"
#include <except.h>
#include <record_receiver.h>
#include <statement.h>

using namespace std;
using namespace ml::db;

namespace {

class Result_receiver : public Record_receiver {
public:
	typedef back_insert_iterator<
		vector<pair<int, string> > > Inserter;
	static const int default_max = 100;

	Result_receiver(Inserter inserter)
		:	inserter(inserter),
			max(default_max),
			i(0) {
	}
	Result_receiver(Inserter inserter, int max)
		:	inserter(inserter),
			max(max),
			i(0) {
	}
	bool receive(sqlite3_stmt* record) {
		string tmp(reinterpret_cast<const char*>(
			sqlite3_column_text(record, 1)));
		*inserter = make_pair(sqlite3_column_int(record, 0), tmp);
		++inserter;
		++i;
		return i < max;
	}
private:
	Inserter inserter;
	int max;
	int i;
};

class Fixture {
public:
	Fixture() {
		remove(fdb.get_dbfilepath().c_str());
		int r = sqlite3_open(fdb.get_dbfilepath().c_str(), &db_handle);
		if (r != SQLITE_OK)
			throw logic_error("db_open");
		Statement::finalize = finalize_spy;
	}
	~Fixture() {
		sqlite3_close(db_handle);
	}
	sqlite3* get_db_handle() {
		return db_handle;
	}
	static int finalize_spy(sqlite3_stmt* statement) {
		++call_count_for_finalize;
		return 0;
	}
	static int call_count_for_finalize;
private:
	test::Db_fixture fdb;
	sqlite3* db_handle;
};
int Fixture::call_count_for_finalize(0);

Statement create_create_table_statement(Fixture& f)
{
	return Statement(f.get_db_handle(),
		"CREATE TABLE table1 ("
			"id INTEGER, "
			"s_value CHARACTER VARYING(512), "
			"PRIMARY KEY(id))");
}

Statement create_insert_record_statement(Fixture& f, int v1,
	const string& v2)
{
	Statement st(f.get_db_handle(), "INSERT INTO table1 VALUES(?, ?)");
	st.bind_parameter(1, v1);
	st.bind_parameter(2, v2);
	return st;
}

string get_select_statement_string()
{
	return "SELECT id, s_value FROM table1 ";
}

Statement create_select_statement(Fixture& f)
{
	return Statement(f.get_db_handle(), get_select_statement_string());
}

void prepare_table(Fixture& f)
{
	Statement(create_create_table_statement(f)).execute();
}

} // unnamed

START_TEST(should_execute_statement)
{
	Fixture f;
	Statement statement1(create_create_table_statement(f));
	int r1 = statement1.execute();
	Statement statement2(create_insert_record_statement(f, 1, "abc"));
	int r2 = statement2.execute();

	fail_unless(r1 == 0, "statement should return 0 for success.");
	fail_unless(r2 == 0, "statement should execute statement.");
}
END_TEST

START_TEST(should_return_records)
{
	Fixture f;
	prepare_table(f);
	Statement(create_insert_record_statement(f, 1, "abc")).execute();
	Statement(create_insert_record_statement(f, 2, "def")).execute();

	Statement statement1(create_select_statement(f));
	vector<pair<int, string> > results;
	Result_receiver receiver(back_inserter(results));
	int r = statement1.execute(receiver);

	fail_unless(r == 0, "result value should be 0.");
	fail_unless(results.size() == 2, "should return records.");
	fail_unless(results.at(0).first == 1, "content should match.");
	fail_unless(results.at(0).second == "abc", "content should match.");
	fail_unless(results.at(1).first == 2, "content should match.");
	fail_unless(results.at(1).second == "def", "content should match.");
}
END_TEST

START_TEST(should_return_records_until_false)
{
	Fixture f;
	prepare_table(f);
	Statement(create_insert_record_statement(f, 1, "abc")).execute();
	Statement(create_insert_record_statement(f, 2, "def")).execute();
	Statement(create_insert_record_statement(f, 3, "ghi")).execute();

	Statement statement1(create_select_statement(f));
	vector<pair<int, string> > results;
	Result_receiver receiver(back_inserter(results), 2);
	statement1.execute(receiver);

	fail_unless(results.size() == 2, "limit");
}
END_TEST

START_TEST(should_be_able_to_execute_multiple_times)
{
	Fixture f;
	prepare_table(f);
	Statement(create_insert_record_statement(f, 1, "abc")).execute();
	Statement(create_insert_record_statement(f, 2, "def")).execute();
	Statement st(f.get_db_handle(),
		get_select_statement_string() + "WHERE id = ?");
	st.bind_parameter(1, 1);
	vector<pair<int, string> > results;
	Result_receiver receiver(back_inserter(results));
	st.execute(receiver);
	st.bind_parameter(1, 2);
	st.execute(receiver);

	fail_unless(results.at(0).second == "abc", "1");
	fail_unless(results.at(1).second == "def", "2");
}
END_TEST

START_TEST(should_finalize_statement)
{
	Fixture f;
	prepare_table(f);
	{
		Statement st(create_insert_record_statement(f, 1, "abc"));
		st.execute();
		f.call_count_for_finalize = 0;
	}
	fail_unless(f.call_count_for_finalize == 1, "count");
}
END_TEST

START_TEST(should_use_move_constructor)
{
	Fixture f;
	prepare_table(f);
	f.call_count_for_finalize = 0;
	{
		Statement st1(create_insert_record_statement(f, 1, "abc"));
		Statement st2(move(st1));
	}
	fail_unless(f.call_count_for_finalize == 1, "count");
}
END_TEST

START_TEST(should_throw_exception_when_unable_to_prepare)
{
	Fixture f;
	try {
		Statement(0, "SELECT * FROM table1");
		throw logic_error("shouldn't reach here.");
	}
	catch (const Prepare_error& e) {
	}
}
END_TEST

Suite* create_statement_test_suite()
{
	Suite* suite = suite_create("Statement");
	TCase* tcase = tcase_create("core");
	tcase_add_test(tcase, should_execute_statement);
	tcase_add_test(tcase, should_return_records);
	tcase_add_test(tcase, should_return_records_until_false);
	tcase_add_test(tcase, should_be_able_to_execute_multiple_times);
	tcase_add_test(tcase, should_finalize_statement);
	tcase_add_test(tcase, should_use_move_constructor);
	tcase_add_test(tcase, should_throw_exception_when_unable_to_prepare);
	suite_add_tcase(suite, tcase);
	return suite;
}
