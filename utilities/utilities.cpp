//
// Created by Eric Dattore on 8/24/18.
//
#include "utilities.hpp"

namespace utilities {
    double cap(double value, int cap) {
        if (value > cap) { return cap; }
        else { return value; }
    }

    double stat_reader(std::string _data) {
        double number;
        std::istringstream sstr(_data);
        if (!(sstr >> number)) {
            number = -1;
        }
        return number;
    }

    std::string username_stripper(std::string &username) {
        int pos = username.find("_");
        if (pos != -1) {
            username = username.substr(pos + 1, username.size() - pos);
        }
        pos = username.find("-US-");
        if (pos != -1) {
            username = username.substr(0, pos);
        } else {
            pos = username.find(".");
            if (pos != -1) {
                username = username.substr(0, pos);
            }
        }
        return username;
    }

    void BF1API(std::string name) {
        auto r = cpr::Get(
                cpr::Url{"https://battlefieldtracker.com/bf1/api/Stats/DetailedStats?platform=3&displayName=" + name},
                cpr::VerifySsl{false}, cpr::Header{{"TRN-Api-Key", "152de4e7-22a4-4a0a-b2b6-2bea2c7886b6"}});
        std::cout << r.text << '\n';
        std::cout << r.status_code << '\n';
    }

    void API() {
        std::string st;
        std::vector<int> size;
        std::vector<std::string> lstupdate;
        std::vector<std::string> dirname;
        std::string inconsistency;
        std::cout << "Perform Update Inconsistency Analysis? ";
        getline(std::cin, inconsistency);
        std::cout << '\n';
        if (inconsistency == "Y" || inconsistency == "y") {
            while (true) {
                std::cout << "Type in the database directory you're tracking your stats from: ";
                getline(std::cin, st);
                std::cout << '\n';
                if (st == "exit") { break; }
                std::string url = "http://unkso.michaeljamesbondparsons.com/stats/bf1/latest?events[0]=" + st;
                auto r = cpr::Get(cpr::Url{url}, cpr::Header{{"content-type",  "application/json"},
                                                             {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
                if (r.text != "{\"players\":[]}") {
                    json pack = json::parse(r.text);
                    pack = pack["players"];
                    std::cout << "Player " << "|" << " Updated at" << '\n';
                    for (int j = 0; j < pack.size(); j++) {
                        std::cout << pack[j]["general"]["updated_at"] << '\n';
                    }
                }
            }
        }
        while (true) {
            std::cout << "Type in the database directory you're tracking your stats from: ";
            getline(std::cin, st);
            std::cout << '\n';
            if (st == "exit") { break; }
            std::string url = "http://unkso.michaeljamesbondparsons.com/stats/bf1/latest?events[0]=" + st;
            auto r = cpr::Get(cpr::Url{url}, cpr::Header{{"content-type",  "application/json"},
                                                         {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
            if (r.text != "{\"players\":[]}") {
                json pack = json::parse(r.text);
                pack = pack["players"];
                size.push_back(pack.size());
                lstupdate.push_back(pack[1]["general"]["updated_at"]);
                dirname.push_back(st);
            }
        }
        std::cout << "Directory Name | Nº of Players | Last Update " << '\n';
        for (int i = 0; i < dirname.size(); i++) {
            std::cout << std::setw(14) << dirname[i] << " | " << std::setw(13) << size[i] << " players" << " | " << std::setw(11)
                 << lstupdate[i] << '\n';
        }
    }
}