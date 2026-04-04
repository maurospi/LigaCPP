#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <ctime>
using namespace std;

struct LeagueConfig {
    string leagueName;
    int winPts;
    int lossPts;
    int drawPts;
    vector<string> clubs;
};

struct Match {
    string date;
    string home;
    string away;
    int homeGoals;
    int awayGoals;
};

struct Club {
    string name;
    string status;
    int played;
    int won;
    int drawn;
    int lost;
    int goalsFor;
    int goalsAgainst;
    int goalDiff;
    int points;
};

string getTodayDate() {
    time_t t = time(nullptr);
    tm* tmInfo = localtime(&t);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", tmInfo);
    return string(buffer);
}

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if(start == string::npos) {
        return "";
    }
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool loadConfig(const string& file, LeagueConfig& config) {
    ifstream configFile(file);
    if(!configFile.is_open()) {
        cerr << "ERROR: No se pudo abrir '" << file << "'.\n";
        cerr << "El archivo de configuracion es obligatorio.\n";
        return false;
    }

    bool hasName = false, hasWin = false, hasLoss = false, hasDraw = false;
    string line;
    while(getline(configFile, line)) {
        line = trim(line);
        if(line.empty() || line[0] == '#') {
            continue;
        }
        size_t pos = line.find('=');
        if(pos == string::npos) {
            continue;
        }
        string key   = trim(line.substr(0, pos));
        string value = trim(line.substr(pos + 1));
        if(key == "league") {
            config.leagueName = value;
            hasName = true;
        } else if(key == "win_points") {
            config.winPts = stoi(value);
            hasWin = true;
        } else if(key == "draw_points") {
            config.drawPts = stoi(value);
            hasDraw = true;
        } else if(key == "defeat_points") {
            config.lossPts = stoi(value);
            hasLoss = true;
        } else if(key == "club" && !value.empty()) {
            config.clubs.push_back(value);
        }
    }
    configFile.close();
    if(!hasName || !hasWin || !hasDraw || !hasLoss) {
        cerr << "ERROR: config.txt le faltan campos obligatorios.\n";
        cerr << "       Datos Requeridos: league, win_points, draw_points, defeat_points\n";
        return false;
    }
    if(config.clubs.empty()) {
        cerr << "ERROR: config.txt no tiene ningun equipo registrado (clave 'club=').\n";
        return false;
    }
    return true;
}

bool loadMatches(const string& file, vector<Match>& matches) {
    matches.clear();
    ifstream matchFile(file);
    if(!matchFile.is_open()) {
        return true;
    }

    string line;
    int lineNum = 0;
    while(getline(matchFile, line)) {
        lineNum++;
        line = trim(line);
        if(line.empty()) {
            continue;
        }

        istringstream ss(line);
        string date, home, away, sHomeGoals, sAwayGoals;

        if(!getline(ss, date,       ';') || !getline(ss, home,       ';') || !getline(ss, away,       ';') || !getline(ss, sHomeGoals, ';') || !getline(ss, sAwayGoals, ';')) {
            cout << "ADVERTENCIA: Linea " << lineNum << " de partidos.txt tiene formato invalido.\n";
            continue;
        }

        Match m;
        m.date      = trim(date);
        m.home      = trim(home);
        m.away      = trim(away);
        m.homeGoals = stoi(trim(sHomeGoals));
        m.awayGoals = stoi(trim(sAwayGoals));
        matches.push_back(m);
    }
    matchFile.close();
    return true;
}

void showMatchHistory(const string& file) {
    ifstream historyFile(file);
    if(!historyFile.is_open()) {
        cout << "\nNo hay historial de jornadas registrado aun.\n";
        return;
    }

    string line;
    bool hasContent = false;
    cout << "\n";

    while(getline(historyFile, line)) {
        line = trim(line);
        if(line.empty()) {
            continue;
        }
        hasContent = true;

        if(line.find("JORNADA=") == 0) {
            cout << "JORNADA " << line.substr(8) << "\n";
        } else if(line.find("FECHA=") == 0) {
            cout << "Fecha: " << line.substr(6) << "\n";
            cout << "----------------------------------------\n";
        } else if(line == "FIN_JORNADA") {
            cout << "\n\n";
        } else {
            istringstream ss(line);
            string home, away, sHomeGoals, sAwayGoals;
            if(getline(ss, home,       ';') && getline(ss, away,       ';') && getline(ss, sHomeGoals, ';') && getline(ss, sAwayGoals)) {
                cout << "  " << trim(home) << "  " << trim(sHomeGoals) << " - " << trim(sAwayGoals) << "  " << trim(away) << "\n";
            }
        }
    }
    historyFile.close();

    if(!hasContent) {
        cout << "No hay registro historico de jornadas registradas.\n";
    }
}

