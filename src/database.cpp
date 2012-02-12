#include "database.h"

#include <libpq-fe.h>
#include <stdio.h>

class Database {
	private:
		PGconn *connection = 0;
	public:
		~Database();
		void abort();
		void connect(char*);
		int file_exists(char*);
		void terminate();
}


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

int Database::file_exists(char *fingerprint)
{
	if (connection == 0) return 0;

	PGresult *fingerprint_result;

	fingerprint_result = PQexecPrepared(connection, "file_exists", 1, &fingerprint, 0, 0, 0);
	if (PQresultStatus(fingerprint_result) != PGRES_TUPLES_OK) {
		printf("Unable to check for fingerprint in database!");
		return 0;
	}
	if (PQntuples(fingerprint_result) == 0) { //fingerprint not found
		PQclear(fingerprint_result);
		return 0;
	}
	else { //fingerprint found
		PQclear(fingerprint_result);
		return 1;
	}
}

void Database::terminate()
{
	if (connection == 0) return;
	PQfinish(connection);
}
