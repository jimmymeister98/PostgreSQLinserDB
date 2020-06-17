#pragma once
//
// DBS-Praktikum Aufgabe 2
//    Header abstraktes DB-Interface
//
// Christoph Dalitz
// Hochschule Niederrhein
// 2013/03/26
//

#ifndef DB_H
#define DB_H

#include <string>


using namespace std;

// Datenbank-Login
// rc: 0 = ok, 1 = error
int db_login(const string& user, const string& password, const string& host, const string& dbname,const string& port);

// Datenbank-Logout
void db_logout();

// Transaktionsbefehle
// rc: 0 = ok, 1 = error
int db_begin();
int db_commit();
int db_rollback();

// Herstellernummer schon vorhanden?
// rc: 0 = noch nicht da, 1 = schon da, -1 = error
int db_findhnr(const string& hnr);

// Einfuegen Datensatz
// rc: 0 = ok, 1 = error
int db_insert(const string& hnr, const string& name, const string& plz, const string& ort);

// Loeschen des kompletten Tabelleninhalts
// rc: 0 = ok, 1 = error
int db_delete();

#endif
