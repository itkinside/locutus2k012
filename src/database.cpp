#include "database.h"

#include <libpq-fe.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

class Database {
	private:
		PGconn *connection;
	public:
		~Database();
		void abort();
		void connect(char*);
		int execute_statement(char*, int, ...);
		int file_exists(struct track_info*);
		void terminate();
};


Database::~Database()
{
	Database::terminate();
}

void Database::abort()
{
	PQfinish(connection);
	connection = 0;
}

void Database::connect(char *db_connection_string)
{
	connection = PQconnectdb(db_connection_string);
	if (PQstatus(connection) != CONNECTION_OK) {
		printf("Unable to connect to database!\n");
		Database::abort();
		return;
	}

	PGresult *result;

	result = PQprepare(connection, "file_exists", "SELECT * FROM file WHERE fingerprint = ?", 0, 0);
	if (PQresultStatus(result) != PGRES_COMMAND_OK) {
		PQclear(result);
		printf("Unable to create prepared function (file_exists)!");
		Database::abort();
		return;
	}
	PQclear(result);

	printf("Database connected\n");
}

int Database::execute_statement(char *statement, int argc, ...)
{
	if (connection == 0) return 1;

	PGresult *result;

	const char *parameters[argc];
	int lengths[argc];
	int binary[argc];

	va_list arguments;
	va_start(arguments, argc);
	for (int i = 0; i < argc; i++) {
		parameters[i] = va_arg(arguments, char*);
		lengths[i] = strlen(parameters[i]);
		binary[i] = 0;
	}
	va_end(arguments);

	result = PQexecParams(connection, statement, argc, 0, parameters, lengths, binary, 0);
	if (PQresultStatus(result) != PGRES_COMMAND_OK) {
		printf("Unable to execute statement %s!\n", statement);
		return 1;
	}

	return 0;
}


int Database::file_exists(struct track_info *our_file)
{
	if (connection == 0) return 1;

	PGresult *fingerprint_result;
	const char *parameters[1];
	parameters[0] = our_file->fingerprint;

	fingerprint_result = PQexecPrepared(connection, "file_exists", 1, parameters, 0, 0, 0);
	if (PQresultStatus(fingerprint_result) != PGRES_TUPLES_OK) {
		printf("Unable to check for fingerprint in database!");
		return 1;
	}
	if (PQntuples(fingerprint_result) == 0) { //fingerprint not found
		PQclear(fingerprint_result);
		return 1;
	}
	else { //fingerprint found
		int c_num;
		char *value;
		c_num = PQfnumber(fingerprint_result, "filename");
		value = PQgetvalue(fingerprint_result, 0, c_num);
		strcpy((*our_file).filename, value);
		c_num = PQfnumber(fingerprint_result, "location");
		value = PQgetvalue(fingerprint_result, 0, c_num);
		strcpy((*our_file).location, value);
		c_num = PQfnumber(fingerprint_result, "extension");
		value = PQgetvalue(fingerprint_result, 0, c_num);
		strcpy((*our_file).extension, value);
		PQclear(fingerprint_result);
		return 0;
	}
}

void Database::terminate()
{
	if (connection == 0) return;
	PQfinish(connection);
}
