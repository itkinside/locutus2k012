#include "database.h"

#include <libpq-fe.h>
#include <stdio.h>

class Database {
	private:
		PGconn *connection = 0;
	public:
		void abort(void);
		void connect(void);
		void terminate(void);
}
	
void Database::abort(void)
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

	printf("Database connected\n");
}

void Database::terminate(void)
{
	if (connection == 0) return;
	PQfinish(connection);
}
