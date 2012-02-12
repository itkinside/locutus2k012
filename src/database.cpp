#include "database.h"

#include <libpq-fe.h>
#include <stdio.h>
#include <string.h>

class Database {
	private:
		PGconn *connection;
	public:
		~Database();
		void abort();
		void connect(char*);
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

int Database::file_exists(struct track_info *our_file)
{
	if (connection == 0) return 0;

	PGresult *fingerprint_result;
	const char *parameters[1];
	parameters[0] = our_file->fingerprint;

	fingerprint_result = PQexecPrepared(connection, "file_exists", 1, parameters, 0, 0, 0);
	if (PQresultStatus(fingerprint_result) != PGRES_TUPLES_OK) {
		printf("Unable to check for fingerprint in database!");
		return 0;
	}
	if (PQntuples(fingerprint_result) == 0) { //fingerprint not found
		PQclear(fingerprint_result);
		return 0;
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
		return 1;
	}
}

void Database::terminate()
{
	if (connection == 0) return;
	PQfinish(connection);
}
