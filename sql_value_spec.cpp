#include "sql_value_spec.h"

using namespace std;

namespace ml {

Sql_middle_value_spec::Sql_middle_value_spec(const string& value)
:	value(value)
{
}

string Sql_middle_value_spec::get_like_argument() const throw()
{
	return "%" + value + "%";
}

Sql_left_value_spec::Sql_left_value_spec(const string& value)
:	value(value)
{
}

string Sql_left_value_spec::get_like_argument() const throw()
{
	return value + "%";
}

} // ml