void appendMatch(const string& file, const Match& m) {
    ofstream matchFile(file, ios::app);
    if(!matchFile.is_open()) {
        cerr << "ERROR: No se pudo abrir '" << file << "' para escritura.\n";
        return;
    }
    matchFile << m.date << ";" << m.home << ";" << m.away << ";" << m.homeGoals << ";" << m.awayGoals << "\n";
    matchFile.close();
}

void saveAllMatches(const string& file, const vector<Match>& matches) {
    ofstream matchFile(file);
    if(!matchFile.is_open()) {
        cerr << "ERROR: No se pudo abrir '" << file << "'.\n";
        return;
    }
    for(int i = 0; i < (int)matches.size(); i++) {
        matchFile << matches[i].date << ";" << matches[i].home << ";" << matches[i].away << ";" << matches[i].homeGoals << ";" << matches[i].awayGoals << "\n";
    }
    matchFile.close();
}

void updateMatchDay(const string& file, const Match& m) {
    vector<string> lines;
    ifstream daysFileIn(file);
    if(daysFileIn.is_open()) {
        string line;
        while(getline(daysFileIn, line)) {
            lines.push_back(line);
        }
        daysFileIn.close();
    }

    string matchEntry = m.home + ";" + m.away + ";" + to_string(m.homeGoals) + ";" + to_string(m.awayGoals);
    string dayKey     = "FECHA=" + m.date;
    bool dayFound     = false;

    for(int i = 0; i < (int)lines.size(); i++) {
        if(trim(lines[i]) == dayKey) {
            for(int j = i + 1; j < (int)lines.size(); j++) {
                if(trim(lines[j]) == "FIN_JORNADA") {
                    lines.insert(lines.begin() + j, matchEntry);
                    dayFound = true;
                    break;
                }
            }
            break;
        }
    }

    if(!dayFound) {
        int dayCount = 1;
        for(int i = 0; i < (int)lines.size(); i++) {
            if(trim(lines[i]).find("JORNADA=") == 0) {
                dayCount++;
            }
        }
        lines.push_back("JORNADA=" + to_string(dayCount));
        lines.push_back("FECHA="   + m.date);
        lines.push_back(matchEntry);
        lines.push_back("FIN_JORNADA");
    }

    ofstream daysFileOut(file);
    if(!daysFileOut.is_open()) {
        cerr << "ERROR: No se puede abrir: " << file << "\n";
        return;
    }
    for(int i = 0; i < (int)lines.size(); i++) {
        daysFileOut << lines[i] << "\n";
    }
    daysFileOut.close();
}

void saveConfig(const string& file, const LeagueConfig& config) {
    ofstream configFile(file);
    if(!configFile.is_open()) {
        cerr << "ERROR: No se pudo guardar '" << file << "'.\n";
        return;
    }
    configFile << "league="        << config.leagueName << "\n";
    configFile << "win_points="    << config.winPts     << "\n";
    configFile << "draw_points="   << config.drawPts    << "\n";
    configFile << "defeat_points=" << config.lossPts    << "\n";
    for(int i = 0; i < (int)config.clubs.size(); i++) {
        configFile << "club=" << config.clubs[i] << "\n";
    }
    configFile.close();
}

void updateStats(Club* club, int goalsFor, int goalsAgainst, const LeagueConfig& config) {
    club->played++;
    club->goalsFor     += goalsFor;
    club->goalsAgainst += goalsAgainst;
    club->goalDiff      = club->goalsFor - club->goalsAgainst;

    if(goalsFor > goalsAgainst) {
        club->won++;
        club->points += config.winPts;
    } else if(goalsFor == goalsAgainst) {
        club->drawn++;
        club->points += config.drawPts;
    } else {
        club->lost++;
        club->points += config.lossPts;
    }
}

vector<Club> buildTable(const vector<Match>& matches, const LeagueConfig& config) {
    vector<Club> table;

    for(int i = 0; i < (int)config.clubs.size(); i++) {
        Club c;
        c.name     = config.clubs[i];
        c.status   = "-----";
        c.played   = c.won = c.drawn = c.lost = 0;
        c.goalsFor = c.goalsAgainst = c.goalDiff = c.points = 0;
        table.push_back(c);
    }

    for(int i = 0; i < (int)matches.size(); i++) {
        const Match& m = matches[i];
        for(int j = 0; j < (int)table.size(); j++) {
            if(table[j].name == m.home) {
                updateStats(&table[j], m.homeGoals, m.awayGoals, config);
            }
            if(table[j].name == m.away) {
                updateStats(&table[j], m.awayGoals, m.homeGoals, config);
            }
        }
    }
    return table;
}

