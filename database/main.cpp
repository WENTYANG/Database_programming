#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include <string>

#include "exerciser.h"

using namespace std;
using namespace pqxx;

void executeSQL(connection* C, string sql) {
    work W(*C);
    // Execute SQL query
    W.exec(sql);
    W.commit();
}

void dropTables(connection* C) {
    string sql;
    sql = "DROP TABLE IF EXISTS \"PLAYER\";";
    executeSQL(C, sql);
    sql = "DROP TABLE IF EXISTS \"TEAM\";";
    executeSQL(C, sql);
    sql = "DROP TABLE IF EXISTS \"STATE\";";
    executeSQL(C, sql);
    sql = "DROP TABLE IF EXISTS \"COLOR\";";
    executeSQL(C, sql);

    // cout << "Tables dropped successfully" << endl;
}

void createTables(connection* C) {
    string sql;
    // Create State
    sql =
        "CREATE TABLE \"STATE\"("
        "\"STATE_ID\" SERIAL NOT NULL,"
        "\"NAME\" VARCHAR(256) NOT NULL,"
        "CONSTRAINT \"STATEPPK\" PRIMARY KEY(\"STATE_ID\"));";
    executeSQL(C, sql);
    // cout << "Table State created successfully" << endl;

    // Create Color
    sql =
        "CREATE TABLE \"COLOR\"("
        "\"COLOR_ID\" SERIAL NOT NULL,"
        "\"NAME\" VARCHAR(256) NOT NULL,"
        "CONSTRAINT \"COLORPPK\" PRIMARY KEY(\"COLOR_ID\"));";
    executeSQL(C, sql);
    // cout << "Table Color created successfully" << endl;

    // Create Team
    sql =
        "CREATE TABLE \"TEAM\"("
        "\"TEAM_ID\" SERIAL NOT NULL,"
        "\"NAME\" VARCHAR(256) NOT NULL,"
        "\"STATE_ID\" INT,"
        "\"COLOR_ID\" INT,"
        "\"WINS\" INT NOT NULL,"
        "\"LOSSES\" INT NOT NULL,"
        "CONSTRAINT \"TEAMPPK\" PRIMARY KEY(\"TEAM_ID\"),"
        "FOREIGN KEY (\"STATE_ID\") REFERENCES \"STATE\"(\"STATE_ID\") ON "
        "DELETE SET NULL "
        "ON UPDATE CASCADE,"
        "FOREIGN KEY (\"COLOR_ID\") REFERENCES \"COLOR\"(\"COLOR_ID\") ON "
        "DELETE SET NULL "
        "ON UPDATE CASCADE);";
    executeSQL(C, sql);
    // cout << "Table Team created successfully" << endl;

    // Create Player
    sql =
        "CREATE TABLE \"PLAYER\"("
        "\"PLAYER_ID\" SERIAL NOT NULL,"
        "\"TEAM_ID\" INT,"
        "\"UNIFORM_NUM\" INT NOT NULL,"
        "\"FIRST_NAME\" VARCHAR(256) NOT NULL,"
        "\"LAST_NAME\" VARCHAR(256) NOT NULL,"
        "\"MPG\" INT NOT NULL,"
        "\"PPG\" INT NOT NULL,"
        "\"RPG\" INT NOT NULL,"
        "\"APG\" INT NOT NULL,"
        "\"SPG\" DECIMAL(18,1) NOT NULL,"
        "\"BPG\" DECIMAL(18,1) NOT NULL,"
        "CONSTRAINT \"PLYERPPK\" PRIMARY KEY(\"PLAYER_ID\"),"
        "FOREIGN KEY (\"TEAM_ID\") REFERENCES \"TEAM\"(\"TEAM_ID\") ON DELETE "
        "SET NULL "
        "ON UPDATE CASCADE);";
    executeSQL(C, sql);
    // cout << "Table Player created successfully" << endl;
}

void populatePlayer(connection* C, string filename) {
    int player_id, team_id, jersey_num, mpg, ppg, rpg, apg;
    string first_name, last_name;
    double spg, bpg;

    ifstream ifs(filename.c_str());
    if (!ifs.is_open()) {
        cerr << "Unable to open file" << filename << endl;
        return;
    }
    string line;
    while (getline(ifs, line)) {
        stringstream row;
        row << line;
        row >> player_id >> team_id >> jersey_num >> first_name >> last_name >>
            mpg >> ppg >> rpg >> apg >> spg >> bpg;
        add_player(C, team_id, jersey_num, first_name, last_name, mpg, ppg, rpg,
                   apg, spg, bpg);
    }
    ifs.close();
}

void populateTeam(connection* C, string filename) {
    int team_id, state_id, color_id, wins, losses;
    string name;

    ifstream ifs(filename.c_str());
    if (!ifs.is_open()) {
        cerr << "Unable to open file" << filename << endl;
        return;
    }
    string line;
    while (getline(ifs, line)) {
        stringstream row;
        row << line;
        row >> team_id >> name >> state_id >> color_id >> wins >> losses;
        add_team(C, name, state_id, color_id, wins, losses);
    }
    ifs.close();
}

void populateState(connection* C, string filename) {
    int state_id;
    string name;
    ifstream ifs(filename.c_str());
    if (!ifs.is_open()) {
        cerr << "Unable to open file" << filename << endl;
        return;
    }
    string line;
    while (getline(ifs, line)) {
        stringstream row;
        row << line;
        row >> state_id >> name;
        add_state(C, name);
    }
    ifs.close();
}

void populateColor(connection* C, string filename) {
    int color_id;
    string name;
    ifstream ifs(filename.c_str());
    if (!ifs.is_open()) {
        cerr << "Unable to open file" << filename << endl;
        return;
    }
    string line;
    while (getline(ifs, line)) {
        stringstream row;
        row << line;
        row >> color_id >> name;
        add_color(C, name);
    }
    ifs.close();
}

int main(int argc, char* argv[]) {
    // Allocate & initialize a Postgres connection object
    connection* C;

    try {
        // Establish a connection to the database
        // Parameters: database name, user name, user password
        C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
        if (C->is_open()) {
            cout << "Opened database successfully: " << C->dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
            return 1;
        }
    } catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return 1;
    }

    // TODO: create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL
    // database
    //       load each table with rows from the provided source txt files

    dropTables(C);
    createTables(C);
    populateState(C, "state.txt");
    populateColor(C, "color.txt");
    populateTeam(C, "team.txt");
    populatePlayer(C, "player.txt");
    exercise(C);

    // Close database connection
    C->disconnect();

    return 0;
}