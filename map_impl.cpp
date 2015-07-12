#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include "except.h"
#include "map_impl.h"
#include "receiver.h"
#include "record_readers.h"
#include "sql_value_spec.h"

namespace {

using namespace ml;
using namespace db;
using namespace std;

class Value_receiver : public Record_receiver {
public:
	Value_receiver(Receiver<string>& receiver)
		:	receiver(receiver) {
	}
	bool receive(sqlite3_stmt* record) {
		const unsigned char* p(sqlite3_column_text(record, 0));
		return receiver.receive(reinterpret_cast<const char*>(p));
	}
private:
	Receiver<string>& receiver;
};

} // unnamed

namespace ml {

using namespace db;

const char* Map_impl::delete_string("DELETE FROM map WHERE value = ?");

const char* Map_impl::delete_like_string("DELETE FROM map WHERE value like ?");

const char* Map_impl::insert_statement_string(
	"INSERT INTO map VALUES(?, ?, ?)");

const char* Map_impl::get_categories_string(
	"SELECT DISTINCT category FROM map");

const char* Map_impl::get_categories_for_value_string(
	"SELECT DISTINCT category FROM map WHERE value = ?");

const char* Map_impl::get_values_string(
	"SELECT DISTINCT value FROM map WHERE value LIKE ?");

const char* Map_impl::max_id_statement_string(
	"SELECT COALESCE(MAX(id), 1) AS id FROM map");

db::Statement Map_impl::prepare_statement(const string& statement)
{
	try {
		return Statement(db, statement);
	}
	catch (const Prepare_error& e) {
		create_table();
		return Statement(db, statement);
	}
}

Map_impl::Map_impl(const string& dbfilepath)
:	db(0),
	dbfilepath(dbfilepath)
{
	sqlite3_open(dbfilepath.c_str(), &db);
}

Map_impl::~Map_impl() throw()
{
}

void Map_impl::set(const string& target, Map::CIter begin, Map::CIter end)
{
	remove(target);
	if (!insert_statement)
		insert_statement.reset(new Statement(
			prepare_statement(insert_statement_string)));
	int max_id = get_max_id();
	for (int i = 0; begin != end; ++begin, ++i) {
		insert_statement->bind_parameter(1, max_id + 1 + i);
		insert_statement->bind_parameter(2, *begin);
		insert_statement->bind_parameter(3, target);
		insert_statement->execute();
	}
}

void Map_impl::remove(const string& target)
{
	if (!delete_statement)
		delete_statement.reset(new Statement(
			prepare_statement(delete_string)));
	delete_statement->bind_parameter(1, target);
	delete_statement->execute();
}

void Map_impl::remove(const Sql_value_spec& value_spec)
{
	if (!delete_like_statement)
		delete_like_statement.reset(new Statement(
			prepare_statement(delete_like_string)));
	delete_like_statement->bind_parameter(1,
		value_spec.get_like_argument());
	delete_like_statement->execute();
}

void Map_impl::copy(const string& src, const string& dest)
{
	vector<string> c;
	get_categories(src, back_inserter(c));
	remove(dest);
	if (c.empty())
		return;
	set(dest, c.begin(), c.end());
}

void Map_impl::move(const string& src, const string& dest)
{
	if (src == dest)
		return;
	copy(src, dest);
	remove(src);
}

void Map_impl::get(const vector<string>& categories,
	Map::Inserter inserter)
{
	ostringstream os;
	os << "SELECT DISTINCT t0.value FROM map AS t0 ";
	for (size_t i = 0; i < categories.size(); ++i) {
		os << "JOIN ( SELECT value FROM map WHERE category = ? ) " <<
			"AS t" << (i + 1) << " ON t" << i << ".value = " <<
			"t" << (i + 1) << ".value ";
	}
	Statement st(prepare_statement(os.str()));
	for (size_t i = 0; i < categories.size(); ++i)
		st.bind_parameter(static_cast<int>(i + 1), categories.at(i));

	auto collector(Record_collector_fun(inserter, convert_get_record));
	st.execute(collector);
}

void Map_impl::getall(const vector<string>& c, Map::Inserter ins)
{
}

namespace {

string column_string(sqlite3_stmt* record)
{
	return string(reinterpret_cast<const char*>(
		sqlite3_column_text(record, 0)));
}

} // unnamed

using placeholders::_1;

void Map_impl::get_categories(Map::Inserter inserter)
{
	if (!get_categories_statement)
		get_categories_statement.reset(new Statement(
			prepare_statement(get_categories_string)));

	auto collector(Record_collector_fun(inserter, bind(column_string, _1)));
	get_categories_statement->execute(collector);
}

void Map_impl::get_categories(const string& target, Map::Inserter ins)
{
	if (get_categories_for_value_statement == 0)
		get_categories_for_value_statement.reset(
			new Statement(prepare_statement(
				get_categories_for_value_string)));
	vector<string> categories;
	get_categories_for_value_statement->bind_parameter(1, target);
	auto collector(Record_collector_fun(back_inserter(categories),
		bind(column_string, _1)));
	get_categories_for_value_statement->execute(collector);
	std::copy(categories.begin(), categories.end(), ins);
}

void Map_impl::get_values(const Sql_value_spec& value_spec,
	Receiver<string>& receiver)
{
	if (!get_values_statement)
		get_values_statement.reset(new Statement(
			prepare_statement(get_values_string)));
	get_values_statement->bind_parameter(1, value_spec.get_like_argument());
	
	Value_receiver adapter(receiver);
	get_values_statement->execute(adapter);
}

void Map_impl::create_table()
{
	Statement st(db, 
		"CREATE TABLE map ("
			"id DECIMAL(8), "
			"category CHARACTER VARYING(512), "
			"value CHARACTER VARYING(1024), "
			"PRIMARY KEY(id))");
	st.execute();
}

int Map_impl::get_max_id()
{
	if (!max_id_statement)
		max_id_statement.reset(new Statement(
			prepare_statement(max_id_statement_string)));
	int id;
	auto reader(Record_reader_fun(id, bind(sqlite3_column_int, _1, 0)));
	max_id_statement->execute(reader);
	return id;
}

string Map_impl::convert_get_record(sqlite3_stmt* record)
{
	return string(reinterpret_cast<const char*>(
		sqlite3_column_text(record, 0)));
}

} // ml