bool compareClubs(const Club& a, const Club& b) {
    if(a.points   != b.points)   { return a.points   > b.points; }
    if(a.goalDiff != b.goalDiff) { return a.goalDiff > b.goalDiff; }
    return a.goalsFor > b.goalsFor;
}

void sortTable(vector<Club>& table) {
    sort(table.begin(), table.end(), compareClubs);
}

void printTable(const vector<Club>& table) {
    cout << "\n";
    cout << left  << setw(4)  << "#" << setw(22) << "Equipo" << setw(14) << "Estado" << right << setw(5)  << "PJ" << setw(5)  << "PG" << setw(5)  << "PE" << setw(5)  << "PP" << setw(5)  << "GF" << setw(5)  << "GC" << setw(6)  << "DG" << setw(6)  << "PTS" << "\n";
    cout << string(82, '-') << "\n";

    for(int i = 0; i < (int)table.size(); i++) {
        const Club& c = table[i];
        string gdStr;
        if(c.goalDiff >= 0) {
            gdStr = "+" + to_string(c.goalDiff);
        } else {
            gdStr = to_string(c.goalDiff);
        }
        cout << left  << setw(4)  << (i + 1) << setw(22) << c.name << setw(14) << c.status << right << setw(5)  << c.played << setw(5)  << c.won << setw(5)  << c.drawn << setw(5)  << c.lost << setw(5)  << c.goalsFor << setw(5)  << c.goalsAgainst << setw(6)  << gdStr << setw(6)  << c.points << "\n";
    }
    cout << "\n";
}

void saveTable(const vector<Club>& table, const string& file) {
    ofstream tableFile(file);
    if(!tableFile.is_open()) {
        cerr << "ERROR: No se pudo guardar la tabla en: " << file << "\n";
        return;
    }
    tableFile << left  << setw(4)  << "#" << setw(22) << "Equipo" << setw(14) << "Estado" << right << setw(5)  << "PJ" << setw(5)  << "PG" << setw(5)  << "PE" << setw(5)  << "PP" << setw(5)  << "GF" << setw(5)  << "GC" << setw(6)  << "DG" << setw(6)  << "PTS" << "\n";
    tableFile << string(82, '-') << "\n";
    for(int i = 0; i < (int)table.size(); i++) {
        const Club& c = table[i];
        string gdStr;
        if(c.goalDiff >= 0) {
            gdStr = "+" + to_string(c.goalDiff);
        } else {
            gdStr = to_string(c.goalDiff);
        }
        tableFile << left  << setw(4)  << (i + 1) << setw(22) << c.name << setw(14) << c.status << right << setw(5)  << c.played << setw(5)  << c.won << setw(5)  << c.drawn << setw(5)  << c.lost << setw(5)  << c.goalsFor << setw(5)  << c.goalsAgainst << setw(6)  << gdStr << setw(6)  << c.points << "\n";
    }
    tableFile.close();
    cout << "La tabla se ha guardado en el archivo: " << file << ".\n";
}

int showMenu(const string& leagueName) {
    cout << "-----------------------------------------\n";
    cout << "1. Ver tabla de posiciones\n";
    cout << "2. Registrar partido\n";
    cout << "3. Ver historial de jornadas\n";
    cout << "4. Ver todos los partidos\n";
    cout << "5. Salir\n\n";
    cout << "Opcion: ";
    int option;
    cin >> option;
    while(cin.fail() || option < 1 || option > 5) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Opcion invalida. Ingrese del 1 al 5: ";
        cin >> option;
    }
    cin.ignore(1000, '\n');
    return option;
}

Match inputMatch(const LeagueConfig& config) {
    Match m;
    m.date = getTodayDate();
    cout << "\n--- Equipos disponibles ---\n";
    for(int i = 0; i < (int)config.clubs.size(); i++) {
        cout << "  " << (i + 1) << ". " << config.clubs[i] << "\n";
    }

    int homeNum = 0;
    cout << "\nSeleccione el equipo local: ";
    cin >> homeNum;
    while(cin.fail() || homeNum < 1 || homeNum > (int)config.clubs.size()) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Numero invalido: ";
        cin >> homeNum;
    }

    int awayNum = 0;
    cout << "Seleccione el equipo visitante: ";
    cin >> awayNum;
    while(cin.fail() || awayNum < 1 || awayNum > (int)config.clubs.size() || awayNum == homeNum) {
        bool typeError = cin.fail();
        if(typeError) {
            cin.clear();
        }
        cin.ignore(1000, '\n');
        if(!typeError && awayNum == homeNum) {
            cout << "No puede ser el mismo que el local: ";
        } else {
            cout << "Numero invalido: ";
        }
        cin >> awayNum;
    }

    m.home = config.clubs[homeNum - 1];
    m.away = config.clubs[awayNum - 1];

    cout << "Goles de " << m.home << ": ";
    cin >> m.homeGoals;
    while(cin.fail() || m.homeGoals < 0) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalido, ingrese mayor o igual a 0: ";
        cin >> m.homeGoals;
    }

    cout << "Goles de " << m.away << ": ";
    cin >> m.awayGoals;
    while(cin.fail() || m.awayGoals < 0) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalido, ingrese mayor o igual a 0: ";
        cin >> m.awayGoals;
    }
    cin.ignore(1000, '\n');
    return m;
}

