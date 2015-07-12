#ifndef __MAP_IMPL_H__
#define __MAP_IMPL_H__

#include <memory>
#include <sqlite3.h>
#include "rdb_map.h"
#include "statement.h"

namespace ml {
	namespace db {
		class Statement;
	} // db

class Map_impl : public virtual Rdb_map {
public:
	Map_impl(const std::string& dbfilepath);
	~Map_impl() throw();

	void set(const std::string& target, CIter begin, CIter end);
	void remove(const std::string& target);
	void remove(const Sql_value_spec& value_spec);
	void copy(const std::string& source, const std::string& destination);
	void move(const std::string& source, const std::string& destination);
	void get(const std::vector<std::string>& categories, Inserter inserter);
	void getall(const std::vector<std::string>& categories,
		Inserter inserter);
	void get_categories(Inserter inserter);
	void get_categories(const std::string& target, Inserter inserter);
	void get_values(const Sql_value_spec& value_spec,
		Receiver<std::string>& receiver);
private:
	void create_table();
	int get_max_id();
	db::Statement prepare_statement(const std::string& statement);
	static std::string convert_get_record(sqlite3_stmt* record);

	sqlite3* db;
	std::unique_ptr<db::Statement> delete_statement;
	std::unique_ptr<db::Statement> delete_like_statement;
	std::unique_ptr<db::Statement> get_categories_statement;
	std::unique_ptr<db::Statement> get_categories_for_value_statement;
	std::unique_ptr<db::Statement> get_values_statement;
	std::unique_ptr<db::Statement> insert_statement;
	std::unique_ptr<db::Statement> max_id_statement;
	std::string dbfilepath;
	static const char* delete_string;
	static const char* delete_like_string;
	static const char* insert_statement_string;
	static const char* get_categories_string;
	static const char* get_categories_for_value_string;
	static const char* get_values_string;
	static const char* max_id_statement_string;
};

}

#endif // __MAP_IMPL_H__
