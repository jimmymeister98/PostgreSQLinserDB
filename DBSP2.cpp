// DBSP2.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream> 
#include "db.h"
#define DEBUG false
void tokenize(std::string const& str, const char delim,
    std::vector<std::string>& out)
{
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
}

std::string toLower(const char argv[]) {
    std::string str = argv;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] -= ('A' - 'a');
    }
    return str;
}

int main(int argc, char* argv[])
{
    std::string hostaddr, port, pgdbname, pguser, pgpassword, infile;
    bool delParam = false;

    // Parameter verarbeiten
    std::string tmp;
    for (int i = 1; i < argc; i++) {
        if (DEBUG) std::cout << "[" << i << "]: " << argv[i] << std::endl;
        if (argv[i][0] == '-') { // wenn mit '-' anfängt -> toLower
            tmp = toLower(argv[i]);
        }

        else {    // muss Dateiname sein
            infile = argv[i];
            if (DEBUG) std::cout << "Set infile = " << infile << std::endl;
            continue;
        }

        if (tmp == "-del") {
            delParam = true;
            if (DEBUG) std::cout << "Set del = true" << std::endl;
        }
        else if (argc > i + 1) {
            if (tmp == "-u") {
                pguser = argv[i + 1];
                if (DEBUG) std::cout << "Set pguser = " << pguser << std::endl;
                i++;
            }
            else if (tmp == "-p") {
                pgpassword = argv[i + 1];
                if (DEBUG) std::cout << "Set pgpassword = " << pgpassword << std::endl;
                i++;
            }
            else if (tmp == "-h") {
                hostaddr = argv[i + 1];
                if (DEBUG) std::cout << "Set hostaddr = " << hostaddr << std::endl;
                i++;
            }
            else if (tmp == "-d") {
                pgdbname = argv[i + 1];
                if (DEBUG) std::cout << "Set pgdbname = " << pgdbname << std::endl;
                i++;
            }
            else if (tmp == "-port") { 
                port = argv[i + 1];
                if (DEBUG) std::cout << "Set port = " << port << std::endl;
                i++;
            }
            else {
                std::cout << "Error, unknown argument '" << argv[i] << "'" << std::endl;
                return -1;
            }
        }
        else {
            std::cout << "Error, no value for '" << argv[i] << "'" << std::endl;
            return -1;
        }
    }
    if (pguser == "" || pgpassword == "" || hostaddr == "" || pgdbname == "" || port == ""||infile == "") {
        std::cout << "\nUsage:\n       dbimp [options] <infile>\nOptions:\n       -del delete table contents before import\n       -u database user\n       -p password\n       -h database host\n       -d database name\n       -port port number" << std::endl;
        exit(-1);
    }
    int res;
    res = db_login(pguser, pgpassword, hostaddr, pgdbname, port);
    if (res == 1) {
        cout << "Error, could'nt login to DB!" << endl;
        return -1;
    }

    res = db_begin();

    if (res == 1) {
        cout << "Error, BEGIN failed!" << endl;
        return -1;
    }

    if (delParam) { // herstellertabelle vorher leeren
        res = db_delete();
        if (res == 1) {
            cout << "Failed to empty the table!" << endl;
            res = db_rollback();
            if (res == 1) cerr << "ROLLBACK failed, this shouldn't be happening." << endl;
            return -1;
        }
    }


    bool doRollback = false;
    int insertedCount = 0;
    int j = 0;
    try{
    std::ifstream infilestream;
    string    hnr;
    string    name;
    string    plz;
    string    ort;

    infilestream.open(infile.c_str(), std::ios::in);
    string str;
    string line;
    if (infilestream) {
        string token;
        stringstream iss;

        while (getline(infilestream, line, '\n')) {
            iss << line;                                                                    //Ganze zeile
            //std::cout << "eingelesen: " << hnr << name << plz << ort << std::endl;
            int i = 0;
            while (getline(iss, token, ';')) {

                if (i == 0) {
                    hnr = token;
                 //   while (hnr.length() > 3) {
                  //      hnr.pop_back();
                 //   }
                }
                if (i == 1) {
                    name = token;
                   // while (name.length() > 30)
                   //     name.pop_back();
                }
                if (i == 2) {
                    plz = token;
                 //   while (plz.length() > 5)
                 //       plz.pop_back();
                }
                if (i == 3) {
                    ort = token;
                 //   while (ort.length() > 30)
                  //      ort.pop_back();
                }
                ++i;
                
            }
            j++;



           
            iss.clear();
            res = db_findhnr(hnr);

            if (res == -1) {
                cout << "Failed to import " << infile <<": search for '" << hnr << "' failed!" << endl;
                doRollback = true;
                break;
            }
            else if (res == 1) {
                continue;   
            }
            // res == 0:
            res = db_insert(hnr, name, plz, ort);

            if (res == 1) {
                cout << "Failed to import " << infile << " INSERT failed!" << endl;
                doRollback = true;
                break;
            }

            insertedCount++;





        }
    }

    infilestream.close();
}
    catch (exception& e) {
        cerr << "Exception occured: " << e.what() << endl;
    }
    if (doRollback) {
        res = db_rollback();
        if (res == 1) cerr << "ROLLBACK failed, this shouldn't be happening." << endl;
    }
    else {
        res = db_commit();
        if (res == 1) cerr << "COMMIT failed!" << endl;
        else cout << insertedCount <<"/"<<j<< " Entrys from '" << infile << "' imported!" << endl;
    }
    db_logout();
    return 0;
}

// Programm ausführen: STRG+F5 oder Menüeintrag "Debuggen" > "Starten ohne Debuggen starten"
// Programm debuggen: F5 oder "Debuggen" > Menü "Debuggen starten"

// Tipps für den Einstieg: 
//   1. Verwenden Sie das Projektmappen-Explorer-Fenster zum Hinzufügen/Verwalten von Dateien.
//   2. Verwenden Sie das Team Explorer-Fenster zum Herstellen einer Verbindung mit der Quellcodeverwaltung.
//   3. Verwenden Sie das Ausgabefenster, um die Buildausgabe und andere Nachrichten anzuzeigen.
//   4. Verwenden Sie das Fenster "Fehlerliste", um Fehler anzuzeigen.
//   5. Wechseln Sie zu "Projekt" > "Neues Element hinzufügen", um neue Codedateien zu erstellen, bzw. zu "Projekt" > "Vorhandenes Element hinzufügen", um dem Projekt vorhandene Codedateien hinzuzufügen.
//   6. Um dieses Projekt später erneut zu öffnen, wechseln Sie zu "Datei" > "Öffnen" > "Projekt", und wählen Sie die SLN-Datei aus.
