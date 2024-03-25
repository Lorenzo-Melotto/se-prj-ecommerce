#include "pgsql.h"

#define DEBUG 0

Con2DB::Con2DB(const char *hostname, const char *port, const char *username,
			   const char *password, const char *dbname) {
	char connString[1000];
	sprintf(connString, "host=%s port=%s user=%s password=%s dbname=%s",
			hostname,
			port,
			username,
			password,
			dbname);
  
  	// connect linux user to database (must have privileges)
    conn = PQconnectdb(connString);

    // check if connected succesfully
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Con2DB(%s): Connection to database failed: %s\n",
		        dbname,
		        PQerrorMessage(conn));
        finish();
    }

#if 0
    fprintf(stderr, "Con2DB(%s): PQconnectPoll: %d\n", dbname, PQconnectPoll(conn));        
    fprintf(stderr, "Con2DB(%s): PQstatus: %d\n", dbname, PQstatus(conn));       
    fprintf(stderr, "Con2DB(%s): Connection to DB successfully established \n", dbname);        
#endif
}

/**
 * @brief Closes DB connection and exits progam
 */
void Con2DB::finish() {
    PQfinish(conn);
    conn = NULL;
    exit(1);
}

/**
 * @brief Closes DB connection
 */
void Con2DB::close_connection() {
    PQfinish(conn);
    conn = NULL;
}

/**
 * @brief Execute query on DB. Used for query that return nothing.
 *
 * @param sqlCmd the SQL query
 */
PGresult* Con2DB::ExecSQLcmd(char *sqlCmd) {
	if (conn == NULL) {
		fprintf(stderr, "ExecSQLcmd(): no connection to DB: PQexec on: %s\n", sqlCmd);
        exit (1);
    }
  
#if (DEBUG > 0)
	fprintf(stderr, "ExecSQLcmd(): PQexec on: %s\n", sqlCmd);        
	fprintf(stderr, "ExecSQLcmd(): PQconnectPoll: %d\n", PQconnectPoll(conn));        
	fprintf(stderr, "ExecSQLcmd(): PQstatus: %d\n", PQstatus(conn));        
#endif

	PGresult *res = PQexec(conn, sqlCmd);    

#if (DEBUG > 0)
    fprintf(stderr, "ExecSQLcmd(): PQexec done\n");        
#endif
    
    // error checking
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "ExecSQLcmd(): SQL command failed: %s\n", sqlCmd);
		fprintf(stderr, "ExecSQLcmd(): %s\n", PQresStatus(PQresultStatus(res)));
		fprintf(stderr, "ExecSQLcmd(): PQresultErrorMessage: %s\n", PQresultErrorMessage(res));
           
        PQclear(res);
        finish(); // close connection and exit
    }    

#if 0
    fprintf(stderr, "ExecSQLcmd() SQL command OK: %s\n", sqlCmd);        
#endif

    return res;
}


/**
 * @brief Execute query on DB. Used for query that return data.
 *
 * @param sqlCmd the SQL query
 */
PGresult* Con2DB::ExecSQLtuples(char *sqlCmd) {
	if (conn == NULL) {
		fprintf(stderr, "ExecSQLtuples(): no connection to DB: sqlCmd = %s\n", sqlCmd);
		exit(1);
	}

	PGresult *res = PQexec(conn, sqlCmd);    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "ExecSQLtuples(): SQL command failed: %s\n", sqlCmd);
		fprintf(stderr, "ExecSQLtuples(): %s\n", PQresStatus(PQresultStatus(res)));

        PQclear(res);
        finish();
    }    

#if 0
    fprintf(stderr, "ExecSQLtuples() OK: %s\n", sqlCmd);        
#endif

    return res;
}
