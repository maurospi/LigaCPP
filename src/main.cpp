#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

struct LeagueConfig {
    string leagueName;
    int winPts;
    int lossPts;
    int drawPts;
    vector<string> teams;
};

struct Match {
    string date;
    string home;
    string away;
    int homeGoals;
    int awayGoals;
};

struct Match {
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
    if(start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool loadConfig(const string& file, LeagueConfig& config) {
    ifstream configFile(file);
    if(!configFile.is_open()) {
        cerr << "ERROR: No se pudo abrir '" << file << "'.\n";
        return false;
    }

    bool hasName = false, hasWin = false, hasLoss = false, hasDraw = false;
    string line;

    while(getline(configFile, line)) {
        line = trim(line);
        if(line.empty() || line[0] == '#') continue;

        size_t pos = line.find('=');
        if(pos == string::npos) continue;

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
        } else if(key == "club") {
            if(!value.empty()) config.teams.push_back(value);
        }
    }
    configFile.close();

    if(!hasName || !hasWin || !hasDraw || !hasLoss) {
        cerr << "ERROR: config.txt le faltan campos obligatorios.\n";
        return false;
    }
    if(config.teams.empty()) {
        cerr << "ERROR: No hay equipos registrados.\n";
        return false;
    }
    return true;
}

int main() {
    LeagueConfig config;

    if(!loadConfig("data/config.txt", config)) {
        cout << "No se pudo cargar la configuracion.\n";
        return 1;
    }

    cout << "Liga: "     << config.leagueName << "\n";
    cout << "Victoria: " << config.winPts  << " pts\n";
    cout << "Empate: "   << config.drawPts << " pts\n";
    cout << "Derrota: "  << config.lossPts << " pts\n";
    cout << "Equipos: "  << config.teams.size() << "\n";
    for(int i = 0; i < (int)config.teams.size(); i++) {
        cout << "  - " << config.teams[i] << "\n";
    }
    cout << "Hoy es: " << getTodayDate() << "\n";

    return 0;
}