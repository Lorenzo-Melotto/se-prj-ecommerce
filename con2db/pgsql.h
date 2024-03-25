#ifndef pgsql_h
#define pgsql_h

#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

class Con2DB {
private:
 	PGconn *conn;

public:
	// connect to DB
	Con2DB(const char *hostname,
		   const char *port,
		   const char *username,
		   const char *password,
		   const char *dbname);

	/* use this for commands returning no data, e.g. INSERT, UPDATE */
	PGresult* ExecSQLcmd(char *sqlcmd);
	/* use this for commands returning data, e.g. SELECT */
	PGresult* ExecSQLtuples(char *sqlcmd);
	/* disconnect from DB */
	void finish();
	void close_connection();
};

#endif 