void printAllMatches(const vector<Match>& matches) {
    if(matches.empty()) {
        cout << "No hay partidos registrados.\n";
        return;
    }
    cout << "\n--- Todos los partidos jugados ---\n";
    cout << left << setw(4) << "#" << setw(12) << "Fecha" << setw(22) << "Local" << setw(9) << "Marcador" << "Visitante\n";
    cout << string(60, '-') << "\n";
    for(int i = 0; i < (int)matches.size(); i++) {
        const Match& m = matches[i];
        string score = to_string(m.homeGoals) + " - " + to_string(m.awayGoals);
        cout << left << setw(4) << (i + 1) << setw(12) << m.date << setw(22) << m.home << setw(9) << score << m.away << "\n";
    }
    cout << "\n";
}

void editClubs(const string& file, LeagueConfig& config) {
    cout << "\n1. Agregar equipo\n";
    cout << "2. Eliminar equipo\n";
    cout << "3. Renombrar equipo\n";
    cout << "Opcion: ";
    int option;
    cin >> option;
    cin.ignore(1000, '\n');

    if(option == 1) {
        cout << "Nombre del equipo: ";
        string newClub;
        getline(cin, newClub);
        config.clubs.push_back(trim(newClub));
        cout << "Equipo agregado.\n";
    } else if(option == 2) {
        for(int i = 0; i < (int)config.clubs.size(); i++) {
            cout << "  " << (i + 1) << ". " << config.clubs[i] << "\n";
        }
        cout << "Numero a eliminar: ";
        int num;
        cin >> num;
        cin.ignore(1000, '\n');
        if(num >= 1 && num <= (int)config.clubs.size()) {
            config.clubs.erase(config.clubs.begin() + (num - 1));
            cout << "Equipo eliminado.\n";
        } else {
            cout << "Numero invalido.\n";
        }
    } else if(option == 3) {
        for(int i = 0; i < (int)config.clubs.size(); i++) {
            cout << "  " << (i + 1) << ". " << config.clubs[i] << "\n";
        }
        cout << "Numero a renombrar: ";
        int num;
        cin >> num;
        cin.ignore(1000, '\n');
        if(num >= 1 && num <= (int)config.clubs.size()) {
            cout << "Nuevo nombre: ";
            string newClubName;
            getline(cin, newClubName);
            config.clubs[num - 1] = trim(newClubName);
            cout << "Equipo renombrado.\n";
        } else {
            cout << "Numero invalido.\n";
        }
    } else {
        cout << "Opcion invalida.\n";
        return;
    }
    saveConfig(file, config);
}

int main() {
    const string CONFIG_FILE  = "../data/config.txt";
    const string MATCHES_FILE = "../data/partidos.txt";
    const string DAYS_FILE    = "../data/fechas.txt";

    LeagueConfig config;
    if(!loadConfig(CONFIG_FILE, config)) {
        return 1;
    }

    cout << "\nLiga cargada: " << config.leagueName << "\n";
    cout << "Equipos: " << config.clubs.size() << "\n";

    int option;
    do {
        option = showMenu(config.leagueName);
        switch(option) {
            case 1: {
                vector<Match> matches;
                loadMatches(MATCHES_FILE, matches);
                vector<Club> table = buildTable(matches, config);
                sortTable(table);
                printTable(table);
                break;
            }
            case 2: {
                Match m = inputMatch(config);
                cout << "\n--- Confirmacion ---\n";
                cout << m.home << "  " << m.homeGoals << " - " << m.awayGoals << "  " << m.away << "\n";
                cout << "Guardar? (s/n): ";
                char confirm;
                cin >> confirm;
                cin.ignore(1000, '\n');
                if(confirm == 's' || confirm == 'S') {
                    appendMatch(MATCHES_FILE, m);
                    updateMatchDay(DAYS_FILE, m);
                    cout << "Partido registrado.\n";
                }
                break;
            }
            case 3: {
                showMatchHistory(DAYS_FILE);
                break;
            }
            case 4: {
                vector<Match> matches;
                loadMatches(MATCHES_FILE, matches);
                printAllMatches(matches);
                break;
            }
            case 5: {
                cout << "\nHasta luego.\n";
                break;
            }
        }
    } while(option != 5);
    return 0;
}