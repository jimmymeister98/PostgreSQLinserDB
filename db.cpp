#include "db.h"
#include <iostream>
#include <libpq-fe.h>
#include <algorithm>
#define DEBUG false;
std::string conninfo;
PGconn* conn;
PGresult* res;
int         nFields;
int         i,j;

static void exit_nicely(PGconn* conn)
{
	PQfinish(conn);
	exit(1);
}

int db_login(const string& user, const string& password, const string& host, const string& dbname, const string& port)
{	
	conninfo = "hostaddr=";
	conninfo += host;
	conninfo += " port=";
	conninfo += port;
	conninfo += " dbname=";
	conninfo += dbname;
	conninfo += " user=";
	conninfo += user;
	conninfo += " password=";
	conninfo += password;
	//wandle string in c string(const char*) um
	const char* connectstring = conninfo.c_str();
	//Verbinde mit DB
	conn = PQconnectdb(connectstring);
	//Prüfe ob Verbindung steht
	if (PQstatus(conn) != CONNECTION_OK)
	{
		fprintf(stderr, "Connection to database failed: %s",
			PQerrorMessage(conn));
		exit_nicely(conn);
	}
	
	
		std::cout << "Connected!" << std::endl;
		//std::cout << "Connection Successfull with the following params  ====>" << conninfo << std::endl;
	


	PQclear(res);
	//std::cout << "Hat funktioniert" << std::endl;
	return 0;
}

void db_logout()
{
	exit_nicely(conn);
}

int db_begin()
{
	res = PQexec(conn, "BEGIN");
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(conn));
		PQclear(res);
		exit_nicely(conn);
	}
	return 0;
}

int db_commit()
{
	PGresult* db_res;
	db_res = PQexec(conn, "COMMIT");

	// Fehler
	if (PQresultStatus(db_res) != PGRES_COMMAND_OK) {
		cerr << "COMMIT fehlgeschlagen: " << PQresultErrorMessage(db_res) << endl;
		PQclear(db_res);
		return 1;
	}

	PQclear(db_res);
	return 0;
}

int db_rollback()
{
	PGresult* db_res;
	db_res = PQexec(conn, "ROLLBACK");

	// Fehler
	if (PQresultStatus(db_res) != PGRES_COMMAND_OK) {
		cerr << "ROLLBACK fehlgeschlagen: " << PQresultErrorMessage(db_res) << endl;
		PQclear(db_res);
		return 1;
	}

	PQclear(db_res);
	return 0;
}

int db_findhnr(const string& hnr)
{	

	PGresult* db_res;
	db_res = PQexec(conn, ("SELECT hnr FROM hersteller WHERE hnr = '" + hnr + "'").c_str());

	// Fehler
	if (PQresultStatus(db_res) != PGRES_TUPLES_OK) {
		cerr << "Fehler beim Herstellernummer finden: " << PQresultErrorMessage(db_res) << endl;
		PQclear(db_res);
		return -1;
	}

	int resultCount = PQntuples(db_res);

	PQclear(db_res);

	return std::min(resultCount, 1);  // 0 oder 1
}

int db_insert(const string& hnr, const string& name, const string& plz, const string& ort)
{
	string command = "INSERT INTO hersteller ( hnr, name, plz, ort ) "
		"VALUES ( '" + hnr + "', '" + name + "', '" + plz + "', '" + ort + "' )";

	PGresult* db_res;
	db_res = PQexec(conn, command.c_str());

	// Fehler
	if (PQresultStatus(db_res) != PGRES_COMMAND_OK) {
		cerr << "INSERT fehlgeschlagen: " << PQresultErrorMessage(db_res) << endl;
		PQclear(db_res);
		return 1;
	}

	PQclear(db_res);
	return 0;
}

int db_delete()
{
	PGresult* db_res;
	db_res = PQexec(conn, "DELETE FROM hersteller");

	// Fehler
	if (PQresultStatus(db_res) != PGRES_COMMAND_OK) {
		cerr << "DELETE fehlgeschlagen: " << PQresultErrorMessage(db_res) << endl;
		PQclear(db_res);
		return 1;
	}

	PQclear(db_res);
	return 0;
	
}
