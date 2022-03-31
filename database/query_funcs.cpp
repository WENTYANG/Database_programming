#include "query_funcs.h"
#include <string>

void add_player(connection* C,
                int team_id,
                int jersey_num,
                string first_name,
                string last_name,
                int mpg,
                int ppg,
                int rpg,
                int apg,
                double spg,
                double bpg) {
    work W(*C);
    stringstream sql;
    sql << "INSERT INTO \"PLAYER\"(\"TEAM_ID\", \"UNIFORM_NUM\", "
           "\"FIRST_NAME\", "
           "\"LAST_NAME\", \"MPG\", \"PPG\", "
           "\"RPG\", \"APG\", \"SPG\", \"BPG\") VALUES("
        << team_id << "," << jersey_num << "," << W.quote(first_name) << ","
        << W.quote(last_name) << "," << mpg << "," << ppg << "," << rpg << ","
        << apg << "," << spg << "," << bpg << ");";
    W.exec(sql.str());
    W.commit();
}

void add_team(connection* C,
              string name,
              int state_id,
              int color_id,
              int wins,
              int losses) {
    stringstream sql;
    work W(*C);
    sql << "INSERT INTO \"TEAM\" (\"NAME\", \"STATE_ID\", \"COLOR_ID\", "
           "\"WINS\", "
           "\"LOSSES\") VALUES ("
        << W.quote(name) << "," << state_id << "," << color_id << "," << wins
        << "," << losses << ");";

    W.exec(sql.str());
    W.commit();
    // cout << "insert into table TEAM" << endl;
}

void add_state(connection* C, string name) {
    stringstream sql;
    work W(*C);
    sql << "INSERT INTO \"STATE\" (\"NAME\") VALUES (" << W.quote(name) << ");";

    W.exec(sql.str());
    W.commit();
    // cout << "insert into table STATE" << endl;
}

void add_color(connection* C, string name) {
    work W(*C);
    stringstream sql;
    sql << "INSERT INTO \"COLOR\"(\"NAME\") VALUES(" << W.quote(name) << ");";
    W.exec(sql.str());
    W.commit();
}

void query1(connection* C,
            int use_mpg,
            int min_mpg,
            int max_mpg,
            int use_ppg,
            int min_ppg,
            int max_ppg,
            int use_rpg,
            int min_rpg,
            int max_rpg,
            int use_apg,
            int min_apg,
            int max_apg,
            int use_spg,
            double min_spg,
            double max_spg,
            int use_bpg,
            double min_bpg,
            double max_bpg) {
    stringstream sql;
    sql << "SELECT * FROM \"PLAYER\" ";
    int num = 0;
    if (use_mpg) {
        if (!num)
            sql << "WHERE ";
        else
            sql << "AND ";
        sql << "\"MPG\" >= " << min_mpg << " AND \"MPG\" <= " << max_mpg;
        num++;
    }
    if (use_ppg) {
        if (!num)
            sql << "WHERE ";
        else
            sql << "AND ";
        sql << "\"PPG\" >= " << min_ppg << " AND \"PPG\" <= " << max_ppg;
        num++;
    }
    if (use_rpg) {
        if (!num)
            sql << "WHERE ";
        else
            sql << "AND ";
        sql << "\"RPG\" >= " << min_rpg << " AND \"RPG\" <= " << max_rpg;
        num++;
    }
    if (use_apg) {
        if (!num)
            sql << "WHERE ";
        else
            sql << "AND ";
        sql << "\"APG\" >= " << min_apg << " AND \"APG\" <= " << max_apg;
        num++;
    }
    if (use_spg) {
        if (!num)
            sql << "WHERE ";
        else
            sql << "AND ";
        sql << "\"SPG\" >= " << min_spg << " AND \"SPG\" <= " << max_spg;
        num++;
    }
    if (use_bpg) {
        if (!num)
            sql << "WHERE ";
        else
            sql << "AND ";
        sql << "\"BPG\" >= " << min_bpg << " AND \"BPG\" <= " << max_bpg;
        num++;
    }

    sql << ";";
    nontransaction N(*C);
    result R(N.exec(sql.str()));

    cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG "
            "APG SPG BPG"
         << endl;
    for (auto const& r : R) {
        for (auto const& c : r) {
            cout << c.c_str() << ' ';
        }
        cout << endl;
    }
}

void query2(connection* C, string team_color) {
    stringstream sql;
    sql << "SELECT \"TEAM\".\"NAME\" FROM \"TEAM\", \"COLOR\" WHERE "
           "\"TEAM\".\"COLOR_ID\"=\"COLOR\".\"COLOR_ID\" AND "
           "\"COLOR\".\"NAME\" = '"
        << team_color << "';";
    nontransaction N(*C);
    result R(N.exec(sql.str()));
    cout << "NAME" << endl;
    for (auto const& r : R) {
        cout << r[0].c_str() << endl;
    }
}

void query3(connection* C, string team_name) {
    stringstream sql;
    sql << "SELECT \"PLAYER\".\"FIRST_NAME\", \"PLAYER\".\"LAST_NAME\" FROM "
           "\"TEAM\", \"PLAYER\" WHERE "
           "\"PLAYER\".\"TEAM_ID\"=\"TEAM\".\"TEAM_ID\" AND "
           "\"TEAM\".\"NAME\" = '"
        << team_name << "' ORDER BY \"PPG\" DESC;";
    nontransaction N(*C);
    result R(N.exec(sql.str()));
    cout << "FIRST_NAME LAST_NAME" << endl;
    for (auto const& r : R) {
        cout << r[0].c_str() << ' ' << r[1].c_str() << endl;
    }
}

void query4(connection* C, string team_state, string team_color) {
    stringstream sql;
    sql << "SELECT \"PLAYER\".\"UNIFORM_NUM\", \"PLAYER\".\"FIRST_NAME\", "
           "\"PLAYER\".\"LAST_NAME\" FROM "
           "\"TEAM\", \"PLAYER\", \"COLOR\", \"STATE\" WHERE "
           "\"PLAYER\".\"TEAM_ID\"=\"TEAM\".\"TEAM_ID\" AND "
           "\"TEAM\".\"STATE_ID\" = \"STATE\".\"STATE_ID\" AND "
           "\"TEAM\".\"COLOR_ID\" = \"COLOR\".\"COLOR_ID\" AND "
           "\"STATE\".\"NAME\"='"
        << team_state << "' AND \"COLOR\".\"NAME\"=\'" << team_color << "';";
    nontransaction N(*C);
    result R(N.exec(sql.str()));
    cout << "UNIFORM_NUM FIRST_NAME LAST_NAME" << endl;
    for (auto const& r : R) {
        cout << r[0].c_str() << ' ' << r[1].c_str() << ' ' << r[2] << endl;
    }
}

void query5(connection* C, int num_wins) {
    stringstream sql;
    sql << "SELECT \"PLAYER\".\"FIRST_NAME\", \"PLAYER\".\"LAST_NAME\", "
           "\"TEAM\".\"NAME\", \"TEAM\".\"WINS\" FROM "
           "\"TEAM\", \"PLAYER\" WHERE "
           "\"PLAYER\".\"TEAM_ID\"=\"TEAM\".\"TEAM_ID\" AND "
           "\"TEAM\".\"WINS\" > "
        << num_wins << ";";
    nontransaction N(*C);
    result R(N.exec(sql.str()));
    cout << "FIRST_NAME LAST_NAME NAME WINS" << endl;
    for (auto const& r : R) {
        cout << r[0].c_str() << ' ' << r[1].c_str() << ' ' << r[2] << ' '
             << r[3] << endl;
    }
}
