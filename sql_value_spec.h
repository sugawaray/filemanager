#ifndef __SQL_VALUE_SPEC_H__
#define __SQL_VALUE_SPEC_H__

#include <string>

namespace ml {

class Sql_value_spec {
public:
	virtual ~Sql_value_spec() throw() {
	}
	virtual std::string get_like_argument() const throw() = 0;
};

class Sql_middle_value_spec : public Sql_value_spec {
public:
	Sql_middle_value_spec(const std::string& value);
	virtual ~Sql_middle_value_spec() throw() {
	}

	/* returns a string used as SQL like clause argument.
	   The string is %#%
	   where '#' is a string given at the constructor. */
	std::string get_like_argument() const throw();
private:
	std::string value;
};

class Sql_left_value_spec : public Sql_value_spec {
public:
	Sql_left_value_spec(const std::string& value);
	virtual ~Sql_left_value_spec() throw() {
	}

	/* returns a string used as SQL like clause argument.
	   The string is #%
	   where '#' is a string given at the constructor. */
	std::string get_like_argument() const throw();
private:
	std::string value;
};

} // ml

#endif // __SQL_VALUE_SPEC_H__
