// UNKSOTracker.cpp : Defines the entry point for the console application.
// Coded by PO1.JimAlmeida on behalf of the MWR Department in the Intelligence Corps
#include <cpr/cpr.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <functional>
#include <vector>
#include <exception>
#include <fstream>
#include <map>
#include <cmath>
#include <utility>
#include "headers/json.hpp"
#include <iomanip>
#include <locale.h>
#include <chrono>
#include <thread>
#include <ctime>
#include <numeric>
#include <algorithm>

using json = nlohmann::json;
using std::cout;
using std::cin;
using std::string;
using std::vector;

namespace utilities {
    double cap(double value, int cap) {
        if (value > cap) { return cap; }
        else { return value; }
    }

    double statreader(string _data) {
        double number;
        std::istringstream sstr(_data);
        if (!(sstr >> number)) {
            number = -1;
        }
        return number;
    }

    template<typename T>
    double branchperfcalculator(T container) {
        try {
            if (container.size() != 0) {
                std::vector<double> playingfrequency;
                double variancecoefficient;
                double scoreaverage = 0, percentileaverage = 0, stddeviation = 0, variance = 0;
                for (int j = 0; j < container.size(); j++) {
                    scoreaverage += container[j]->GetPIndex();
                    playingfrequency.push_back(container[j]->GetRP());
                }
                scoreaverage /= container.size();
                std::sort(playingfrequency.begin(), playingfrequency.end());
                std::reverse(playingfrequency.begin(), playingfrequency.end());
                int numb = playingfrequency.size() * 0.6;
                for (int k = 0; k < numb; k++) {
                    percentileaverage += playingfrequency[k];
                }
                percentileaverage /= numb;
                for (int z = 0; z < playingfrequency.size(); z++) {
                    variance += std::pow((playingfrequency[z] - percentileaverage), 2);
                }
                variance /= playingfrequency.size();
                stddeviation = std::sqrt(variance);
                variancecoefficient = stddeviation / percentileaverage;
                cout << "-------------" << '\n';
                cout << scoreaverage << '\n';
                cout << numb << '\n';
                cout << percentileaverage << '\n';
                cout << variance << '\n';
                cout << stddeviation << '\n';
                cout << variancecoefficient << '\n';
                if (variancecoefficient < 1) {
                    return (1 - variancecoefficient) * scoreaverage;
                } else return 0;
            } else return 0;
        }
        catch (std::exception &e) {
            std::cerr << e.what() << '\n';
            return -1;
        }
    }

    string username_stripper(string &username) {
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
}

class DataFetcher {
protected:
    std::string sourcecode;
    int statuscode;
public:
    explicit DataFetcher(string url) {
        try {
            auto r = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
            statuscode = r.status_code;
            sourcecode = r.text;
        }
        catch (std::exception &e) {
            std::cerr << e.what();
            sourcecode = "EmptyData";
        }
    }

    DataFetcher(string url, string authparameter, string apikey) {
        try {
            auto r = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false}, cpr::Header{{authparameter, apikey}});
            sourcecode = r.text;
            statuscode = r.status_code;
        }
        catch (std::exception &e) {
            std::cerr << e.what();
            sourcecode = "EmptyData";
        }
    }

    std::string GetSource() { return sourcecode; }

    void SalvaDisco(string filename) {
        string filepath = filename + ".txt";
        std::ofstream fout(filepath);
        fout << sourcecode;
        fout.close();
    }

    void pre_processed_disk_exporter(string filename) {
        string filepath = filename + ".txt";
        int pos;
        //
        pos = sourcecode.find("Please activate javascript to see everything.");
        sourcecode.erase(0, pos);
        //
        std::ofstream fout(filepath);
        fout << sourcecode;
        fout.close();
    }

    friend std::ostream &operator<<(std::ostream &os, DataFetcher &obj) {
        cout << obj.sourcecode;
        return os;
    }

    int GStatusCode() { return statuscode; }
};

class BF4Tracker {
protected:
    DataFetcher *dft;
    string username;
    json database;
    //Old stats
    double ss;
    double rp;
    double sa;
    double ka;
    double rsp;
    double rps;
    double hls;
    double rvs;
    double ptfo;
    double wins;
    double kills;
    double hs;
    double acc;
    double dgtgs;
    //Newly updated stats
    double nss;
    double nrp;
    double nsa;
    double nka;
    double nrsp;
    double nrps;
    double nhls;
    double nrvs;
    double nptfo;
    double nwins;
    double nkills;
    double nhs;
    double nacc;
    double ndgtgs;
    //Delta Values (New - Old)
    double dss;
    double drp;
    double dsa;
    double dka;
    double drsp;
    double drps;
    double dhls;
    double drvs;
    double dptfo;
    double dwins;
    double dkills;
    double dhs;
    double dacc;
    double ddgtgs;
    //Other variables
    double supporteff;
    double mediceff;
    double assisteff;
    double perfindex;
    //Control Variables
    bool flag;
    double indexss;
    double indexrp;
    double indexts;
    double indexwns;
    double indexcts;
    double indexptfo;
public:
    ~BF4Tracker() { delete dft; }

    BF4Tracker(string _username) : username(_username) {
        //Reading updated data
        dft = new DataFetcher("http://api.bf4stats.com/api/playerInfo?plat=pc&name=" + username + "&output=json");
        if (dft->GStatusCode() == 200) {
            cout << "------------------------------------------------------------------------" << '\n';
            cout << "All statistics have been downloaded for player " << username << '\n';
            cout << "------------------------------------------------------------------------" << '\n';
            database = json::parse(dft->GetSource());
            nss = database["stats"]["scores"]["squad"];
            nrp = database["stats"]["extra"]["roundsFinished"];
            nsa = database["stats"]["suppressionAssists"];
            nka = database["stats"]["killAssists"];
            nrsp = database["stats"]["resupplies"];
            nrps = database["stats"]["repairs"];
            nhls = database["stats"]["heals"];
            nrvs = database["stats"]["revives"];
            nwins = database["stats"]["numWins"];
            nkills = database["stats"]["kills"];
            nhs = database["stats"]["headshots"];
            nacc = database["stats"]["extra"]["accuracy"];
            ndgtgs = database["weaponCategory"][9]["stat"]["kills"];
            //calculating ptfo factor
            std::vector<int> modescore;
            for (int i = 0; i < 4; i++) {
                modescore.push_back(database["stats"]["modes"][i]["score"]);
            }
            nptfo = std::accumulate(modescore.begin(), modescore.end(), 0);
            //end of calculation
            //end of updated data
            flag = true;
            //Reading saved data
            readsaveddata();
            indexptfo = 0;
        } else {
            cout << "Something went wrong when fetching user's data from the online BF4 API" << '\n';
        }
    }

    void readsaveddata() {
        //
        string processed_username;
        processed_username = utilities::username_stripper(username);
        //
        auto r = cpr::Get(
                cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats/bf4/latest?players[0]=" + processed_username},
                cpr::Header{{"content-type",  "application/json"},
                            {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
        if (r.status_code == 200) {
            if (r.text != "[]") {
                json statpack;
                statpack = json::parse(r.text);
                cout << "This player's stats have been found in UNSKO Online Database" << '\n';
                ss = statpack[processed_username]["general"]["squadscore"];
                rp = statpack[processed_username]["general"]["roundsplayed"];
                sa = statpack[processed_username]["general"]["suppressionassists"];
                ka = statpack[processed_username]["general"]["killassists"];
                rsp = statpack[processed_username]["general"]["resupplies"];
                rps = statpack[processed_username]["general"]["repairs"];
                hls = statpack[processed_username]["general"]["heals"];
                rvs = statpack[processed_username]["general"]["revives"];
                ptfo = statpack[processed_username]["general"]["ptfo"];
                wins = statpack[processed_username]["general"]["wins"];
                kills = statpack[processed_username]["general"]["kills"];
                hs = statpack[processed_username]["general"]["headshots"];
                acc = statpack[processed_username]["general"]["accuracy"];
            } else {
                cout << "This player has not been found in UNKSO Online Database" << '\n';
                flag = false;
            }
        } else {
            cout << "Something went wrong when trying to establish connection to WEB API. HTTP Status CODE: "
                 << r.status_code << '\n';
            flag = false;
        }
    }

    void savedata() {
        std::ofstream fout("TrackerData\\BF4\\" + username + ".txt");
        fout << "## LIST OF STATISTICS ##" << '\n';
        fout << std::fixed << std::setprecision(2);
        fout << nss << '\n';
        fout << nrp << '\n';
        fout << nsa << '\n';
        fout << nka << '\n';
        fout << nrsp << '\n';
        fout << nrps << '\n';
        fout << nhls << '\n';
        fout << nrvs << '\n';
        fout << nptfo << '\n';
        fout << nwins << '\n';
        fout << nkills << '\n';
        fout << nhs << '\n';
        fout << nacc << '\n';
        fout << ndgtgs << '\n';
        fout.close();
    }

    void Variation() {
        dss = nss - ss;
        drp = nrp - rp;
        dsa = nsa - sa;
        dka = nka - ka;
        drsp = nrsp - rsp;
        drps = nrps - rps;
        dhls = nhls - hls;
        drvs = nrvs - rvs;
        dptfo = nptfo - ptfo;
        dwins = nwins - wins;
        dkills = nkills - kills;
        dhs = nhs - hs;
        dacc = nacc - acc;
        ddgtgs = ndgtgs - dgtgs;
    }

    void PerfIndexBF4(bool outputflag = true) {
        if (flag == true) {
            Variation();
            supporteff = (drsp + drps) / 2;
            mediceff = (dhls + drvs) / 2;
            assisteff = (dsa + dka) / 2;
            indexss = ((dss) / (50 * log10(dss)));
            indexss = utilities::cap(indexss, 250);
            indexrp = (10 * pow(drp, 1.2));
            indexrp = utilities::cap(indexrp, 300);
            indexts = supporteff + mediceff + assisteff;
            indexts = utilities::cap(indexts, 150);
            indexptfo = ((dptfo) / (400 * log10(dptfo)));
            indexptfo = utilities::cap(indexptfo, 200);
            indexcts = 0.25 * dkills / log10(dkills) + (0.15 * sqrt(dhs));
            indexcts = utilities::cap(indexcts, 100);
            perfindex = (indexss + indexrp + indexts + indexptfo + indexcts);
            if (outputflag == true) {
                int op;
                cout << "========================================================================" << '\n';
                cout << "The Performance Index has been calculated. Choose an option to see the results." << '\n';
                cout << "1 - Basic Report" << '\n';
                cout << "2 - Detailed Report" << '\n';
                cout << "3 - Full Report" << '\n';
                cout << "========================================================================" << '\n';
                cout << "Input: ";
                cin >> op;
                cin.get();
                cout << "========================================================================" << '\n';
                switch (op) {
                    case 1: {
                        cout << std::left;
                        cout << "Performance Index Basic Report" << '\n';
                        cout << "Player: " << username << '\n';
                        cout << "Perf.Index: " << perfindex << '\n';
                        cout << "End of Report" << '\n';
                        cout << "========================================================================" << '\n';
                        break;
                    }
                    case 2: {
                        cout << std::left;
                        cout << "Performance Index Detailed Report" << '\n';
                        cout << "Player: " << username << '\n';
                        cout << "Perf.Index: " << perfindex << '\n';
                        cout << "-----------------------------------" << '\n';
                        cout << "Individual Index Components" << '\n';
                        cout << "Score Index: " << indexss << '\n';
                        cout << "Team Stats Index: " << indexts << '\n';
                        cout << "Combat Stats Index: " << indexcts << '\n';
                        cout << "Activity Index (Rounds Played): " << indexrp << '\n';
                        cout << "PTFO Index: " << indexptfo << '\n';
                        cout << "End of Report" << '\n';
                        cout << "========================================================================" << '\n';
                        break;
                    }
                    case 3: {
                        cout << std::left;
                        cout << "Performance Index Full Report" << '\n';
                        cout << "Player: " << username << '\n';
                        cout << "Perf.Index: " << perfindex << '\n';
                        cout << "-----------------------------------" << '\n';
                        cout << "Individual Index Components" << '\n';
                        cout << "Score Index: " << indexss << '\n';
                        cout << "Team Stats Index: " << indexts << '\n';
                        cout << "Combat Stats Index: " << indexcts << '\n';
                        cout << "Activity Index (Rounds Played): " << indexrp << '\n';
                        cout << "PTFO Index: " << indexptfo << '\n';
                        cout << "-----------------------------------" << '\n';
                        cout << "Delta Factor Components" << '\n';
                        cout << "DScore: " << dss << '\n';
                        cout << "DRoundsPlayed: " << drp << '\n';
                        cout << "DSuppressionAssists: " << dsa << '\n';
                        cout << "DKillAssists: " << dka << '\n';
                        cout << "DResupplies: " << drsp << '\n';
                        cout << "DRevives: " << drvs << '\n';
                        cout << "DRepairs: " << drps << '\n';
                        cout << "DHeals: " << dhls << '\n';
                        cout << "DKills: " << dkills << '\n';
                        cout << "DHeadshots: " << dhs << '\n';
                        cout << "Accuracy: " << dacc << '\n';
                        cout << "DDogtagsTaken: " << ddgtgs << '\n';
                        cout << "DPTFO: " << dptfo << '\n';
                        cout << "End of Report" << '\n';
                        cout << "========================================================================" << '\n';
                        break;
                    }
                }
            }
        }
    }

    json exporter() {
        // time stamp
        time_t now = time(0);
        string time = ctime(&now);
        // json assembling
        json export_package;
        export_package["lastupdate"] = time;
        export_package["squad_score"] = nss;
        export_package["rounds_played"] = nrp;
        export_package["suppression_assists"] = nsa;
        export_package["kill_assists"] = nka;
        export_package["repairs"] = nrps;
        export_package["resupplies"] = nrsp;
        export_package["heals"] = nhls;
        export_package["revives"] = nrvs;
        export_package["ptfo"] = nptfo;
        export_package["wins"] = nwins;
        export_package["kills"] = nkills;
        export_package["headshots"] = nhs;
        export_package["accuracy"] = nacc;
        return export_package;
    }

    int GetRP() { return drp; }

    double GetPIndex() { return perfindex; }

    friend class Interface;
};

namespace BF1 {
    class BF1Stats {
    protected:
        DataFetcher *dft_gs; //general stats
        DataFetcher *dft_ws; //weapons stats
        DataFetcher *dft_vs; //vehicle stats
    public:
        json gen_stats;
        json wpon_stats;
        json vhcl_stats;
        std::vector<double> remainingstats;

        BF1Stats(string username) {
            cout << "------------------------------------------------------------------------" << '\n';
            dft_gs = new DataFetcher(
                    "https://battlefieldtracker.com/bf1/api/Stats/DetailedStats?platform=3&displayName=" + username,
                    "TRN-Api-Key", "152de4e7-22a4-4a0a-b2b6-2bea2c7886b6");
            cout << "General Stats have been downloaded for " << username << '\n';
            gen_stats = json::parse(dft_gs->GetSource());
            dft_gs = nullptr;
            cout << "------------------------------------------------------------------------" << '\n';
            dft_ws = new DataFetcher(
                    "https://battlefieldtracker.com/bf1/api/Progression/GetWeapons?platform=3&displayName=" + username +
                    "&game=tunguska", "TRN-Api-Key", "152de4e7-22a4-4a0a-b2b6-2bea2c7886b6");
            cout << "Weapon Stats have been downloaded for " << username << '\n';
            wpon_stats = json::parse(dft_ws->GetSource());
            dft_ws = nullptr;
            cout << "------------------------------------------------------------------------" << '\n';
            dft_vs = new DataFetcher(
                    "https://battlefieldtracker.com/bf1/api/Progression/GetVehicles?platform=3&displayName=" +
                    username + "&game=tunguska", "TRN-Api-Key", "152de4e7-22a4-4a0a-b2b6-2bea2c7886b6");
            cout << "Vehicle Stats have been downloaded for " << username << '\n';
            vhcl_stats = json::parse(dft_vs->GetSource());
            dft_vs = nullptr;
            cout << "------------------------------------------------------------------------" << '\n';
        }

        ~BF1Stats() {
            delete dft_gs;
            delete dft_ws;
            delete dft_vs;
        }
    };

    class Player {
    public:
        BF1Stats *sts;
        DataFetcher *dfetcher;
        string username;
        //Stats
        double wins;
        double losses;
        double wlratio;
        double score;
        double kdr;
        double spm;
        double kpm;
        double hs_killP;
        double kill_hitP;
        double sfm;
        double hshots;
        double kassists;
        double rplayed;
        double kills;
        double deaths;
        double accuracy;
        double avenger_kills;
        double savior_kills;
        double nemesis_kills;
        double nemesis_streak;
        double longest_hs;
        double sassists;
        double repairs;
        double heals;
        double revives;
        double flag_cap;
        double flag_def;
        double squad_score;
        double time_played;
        double s_hit;
        double s_fired;
        double meleekills;
        // include killstreak in the future??
        //Kit Stats
        std::vector<std::vector<double> > kstats;
        std::vector<std::vector<double> > gmstats;
        //Weapon Stats
        std::vector<std::vector<double> > weapons;
        //Additional Statistics for Parsing Operation
        std::vector<string> label_stats;
        std::vector<double> addt_stats;

        /*
		addt_stats[0] = Infantry Kills
		addt_stats[1] = Vehicle Kills
		addt_stats[2] = Resupplies
		addt_stats[3] = Ace Squad
		addt_stats[4] = MVP
		addt_stats[5] = Orders Completed
		addt_stats[6] = Squad Spawns
		addt_stats[7] = Squad Wipes
		*/

        ~Player() {
            delete sts;
            delete dfetcher;
        }

        Player(string _username) : username(_username) {
            //Gen Stats
            sts = new BF1Stats(username);
            wins = sts->gen_stats["result"]["basicStats"]["wins"];
            losses = sts->gen_stats["result"]["basicStats"]["losses"];
            spm = sts->gen_stats["result"]["basicStats"]["spm"];
            hshots = sts->gen_stats["result"]["headShots"];
            kassists = sts->gen_stats["result"]["killAssists"];
            kills = sts->gen_stats["result"]["basicStats"]["kills"];
            deaths = sts->gen_stats["result"]["basicStats"]["deaths"];
            accuracy = sts->gen_stats["result"]["accuracyRatio"];
            avenger_kills = sts->gen_stats["result"]["avengerKills"];
            savior_kills = sts->gen_stats["result"]["saviorKills"];
            nemesis_kills = sts->gen_stats["result"]["nemesisKills"];
            nemesis_streak = sts->gen_stats["result"]["nemesisKillStreak"];
            longest_hs = sts->gen_stats["result"]["longestHeadShot"];
            sassists = sts->gen_stats["result"]["suppressionAssist"];
            repairs = sts->gen_stats["result"]["repairs"];
            heals = sts->gen_stats["result"]["heals"];
            revives = sts->gen_stats["result"]["revives"];
            flag_cap = sts->gen_stats["result"]["flagsCaptured"];
            flag_def = sts->gen_stats["result"]["flagsDefended"];
            squad_score = sts->gen_stats["result"]["squadScore"];
            time_played = sts->gen_stats["result"]["basicStats"]["timePlayed"];
            wlratio = wins / losses;
            score = spm * (time_played / 60);
            kdr = kills / deaths;
            kpm = (kills / (time_played / 60));
            hs_killP = (hshots / kills) * 100;
            rplayed = wins + losses;
            //Kit Stats
            std::vector<double> single_kit;
            double xscore, xtime, xkills;
            for (int i = 0; i < 7; i++) {
                xscore = sts->gen_stats["result"]["kitStats"][i]["score"];
                xtime = sts->gen_stats["result"]["kitStats"][i]["secondsAs"];
                xkills = sts->gen_stats["result"]["kitStats"][i]["kills"];
                single_kit.push_back(xscore);
                single_kit.push_back(xtime);
                single_kit.push_back(xkills);
                kstats.push_back(single_kit);
                single_kit.clear();
            }
            //End of Kit Stats
            //Game Mode Stats
            std::vector<double> singlemodestats;
            string st;
            double xwins;
            double xloss;
            for (int j = 0; j < 6; j++) {
                if (sts->gen_stats["result"]["gameModeStats"][j]["name"] == "Rush"
                    || sts->gen_stats["result"]["gameModeStats"][j]["name"] == "TeamDeathMatch"
                    || sts->gen_stats["result"]["gameModeStats"][j]["name"] == "Conquest"
                    || sts->gen_stats["result"]["gameModeStats"][j]["name"] == "Domination") {
                    xscore = sts->gen_stats["result"]["gameModeStats"][j]["score"];
                    xwins = sts->gen_stats["result"]["gameModeStats"][j]["wins"];
                    xloss = sts->gen_stats["result"]["gameModeStats"][j]["losses"];
                    singlemodestats.push_back(xscore);
                    singlemodestats.push_back(xwins);
                    singlemodestats.push_back(xloss);
                    gmstats.push_back(singlemodestats);
                    singlemodestats.clear();
                }
            }
            //End of Game Mode Stats
            //END of GenStats
            //WeaponStats
            meleekills = 0;
            std::vector<double> singleweapon;
            double numb;
            for (int a = 0; a < 5; a++) {
                numb = sts->wpon_stats["result"][0]["weapons"][a]["stats"]["values"]["kills"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][0]["weapons"][a]["stats"]["values"]["shots"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][0]["weapons"][a]["stats"]["values"]["hits"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][0]["weapons"][a]["stats"]["values"]["headshots"];
                singleweapon.push_back(numb);
                weapons.push_back(singleweapon);
                singleweapon.clear();
            }
            for (int a = 0; a < 27; a++) {
                numb = sts->wpon_stats["result"][1]["weapons"][a]["stats"]["values"]["kills"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][1]["weapons"][a]["stats"]["values"]["shots"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][1]["weapons"][a]["stats"]["values"]["hits"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][1]["weapons"][a]["stats"]["values"]["headshots"];
                singleweapon.push_back(numb);
                weapons.push_back(singleweapon);
                singleweapon.clear();
            }
            for (int a = 0; a < 38; a++) {
                numb = sts->wpon_stats["result"][2]["weapons"][a]["stats"]["values"]["kills"];
                meleekills += numb;
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][2]["weapons"][a]["stats"]["values"]["shots"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][2]["weapons"][a]["stats"]["values"]["hits"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][2]["weapons"][a]["stats"]["values"]["headshots"];
                singleweapon.push_back(numb);
                weapons.push_back(singleweapon);
                singleweapon.clear();
            }
            for (int a = 0; a < 29; a++) {
                numb = sts->wpon_stats["result"][3]["weapons"][a]["stats"]["values"]["kills"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][3]["weapons"][a]["stats"]["values"]["shots"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][3]["weapons"][a]["stats"]["values"]["hits"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][3]["weapons"][a]["stats"]["values"]["headshots"];
                singleweapon.push_back(numb);
                weapons.push_back(singleweapon);
                singleweapon.clear();
            }
            for (int a = 0; a < 28; a++) {
                try {
                    json subpack;
                    subpack = sts->wpon_stats["result"][4]["weapons"][a]["stats"]["values"];
                    numb = subpack.at("kills");
                    singleweapon.push_back(numb);
                    numb = subpack.at("shots");
                    singleweapon.push_back(numb);
                    numb = subpack.at("hits");
                    singleweapon.push_back(numb);
                    numb = subpack.at("headshots");
                    singleweapon.push_back(numb);
                    weapons.push_back(singleweapon);
                    singleweapon.clear();
                }
                catch (json::out_of_range &e) {}
            }
            for (int a = 0; a < 27; a++) {
                numb = sts->wpon_stats["result"][5]["weapons"][a]["stats"]["values"]["kills"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][5]["weapons"][a]["stats"]["values"]["shots"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][5]["weapons"][a]["stats"]["values"]["hits"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][5]["weapons"][a]["stats"]["values"]["headshots"];
                singleweapon.push_back(numb);
                weapons.push_back(singleweapon);
                singleweapon.clear();
            }
            for (int a = 0; a < 12; a++) {
                try {
                    json subpack;
                    subpack = sts->wpon_stats["result"][6]["weapons"][a]["stats"]["values"];
                    numb = subpack.at("kills");
                    singleweapon.push_back(numb);
                    numb = subpack.at("shots");
                    singleweapon.push_back(numb);
                    numb = subpack.at("hits");
                    singleweapon.push_back(numb);
                    numb = subpack.at("headshots");
                    singleweapon.push_back(numb);
                    weapons.push_back(singleweapon);
                    singleweapon.clear();
                }
                catch (json::out_of_range &e) {}
            }
            for (int a = 0; a < 6; a++) {
                numb = sts->wpon_stats["result"][7]["weapons"][a]["stats"]["values"]["kills"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][7]["weapons"][a]["stats"]["values"]["shots"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][7]["weapons"][a]["stats"]["values"]["hits"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][7]["weapons"][a]["stats"]["values"]["headshots"];
                singleweapon.push_back(numb);
                weapons.push_back(singleweapon);
                singleweapon.clear();
            }
            for (int a = 0; a < 12; a++) {
                numb = sts->wpon_stats["result"][8]["weapons"][a]["stats"]["values"]["kills"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][8]["weapons"][a]["stats"]["values"]["shots"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][8]["weapons"][a]["stats"]["values"]["hits"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][8]["weapons"][a]["stats"]["values"]["headshots"];
                singleweapon.push_back(numb);
                weapons.push_back(singleweapon);
                singleweapon.clear();
            }
            for (int a = 0; a < 8; a++) {
                numb = sts->wpon_stats["result"][9]["weapons"][a]["stats"]["values"]["kills"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][9]["weapons"][a]["stats"]["values"]["shots"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][9]["weapons"][a]["stats"]["values"]["hits"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][9]["weapons"][a]["stats"]["values"]["headshots"];
                singleweapon.push_back(numb);
                weapons.push_back(singleweapon);
                singleweapon.clear();
            }
            for (int a = 0; a < 15; a++) {
                numb = sts->wpon_stats["result"][10]["weapons"][a]["stats"]["values"]["kills"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][10]["weapons"][a]["stats"]["values"]["shots"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][10]["weapons"][a]["stats"]["values"]["hits"];
                singleweapon.push_back(numb);
                numb = sts->wpon_stats["result"][10]["weapons"][a]["stats"]["values"]["headshots"];
                singleweapon.push_back(numb);
                weapons.push_back(singleweapon);
                singleweapon.clear();
            }
            for (int a = 0; a < 21; a++) {
                std::vector<double> nullwpon = {0, 0, 0, 0};
                try {
                    json subpack;
                    subpack = sts->wpon_stats["result"][11]["weapons"][a]["stats"]["values"];
                    numb = subpack.at("kills");
                    singleweapon.push_back(numb);
                    numb = subpack.at("shots");
                    singleweapon.push_back(numb);
                    numb = subpack.at("hits");
                    singleweapon.push_back(numb);
                    numb = subpack.at("headshots");
                    singleweapon.push_back(numb);
                    weapons.push_back(singleweapon);
                    singleweapon.clear();
                }
                catch (std::exception &e) { weapons.push_back(nullwpon); }
            }
            //End of Weapon Stats
            Parser();
            s_hit = 0;
            s_fired = 0;
            for (int j = 0; j < weapons.size(); j++) {
                s_fired += weapons[j][1];
                s_hit += weapons[j][2];
            }
            kill_hitP = kills / s_hit;
            sfm = s_fired / time_played * 60;
            sts = nullptr;
        }

        void Parser() {
            string data;
            double numb;
            bool flag;
            std::ifstream fin("BF1GenStats.txt");
            while (!fin.eof()) {
                getline(fin, data);
                label_stats.push_back(data);
            }
            fin.close();;
            dfetcher = new DataFetcher("https://battlefieldtracker.com/bf1/profile/pc/" + username);
            cout << "Additional General Stats have been downloaded for " << username << '\n';
            dfetcher->SalvaDisco("bf1sourcecode");
            fin.open("bf1sourcecode.txt");
            while (!fin.eof()) {
                getline(fin, data);
                flag = statsfinder(data);
                if (flag == true) {
                    getline(fin, data);
                    data.erase(std::remove(data.begin(), data.end(), ','), data.end());
                    numb = statreader(data);
                    if (numb != -1) { addt_stats.push_back(numb); }
                }
            }
            fin.close();
            cout << "------------------------------------------------------------------------" << '\n';
        }

        bool statsfinder(string _data) {
            for (string &s : label_stats) {
                if (_data == s) {
                    return true;
                }
            }
            return false;
        }

        double statreader(string _data) {
            double number;
            std::istringstream sstr(_data);
            if (!(sstr >> number)) {
                number = -1;
            }
            return number;
        }

        friend std::ostream &operator<<(std::ostream &os, Player *plr_ptr) {
            string spacer = " | ";
            string linebreaker = "-----------------------------------------------------------------------------------------------------------------------------------";
            try {
                os << std::fixed << std::setprecision(2) << '\n';
                os << linebreaker << '\n';
                os << "                                      Showing general stats for player: " << plr_ptr->username
                   << '\n';
                os << linebreaker << '\n';
                os << std::right;
                os << std::setw(20) << "Kills" << spacer << std::setw(20) << "Deaths" << spacer << std::setw(14)
                   << "KDR" << spacer << std::setw(14) << "Rounds Played" << spacer << std::setw(12) << "Time Played"
                   << spacer << std::setw(13) << "Kill Assists" << spacer << std::setw(20) << "Suppression Assists"
                   << '\n';
                os << std::setw(20) << plr_ptr->kills << spacer << std::setw(20) << plr_ptr->deaths << spacer
                   << std::setw(14) << plr_ptr->kdr << spacer << std::setw(14) << plr_ptr->rplayed << spacer
                   << std::setw(12) << plr_ptr->time_played << spacer << std::setw(13) << plr_ptr->kassists << spacer
                   << std::setw(20) << plr_ptr->sassists << '\n';
                os << linebreaker << '\n';
                os << std::setw(20) << "Wins" << spacer << std::setw(20) << "Losses" << spacer << std::setw(14)
                   << "W/L Ratio" << spacer << std::setw(14) << "Score" << spacer << std::setw(12) << "SFM" << spacer
                   << std::setw(13) << "SPM" << spacer << std::setw(20) << "KPM" << '\n';
                os << std::setw(20) << plr_ptr->wins << spacer << std::setw(20) << plr_ptr->losses << spacer
                   << std::setw(14) << plr_ptr->wlratio << spacer << std::setw(14) << plr_ptr->score << spacer
                   << std::setw(12) << plr_ptr->sfm << spacer << std::setw(13) << plr_ptr->spm << spacer
                   << std::setw(20) << plr_ptr->kpm << '\n';
                os << linebreaker << '\n';
                os << std::setw(20) << "HS/Kill Percentage" << spacer << std::setw(20) << "Kill_Hit Percentage"
                   << spacer << std::setw(14) << "Shots Fired" << spacer << std::setw(14) << "Shots Hit" << spacer
                   << std::setw(12) << "Accuracy" << spacer << std::setw(13) << "Headshots" << spacer << std::setw(20)
                   << "Longest Headshot" << '\n';
                os << std::setw(20) << plr_ptr->hs_killP << spacer << std::setw(20) << plr_ptr->kill_hitP << spacer
                   << std::setw(14) << plr_ptr->s_fired << spacer << std::setw(14) << plr_ptr->s_hit << spacer
                   << std::setw(12) << plr_ptr->accuracy << spacer << std::setw(13) << plr_ptr->hshots << spacer
                   << std::setw(20) << plr_ptr->longest_hs << '\n';
                os << linebreaker << '\n';
                os << std::setw(20) << "Avenger Kills" << spacer << std::setw(20) << "Savior Kills" << spacer
                   << std::setw(14) << "Nemesis Kills" << spacer << std::setw(14) << "Repairs" << spacer
                   << std::setw(12) << "Heals" << spacer << std::setw(13) << "Revives" << spacer << std::setw(20)
                   << "Resupplies" << '\n';
                os << std::setw(20) << plr_ptr->avenger_kills << spacer << std::setw(20) << plr_ptr->savior_kills
                   << spacer << std::setw(14) << plr_ptr->nemesis_kills << spacer << std::setw(14) << plr_ptr->repairs
                   << spacer << std::setw(12) << plr_ptr->heals << spacer << std::setw(13) << plr_ptr->revives << spacer
                   << std::setw(20) << plr_ptr->addt_stats[2] << '\n';
                os << linebreaker << '\n';
                return os;
            }
            catch (std::exception &e) {
                cout << "An error has occurred!" << '\n';
                std::cerr << e.what() << '\n';
                return os;
            }
        }

        friend class BF1Tracker;
    };

    class BF1Tracker : public Player {
    public:
        //Old stats
        double oss;
        double orp;
        double osa;
        double oka;
        double orsp;
        double orps;
        double ohls;
        double orvs;
        double optfo;
        double owins;
        double okills;
        double ohs;
        double oacc;
        double odgtgs;
        //Newly updated stats
        double *nss;
        double *nrp;
        double *nsa;
        double *nka;
        double *nrsp;
        double *nrps;
        double *nhls;
        double *nrvs;
        double nptfo;
        double *nwins;
        double *nkills;
        double *nhs;
        double *nacc;
        double *ndgtgs;
        //Delta Values (New - Old)
        double dss;
        double drp;
        double dsa;
        double dka;
        double drsp;
        double drps;
        double dhls;
        double drvs;
        double dptfo;
        double dwins;
        double dkills;
        double dhs;
        double dacc;
        double ddgtgs;
        //Other variables
        double supporteff;
        double mediceff;
        double assisteff;
        double perfindex;
        //
        bool flag;
        double indexss;
        double indexrp;
        double indexts;
        double indexwns;
        double indexcts;
        double indexptfo;

        BF1Tracker(string _username) : Player(_username) {
            flag = true;
            ReadSavedData();
            nss = &squad_score;
            nrp = &rplayed;
            nsa = &sassists;
            nka = &kassists;
            nrsp = &addt_stats[2];
            nrps = &repairs;
            nhls = &heals;
            nrvs = &revives;
            nwins = &wins;
            nkills = &kills;
            nhs = &hshots;
            nacc = &accuracy;
            *nacc *= 100;
            //calculating ptfo factor
            nptfo = gmstats[0][0] + gmstats[1][0] + gmstats[2][0] + gmstats[3][0];
            indexss = 0;
            indexrp = 0;
            indexts = 0;
            indexwns = 0;
            indexcts = 0;
            indexptfo = 0;
        }

        void ReadSavedData() {
            string processed_username;
            processed_username = utilities::username_stripper(username);
            //
            auto r = cpr::Get(cpr::Url{
                                      "http://unkso.michaeljamesbondparsons.com/stats/bf1/latest?players[0]=" + processed_username},
                              cpr::Header{{"content-type",  "application/json"},
                                          {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
            if (r.status_code == 200) {
                if (r.text != "[]") {
                    json statpack;
                    statpack = json::parse(r.text);
                    cout << "This player's stats have been found in UNSKO Online Database" << '\n';
                    oss = statpack[processed_username]["general"]["squadscore"];
                    orp = statpack[processed_username]["general"]["roundsplayed"];
                    osa = statpack[processed_username]["general"]["suppressionassists"];
                    oka = statpack[processed_username]["general"]["killassists"];
                    orsp = statpack[processed_username]["general"]["resupplies"];
                    orps = statpack[processed_username]["general"]["repairs"];
                    ohls = statpack[processed_username]["general"]["heals"];
                    orvs = statpack[processed_username]["general"]["revives"];
                    optfo = statpack[processed_username]["general"]["ptfo"];
                    owins = statpack[processed_username]["general"]["wins"];
                    okills = statpack[processed_username]["general"]["kills"];
                    ohs = statpack[processed_username]["general"]["headshots"];
                    oacc = statpack[processed_username]["general"]["accuracy"];
                    cout << "===========================================================================" << '\n';
                } else {
                    cout << "This player has not been found in our Online Database" << '\n';
                    cout << "===========================================================================" << '\n';
                    flag = false;
                }
            } else {
                cout << "Something went wrong when trying to establish connection to WEB API. HTTP Status CODE: "
                     << r.status_code << '\n';
                flag = false;
            }
        }

        void savedata() {
            std::ofstream fout("TrackerData\\BF1\\" + username + ".txt");
            fout << "## LIST OF STATISTICS ##" << '\n';
            fout << std::fixed << std::setprecision(2);
            fout << *nss << '\n';
            fout << *nrp << '\n';
            fout << *nsa << '\n';
            fout << *nka << '\n';
            fout << *nrsp << '\n';
            fout << *nrps << '\n';
            fout << *nhls << '\n';
            fout << *nrvs << '\n';
            fout << nptfo << '\n';
            fout << *nwins << '\n';
            fout << *nkills << '\n';
            fout << *nhs << '\n';
            fout << *nacc << '\n';
            fout.close();
        }

        void Variation() {
            dss = *nss - oss;
            drp = *nrp - orp;
            dsa = *nsa - osa;
            dka = *nka - oka;
            drsp = *nrsp - orsp;
            drps = *nrps - orps;
            dhls = *nhls - ohls;
            drvs = *nrvs - orvs;
            dptfo = nptfo - optfo;
            dwins = *nwins - owins;
            dkills = *nkills - okills;
            dhs = *nhs - ohs;
            dacc = *nacc - oacc;
        }

        void PerfIndexBF1(bool outputflag = true) {
            if (flag == true) {
                cout << "Calculating Performance Index.../ " << '\n';
                Variation();
                supporteff = (drsp + drps) / 2;
                mediceff = (dhls + drvs) / 2;
                assisteff = (dsa + dka) / 2;
                indexss = ((dss) / (50 * log10(dss)));
                indexss = utilities::cap(indexss, 250);
                indexrp = (3 * pow(drp, 1.2));
                indexrp = utilities::cap(indexrp, 300);
                indexts = (supporteff + mediceff + assisteff);
                indexts = utilities::cap(indexts, 150);
                indexptfo = ((dptfo) / (400 * log10(dptfo)));
                indexptfo = utilities::cap(indexptfo, 200);
                indexcts = 0.25 * dkills / log10(dkills) + (0.15 * sqrt(dhs));
                indexcts = utilities::cap(indexcts, 100);
                perfindex = (indexss + indexrp + indexts + indexptfo + indexcts);
                if (outputflag == true) {
                    int op;
                    cout << "========================================================================" << '\n';
                    cout << "The Performance Index has been calculated. Choose an option to see the results." << '\n';
                    cout << "1 - Basic Report" << '\n';
                    cout << "2 - Detailed Report" << '\n';
                    cout << "3 - Full Report" << '\n';
                    cout << "========================================================================" << '\n';
                    cout << "Input: ";
                    cin >> op;
                    cin.get();
                    cout << "========================================================================" << '\n';
                    switch (op) {
                        case 1: {
                            cout << std::left;
                            cout << "Performance Index Basic Report" << '\n';
                            cout << "Player: " << username << '\n';
                            cout << "Perf.Index: " << perfindex << '\n';
                            cout << "End of Report" << '\n';
                            cout << "========================================================================" << '\n';
                            break;
                        }
                        case 2: {
                            cout << std::left;
                            cout << "Performance Index Detailed Report" << '\n';
                            cout << "Player: " << username << '\n';
                            cout << "Perf.Index: " << perfindex << '\n';
                            cout << "-----------------------------------" << '\n';
                            cout << "Individual Index Components" << '\n';
                            cout << "Score Index: " << indexss << '\n';
                            cout << "Team Stats Index: " << indexts << '\n';
                            cout << "Combat Stats Index: " << indexcts << '\n';
                            cout << "Activity Index (Rounds Played): " << indexrp << '\n';
                            cout << "PTFO Index: " << indexptfo << '\n';
                            cout << "End of Report" << '\n';
                            cout << "========================================================================" << '\n';
                            break;
                        }
                        case 3: {
                            cout << std::left;
                            cout << "Performance Index Full Report" << '\n';
                            cout << "Player: " << username << '\n';
                            cout << "Perf.Index: " << perfindex << '\n';
                            cout << "-----------------------------------" << '\n';
                            cout << "Individual Index Components" << '\n';
                            cout << "Score Index: " << indexss << '\n';
                            cout << "Team Stats Index: " << indexts << '\n';
                            cout << "Combat Stats Index: " << indexcts << '\n';
                            cout << "Activity Index (Rounds Played): " << indexrp << '\n';
                            cout << "PTFO Index: " << indexptfo << '\n';
                            cout << "-----------------------------------" << '\n';
                            cout << "Delta Factor Components" << '\n';
                            cout << "DScore: " << dss << '\n';
                            cout << "DRoundsPlayed: " << drp << '\n';
                            cout << "DSuppressionAssists: " << dsa << '\n';
                            cout << "DKillAssists: " << dka << '\n';
                            cout << "DResupplies: " << drsp << '\n';
                            cout << "DRevives: " << drvs << '\n';
                            cout << "DRepairs: " << drps << '\n';
                            cout << "DHeals: " << dhls << '\n';
                            cout << "DKills: " << dkills << '\n';
                            cout << "DHeadshots: " << dhs << '\n';
                            cout << "DAccuracy: " << dacc << '\n';
                            cout << "DDogtagsTaken: " << ddgtgs << '\n';
                            cout << "DPTFO: " << dptfo << '\n';
                            cout << "End of Report" << '\n';
                            cout << "========================================================================" << '\n';
                            break;
                        }
                    }
                }
            }
        }

        json exporter() {
            // time stamp
            time_t now = time(0);
            string time = ctime(&now);
            // json assembling
            json export_package;
            export_package["lastupdate"] = time;
            export_package["squad_score"] = *nss;
            export_package["rounds_played"] = *nrp;
            export_package["suppression_assists"] = *nsa;
            export_package["kill_assists"] = *nka;
            export_package["repairs"] = *nrps;
            export_package["resupplies"] = *nrsp;
            export_package["heals"] = *nhls;
            export_package["revives"] = *nrvs;
            export_package["ptfo"] = nptfo;
            export_package["wins"] = *nwins;
            export_package["kills"] = *nkills;
            export_package["headshots"] = *nhs;
            export_package["accuracy"] = *nacc;
            return export_package;
        }

        int GetRP() { return drp; }

        double GetPIndex() { return perfindex; }

        friend class Interface;
    };
}

namespace Siege {
    class SiegeGenStats {
    protected:
        std::vector<string> stats; //label
        std::vector<float> vstats; //number
        DataFetcher *fet;
    public:
        SiegeGenStats(string username) {
            try {
                cout << "------------------------------------------------------------------------" << '\n';
                fet = new DataFetcher("https://r6.tracker.network/profile/pc/" + username);
                cout << "Downloading General Stats for player: " << username << '\n';
                fet->SalvaDisco("siege");
                cout << "------------------------------------------------------------------------" << '\n';
                string aux;
                std::ifstream fin("SiegeStats.txt");
                while (!fin.eof()) {
                    getline(fin, aux);
                    stats.push_back(aux);
                }
                fin.close();
                aux.clear();
                DiskKernelParser();
                if (stats.size() != vstats.size()) {
                    cout << stats.size() << " | " << vstats.size() << '\n';
                    throw "Taking alternative path for parsing operation";
                }
                for (int i = 0; i < 10; i++) {
                    if (i < 5)
                        stats[i] = "S8" + stats[i];
                    else
                        stats[i] = "S9" + stats[i];
                }
            }
            catch (std::exception &e) {
                std::cerr << e.what() << '\n';
                cout << "------------------------------------------------------------------------" << '\n';
                stats.clear();
                vstats.clear();
                string aux;
                std::ifstream fin("SiegeStats2.txt");
                while (!fin.eof()) {
                    getline(fin, aux);
                    stats.push_back(aux);
                }
                aux.clear();
                DiskKernelParser();
                for (int i = 0; i < 5; i++) {
                    stats[i] = "S9" + stats[i];
                }
                assert(stats.size() == vstats.size());
            }
        }

        void DiskKernelParser() {
            string data;
            bool flag;
            double auxnumber;
            std::ifstream fin("siege.txt");
            while (!fin.eof()) {
                getline(fin, data);
                flag = statsfinder(data);
                if (flag == true) {
                    fin.ignore(5000, '\n');
                    fin.ignore(5000, '\n');
                    getline(fin, data);
                    data.erase(std::remove(data.begin(), data.end(), ','), data.end());
                    auxnumber = statreader(data);
                    if (auxnumber != -1) {
                        vstats.push_back(auxnumber);
                    }
                }
            }
        }

        double statreader(string _data) {
            double number;
            std::istringstream sstr(_data);
            if (!(sstr >> number)) {
                number = -1;
            }
            return number;
        }

        bool statsfinder(string data) {
            int pos;
            for (int i = 0; i < stats.size(); i++) {
                pos = data.find(stats[i]);
                if (pos != -1) { return true; }
            }
            return false;
        }

        void Mostra() {
            for (int i = 0; i < vstats.size(); i++) {
                cout << stats[i] + ": " << vstats[i] << '\n';
            }
        }

        std::vector<float> getvstats() { return vstats; }

        std::vector<string> getstats() { return stats; }

    };

    class SiegeOperators {
    protected:
        std::vector<string> opfinder;
        std::vector<int> values;
        std::vector<std::vector<int> > oper; //vector of operators
        DataFetcher *fetop;
        /*
		oper[0]=
		voper[0] = operatorId
		voper[1] = kills
		voper[2] = deaths
		voper[3] = K/D
		voper[4] = Wins
		voper[5] = Losses
		voper[6] = W/L Ratio
		voper[7] = Playtime
		voper[8] = special_ability#1
		voper[9] = special_ability#2 (if available)
		voper[10] = special_ability#3 (if available)
		*/
    public:
        ~SiegeOperators() { delete fetop; }

        SiegeOperators(string _username) {
            fetop = new DataFetcher("https://r6stats.com/stats/uplay/" + _username + "/operators");
            cout << "Downloading Operators Data for player: " << _username << '\n';
            fetop->SalvaDisco("siegeop");
            cout << "------------------------------------------------------------------------" << '\n';
            //Header of Variables
            int n1, n2, nb;
            int numb;
            std::vector<int> voper;
            std::map<int, int> operatorlines; //maps operatorId -> number of stats to read
            string aux;
            string aux2;

            std::ifstream fin("SiegeOperatives.txt");
            while (!fin.eof()) {
                fin >> n1;
                fin.ignore(5000, '\n');
                fin >> n2;
                fin.ignore(5000, '\n');
                operatorlines.insert(std::make_pair(n1, n2));
            }
            fin.close();
            fin.open("SiegeOPFinder2.txt");
            while (!fin.eof()) {
                getline(fin, aux2);
                opfinder.push_back(aux2);
            }
            fin.close();
            fin.open("siegeop.txt");
            while (!fin.eof()) {
                getline(fin, aux);
                nb = findop(aux);
                if (nb != -1) {
                    voper.push_back(nb); //find operator in html source code
                    for (int j = 0; j < operatorlines[nb]; j++) {
                        getline(fin, aux);
                        numb = secondaryparser(aux); //convert html line to stat number
                        if (numb != -1) {
                            voper.push_back(numb);
                        }
                    }
                    oper.push_back(voper);
                    voper.clear();
                }
            }
            fin.close();
        }

        double statreader(string _data) {
            double number;
            std::istringstream sstr(_data);
            if (!(sstr >> number)) {
                number = -1;
            }
            return number;
        }

        int findop(string data) {
            int pos;
            for (int k = 0; k < opfinder.size(); k++) {
                pos = data.find(opfinder[k]);
                if (pos != -1 && pos != 0) {
                    return k;
                }
            }
            return -1;
        }

        double secondaryparser(string data) {
            int pos;
            string sdata;
            double number;
            pos = data.find("</span>");
            if (pos != -1) {
                sdata = data.substr(pos + 7);
                number = statreader(sdata);
                if (number == -1) { number = 0; }
                return number;
            } else {
                return -1;
            }
        } // Parses the line with OP stat
        friend std::ostream &operator<<(std::ostream &os, SiegeOperators &obj) {
            os << "Displaying Operators Data. Total of Operators: " << obj.oper.size() << '\n';
            for (int i = 0; i < obj.oper.size(); i++) {
                os << "OPERATOR: " << '\n';
                for (int j = 0; j < obj.oper[i].size(); j++) {
                    //os << "J: " << j << '\n';
                    os << "STAT: " << obj.oper[i][j] << '\n';
                }
            }
            os << "End of Operators Data" << '\n';
            return os;
        }

        std::vector<std::vector<int> > GOper() { return oper; }
    };

    class Player {
    public:
        std::map<string, float> statistics;
        std::map<int, int> scoredict;
        SiegeGenStats *sp;
        SiegeOperators *sop;
        string username;
        DataFetcher *scorestats;
    public:
        ~Player() {
            delete sp;
            delete sop;
        };

        Player(string _username) : username(_username) {
            std::vector<float> values;
            std::vector<string> labels;
            sp = new SiegeGenStats(username);
            sop = new SiegeOperators(username);
            values = sp->getvstats();
            labels = sp->getstats();
            makemap(values, labels);
            additionalstats();
            std::ifstream fin("SiegeScoreTable.txt");
            int score;
            int level;
            while (!fin.eof()) {
                fin >> score;
                fin >> level;
                fin.ignore(5000, '\n');
                scoredict.insert(std::make_pair(level, score));
            }
            fin.close();
        }

        void additionalstats() {
            double aux;
            double aux2;
            double aux3;
            string rmp = "Ranked Matches Played";
            string mp = "Matches Played";
            string rwlr = "Ranked Win/Loss Ratio";
            string wlr = "Win/Loss Ratio";
            //Calculating Ranked Matches_Played
            aux = statistics["Wins"];
            aux2 = statistics["Losses"];
            aux3 = aux + aux2;
            statistics.insert(make_pair(rmp, aux3));
            //Calculating Ranked Win/Loss Ratio
            aux2 = statistics["Ranked Matches Played"];
            aux3 = aux / aux2;
            statistics.insert(make_pair(rwlr, aux3));
            //Calculating Matches_Played
            aux = statistics["Matches Won"];
            aux2 = statistics["Matches Lost"];
            aux3 = aux + aux2;
            statistics.insert(make_pair(mp, aux3));
            //Calculating Win/Loss Ratio
            aux3 = aux / aux2;
            statistics.insert(make_pair(wlr, aux3));
            //Fetching Score Stats
            string strdata;
            int scorepos;
            int levelpos;
            scorestats = new DataFetcher("https://r6stats.com/stats/uplay/" + username);
            if (scorestats->GStatusCode() == 200) {
                scorestats->SalvaDisco("scoresiege");
                std::ifstream fin("scoresiege.txt");
                while (!fin.eof()) {
                    getline(fin, strdata);
                    scorepos = strdata.find("Level Up");
                    levelpos = strdata.find(">Level<");
                    if (scorepos != -1) {
                        getline(fin, strdata);
                        strdata = strdata.substr(19, strdata.size());
                        strdata.erase(std::remove(strdata.begin(), strdata.end(), ','), strdata.end());
                        aux = utilities::statreader(strdata);
                        if (aux != -1) { statistics.insert(std::make_pair("LevelUpScore", aux)); }
                    }
                    if (levelpos != -1) {
                        getline(fin, strdata);
                        strdata = strdata.substr(19, strdata.size());
                        aux = utilities::statreader(strdata);
                        if (aux != -1) { statistics.insert(std::make_pair("Level", aux)); }
                    }
                }
            }
        }

        void makemap(std::vector<float> vstats, std::vector<string> stats) {
            for (int i = 0; i < stats.size(); i++) {
                statistics.insert(make_pair(stats[i], vstats[i]));
            }
        }

        friend std::ostream &operator<<(std::ostream &os, Player &plr) {
            os << plr.username << '\n';
            for (auto &element : plr.statistics) {
                os << element.first << ": " << element.second << '\n';
            }
            os << "End of Stats" << '\n';
            return os;
        }
    };

    class SiegeTracker : public Player {
    public:
        bool trackerflag;
        //  SAVED DATA				 API DATA				FILTERED DATA
        float oscore;
        float nscore;
        float dscore;
        float orounds_played;
        float nrounds_played;
        float drounds_played;
        float owins;
        float nwins;
        float dwins;
        float oreinforcements;
        float *nreinforcements;
        float dreinforcements;
        float obarricades;
        float *nbarricades;
        float dbarricades;
        float ogadgets_destroyed;
        float *ngadgets_destroyed;
        float dgadgets_destroyed;
        float orevives;
        float *nrevives;
        float drevives;
        float okillassists;
        float *nkillassists;
        float dkillassists;
        float okills;
        float *nkills;
        float dkills;
        float oheadshots;
        float *nheadshots;
        float dheadshots;
        float omelee_kills;
        float *nmelee_kills;
        float dmelee_kills;
        float odbno;
        float *ndbno;
        float ddbno;
        float openetration_kills;
        float *npenetration_kills;
        float dpenetration_kills;
        //PERFINDEX DATA
        double perfindex, indexscore, indexrp, indexts, indexwns, indexcts;

        SiegeTracker(string _username) : Player(_username) {
            trackerflag = true;
            ReadSavedData();
            nrounds_played =
                    statistics["S8Wins"] + statistics["S8Losses"] + statistics["S9Wins"] + statistics["S9Losses"] +
                    statistics["Matches Won"] + statistics["Matches Lost"];
            nwins = statistics["Matches Won"] + statistics["S8Wins"] + statistics["S9Wins"];
            nreinforcements = &statistics["Reinforcements Deployed"];
            nbarricades = &statistics["Barricades Deployed"];
            ngadgets_destroyed = &statistics["Gadgets Destroyed"];
            nrevives = &statistics["Revives"];
            nkillassists = &statistics["Kill Assists"];
            nkills = &statistics["Kills"];
            nheadshots = &statistics["Headshots"];
            nmelee_kills = &statistics["Melee Kills"];
            ndbno = &statistics["DBNOs"];
            npenetration_kills = &statistics["Penetration Kills"];
            nscore = scoredict[statistics["Level"]] + statistics["LevelUpScore"];
            indexscore = 0;
            indexrp = 0;
            indexts = 0;
            indexwns = 0;
            indexcts = 0;
        }

        void ReadSavedData() {
            string processed_username;
            processed_username = utilities::username_stripper(username);

            auto r = cpr::Get(cpr::Url{
                                      "http://unkso.michaeljamesbondparsons.com/stats/r6siege/latest?players[0]=" + processed_username},
                              cpr::Header{{"content-type",  "application/json"},
                                          {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
            if (r.status_code == 200) {
                if (r.text != "[]") {
                    json statpack;
                    statpack = json::parse(r.text);
                    cout << "This player's stats have been found in UNKSO Online Database" << '\n';
                    orounds_played = statpack[processed_username]["general"]["roundsplayed"];
                    owins = statpack[processed_username]["general"]["wins"];
                    oreinforcements = statpack[processed_username]["general"]["reinforcements"];
                    obarricades = statpack[processed_username]["general"]["barricades"];
                    ogadgets_destroyed = statpack[processed_username]["general"]["gadgets"];
                    orevives = statpack[processed_username]["general"]["revives"];
                    okillassists = statpack[processed_username]["general"]["killassists"];
                    okills = statpack[processed_username]["general"]["kills"];
                    oheadshots = statpack[processed_username]["general"]["headshots"];
                    omelee_kills = statpack[processed_username]["general"]["meleekills"];
                    odbno = statpack[processed_username]["general"]["dbno"];
                    openetration_kills = statpack[processed_username]["general"]["penetrationkills"];
                    oscore = statpack[processed_username]["general"]["score"];
                } else {
                    cout << "This player has not been found in our Online Database" << '\n';
                    trackerflag = false;
                }
            } else {
                cout << "Something went wrong when trying to establish connection to WEB API. HTTP Status CODE: "
                     << r.status_code << '\n';
                trackerflag = false;
            }
        }

        void savedata() {
            std::ofstream fout("TrackerData\\Siege\\" + username + ".txt");
            fout << "## LIST OF STATISTICS ##" << '\n';
            fout << std::fixed << std::setprecision(2);
            //fout << *nscore << '\n';
            fout << nrounds_played << '\n';
            fout << nwins << '\n';
            fout << *nreinforcements << '\n';
            fout << *nbarricades << '\n';
            fout << *ngadgets_destroyed << '\n';
            fout << *nrevives << '\n';
            fout << *nkillassists << '\n';
            fout << *nkills << '\n';
            fout << *nheadshots << '\n';
            fout << *nmelee_kills << '\n';
            fout << *ndbno << '\n';
            fout << *npenetration_kills << '\n';
            fout.close();
        } //DEPRECATED
        void Variation() {
            dscore = nscore - oscore;
            drounds_played = nrounds_played - orounds_played;
            dwins = nwins - owins;
            dreinforcements = *nreinforcements - oreinforcements;
            dbarricades = *nbarricades - obarricades;
            dgadgets_destroyed = *ngadgets_destroyed - ogadgets_destroyed;
            drevives = *nrevives - orevives;
            dkillassists = *nkillassists - okillassists;
            dkills = *nkills - okills;
            dheadshots = *nheadshots - oheadshots;
            dmelee_kills = *nmelee_kills - omelee_kills;
            ddbno = *ndbno - odbno;
            dpenetration_kills = *npenetration_kills - openetration_kills;
        }

        void PerfIndexSiege(bool outputflag = true) {
            if (trackerflag == true) {
                Variation();
                indexscore = ((dscore) / (200 * log10(dscore)));
                indexrp = (3 * pow(drounds_played, 1.2));
                indexts = (dreinforcements + dbarricades + dgadgets_destroyed + drevives + dkillassists) / 4;
                indexcts = 2 * sqrt(dkills + dheadshots + dmelee_kills + ddbno + dpenetration_kills);
                indexwns = 5 * dwins;
                indexscore = utilities::cap(indexscore, 300);
                indexrp = utilities::cap(indexrp, 250);
                indexts = utilities::cap(indexts, 150);
                indexcts = utilities::cap(indexcts, 150);
                indexwns = utilities::cap(indexwns, 150);
                perfindex = (indexscore + indexrp + indexts + indexwns + indexcts);
                if (outputflag == true) {
                    int op;
                    cout << "========================================================================" << '\n';
                    cout << "The Performance Index has been calculated. Choose an option to see the results." << '\n';
                    cout << "1 - Basic Report" << '\n';
                    cout << "2 - Detailed Report" << '\n';
                    cout << "3 - Full Report" << '\n';
                    cout << "========================================================================" << '\n';
                    cout << "Input: ";
                    cin >> op;
                    cin.get();
                    cout << "========================================================================" << '\n';
                    switch (op) {
                        case 1: {
                            cout << std::left;
                            cout << "Performance Index Basic Report" << '\n';
                            cout << "Player: " << username << '\n';
                            cout << "Perf.Index: " << perfindex << '\n';
                            cout << "End of Report" << '\n';
                            cout << "========================================================================" << '\n';
                            break;
                        }
                        case 2: {
                            cout << std::left;
                            cout << "Performance Index Detailed Report" << '\n';
                            cout << "Player: " << username << '\n';
                            cout << "Perf.Index: " << perfindex << '\n';
                            cout << "-----------------------------------" << '\n';
                            cout << "Individual Index Components" << '\n';
                            cout << "Score Index: " << indexscore << '\n';
                            cout << "Team Stats Index: " << indexts << '\n';
                            cout << "Combat Stats Index: " << indexcts << '\n';
                            cout << "Activity Index (Rounds Played): " << indexrp << '\n';
                            cout << "Wins Index: " << indexwns << '\n';
                            cout << "End of Report" << '\n';
                            cout << "========================================================================" << '\n';
                            break;
                        }
                        case 3: {
                            cout << std::left;
                            cout << "Performance Index Full Report" << '\n';
                            cout << "Player: " << username << '\n';
                            cout << "Perf.Index: " << perfindex << '\n';
                            cout << "-----------------------------------" << '\n';
                            cout << "Individual Index Components" << '\n';
                            cout << "Score Index: " << indexscore << '\n';
                            cout << "Team Stats Index: " << indexts << '\n';
                            cout << "Combat Stats Index: " << indexcts << '\n';
                            cout << "Activity Index (Rounds Played): " << indexrp << '\n';
                            cout << "Wins Index: " << indexwns << '\n';
                            cout << "-----------------------------------" << '\n';
                            cout << "Delta Factor Components" << '\n';
                            cout << "DScore: " << dscore << '\n';
                            cout << "DRoundsPlayed: " << drounds_played << '\n';
                            cout << "DGadgetsDestroyed: " << dgadgets_destroyed << '\n';
                            cout << "DReinforcements: " << dreinforcements << '\n';
                            cout << "DBarricades: " << dbarricades << '\n';
                            cout << "DRevives: " << drevives << '\n';
                            cout << "DKillAssists: " << dkillassists << '\n';
                            cout << "DKills: " << dkills << '\n';
                            cout << "DHeadshots: " << dheadshots << '\n';
                            cout << "DMelee: " << dmelee_kills << '\n';
                            cout << "DDBNO (Down but not out): " << ddbno << '\n';
                            cout << "DPenetrationKills: " << dpenetration_kills << '\n';
                            cout << "End of Report: " << '\n';
                            cout << "========================================================================" << '\n';
                            break;
                        }
                    }
                }
            }
        }

        json exporter() {
            // json assembling
            json export_package;
            export_package["score"] = nscore;
            export_package["rounds_played"] = nrounds_played;
            export_package["reinforcements"] = *nreinforcements;
            export_package["barricades"] = *nbarricades;
            export_package["kill_assists"] = *nkillassists;
            export_package["gadgets"] = *ngadgets_destroyed;
            export_package["melee_kills"] = *nmelee_kills;
            export_package["penetration_kills"] = *npenetration_kills;
            export_package["revives"] = *nrevives;
            export_package["dbno"] = *ndbno;
            export_package["wins"] = nwins;
            export_package["kills"] = *nkills;
            export_package["headshots"] = *nheadshots;
            return export_package;
        }

        int GetRP() { return drounds_played; }

        double GetPIndex() { return perfindex; }

        friend class Interface;
    };
}

namespace Raptor {
    class User {
    public:
        string username;
        int roundsplayed;
        int timeplayed;
        int revives;
        int repairs;
        int heals;
        int resupplies;
        int squadscore;
        int orders_comp;
        int flagcaptured;
        int flagdefended;
        int bombsplaced;
        int bombsdefused;
        float gamemodescore;
        int geographic_region;

        /*===================================================================================================*/
        User() : username(""), repairs(0), orders_comp(0), bombsdefused(0), roundsplayed(0), heals(0), flagcaptured(0),
                 gamemodescore(0), timeplayed(0),
                 flagdefended(0), geographic_region(0), revives(0), squadscore(0), bombsplaced(0) {}

        User(string a, int b, int c, int d, int e, int f, int g, float h, int i, int j, int k, int l, int m, int n)
                : username(a), repairs(b), orders_comp(c), bombsdefused(d), roundsplayed(e), heals(f), flagcaptured(g),
                  gamemodescore(h), timeplayed(i),
                  flagdefended(j), geographic_region(k), revives(l), squadscore(m), bombsplaced(n) {}
    };

    class Vision {
    protected:
        DataFetcher *logdata;
        std::vector<string> t_sourcecode; //transformed source code
        int maxrank;
        int minrank;
        int rank;
        int numb;
        int numbplrs;
        bool search_for_more;
        std::map<string, int> list;
        std::vector<string> users;
        std::vector<string> dbusernames;
    public:
        ~Vision() { delete logdata; }

        Vision() : search_for_more(true), numbplrs(0) {
            int i, point, maxpoint;
            cout
                    << "========================================================================================================================"
                    << '\n';
            cout << "UNKSO RAPTOR" << '\n';
            cout
                    << "========================================================================================================================"
                    << '\n';
            cout << "Instructions of Use: " << '\n';
            cout
                    << "You will filter players by their rank. Initial studies have shown that the rank margin of 100 - 120 will contain the best recruits for our clan.";
            cout
                    << "The number of interactions is the number of times that the Raptor is going to go over a pack of 100 players in the database while making sure they meet certain criteria.";
            cout
                    << "The starting point is the position in the database where you think those players in the rank margin are located. A good example would be values between 300-500.";
            cout
                    << "If this phase takes long, it is because the database is slow. Still, this can take a few to several minutes depending on your search parameters.\n";
            cout
                    << "========================================================================================================================"
                    << '\n';
            cout << "What is the maximum rank for the search?" << '\n';
            cin >> maxrank;
            cin.get();
            cout << "What is the minimum rank for the search?" << '\n';
            cin >> minrank;
            cin.get();
            cout << "What is the maximum number of interactions for the search?" << '\n';
            cin >> numb;
            cin.get();
            cout << "Do you have a starting point (i.e. 50) for where you wish to start the search?" << '\n';
            cin >> point;
            cin.get();
            cout
                    << "========================================================================================================================"
                    << '\n';
            i = 0;
            string url;
            string logurl = "http://unkso.michaeljamesbondparsons.com/stats/bf1/latest";
            auto r = cpr::Get(cpr::Url{logurl}, cpr::Header{{"content-type",  "application/json"},
                                                            {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
            if (r.text != "{\"players\":[]}") {
                json import_package = json::parse(r.text);
                import_package = import_package["players"];
                for (int db_plr = 0; db_plr < import_package.size(); db_plr++) {
                    try {
                        dbusernames.push_back(import_package[db_plr].at("player"));
                    }
                    catch (json::out_of_range &e) {
                        break;
                    }
                    catch (std::exception &x) {}
                }
                cout << "The Database has been downloaded with " << dbusernames.size() << " players in it." << '\n';
                cout
                        << "========================================================================================================================================================================="
                        << '\n';
            } // finds db players
            while (search_for_more) {
                url = "https://battlefieldtracker.com/bf1/leaderboards/pc/Rank?page=" + std::to_string(point);
                logdata = new DataFetcher(url);
                t_sourcecode = SourceCode2StringVector(logdata->GetSource());
                t_sourcecode_processor();
                t_sourcecode.clear();
                cout << list.size() << " players have been found and logged!" << '\n';
                cout
                        << "======================================================================================================================================================================="
                        << '\n';
                i++;
                point++;
                if (i >= numb) { break; }
            }
        }

        std::vector<string> getusers() {
            std::vector<string> exp_users;
            for (auto const &element : list) {
                exp_users.push_back(element.first);
            }
            return exp_users;
        }

        int loggerstatfinder(string dataline) {
            string line_to_find1 = "<div class=\"player\">";
            string line_to_find2 = "<div class=\"pull-right\">";
            int pos;
            pos = dataline.find(line_to_find1);
            if (pos != -1) { return 1; }
            pos = dataline.find(line_to_find2);
            if (pos != -1) { return 2; }
            return 0;
        }

        std::vector<string> SourceCode2StringVector(string sourcecode) {
            std::vector<string> tx_sourcecode;
            std::string line;
            std::istringstream stream(sourcecode);
            while (std::getline(stream, line)) {
                tx_sourcecode.push_back(line);
            }
            return tx_sourcecode;
        }

        void t_sourcecode_processor() {
            int flag;
            int pos_x;
            int pos_xdx;
            string single_user;
            std::vector<string>::iterator unq_plr;
            for (int i = 0; i < t_sourcecode.size(); i++) {
                flag = loggerstatfinder(t_sourcecode[i]);
                if (flag == 1) {
                    pos_x = t_sourcecode[i + 1].find('>');
                    pos_xdx = t_sourcecode[i + 1].rfind('<');
                    if (pos_x != -1 && pos_xdx != -1) {
                        single_user = t_sourcecode[i + 1].substr(pos_x + 1, pos_xdx - pos_x - 1);
                        unq_plr = std::find(dbusernames.begin(), dbusernames.end(), single_user);
                        if ((unq_plr - dbusernames.begin()) < (dbusernames.end() - dbusernames.begin())) {
                            cout << "A non-unique player has been found : [ID]= " << unq_plr - dbusernames.begin()
                                 << '\n';
                            single_user.clear();
                        }
                    }
                } else if (flag == 2) {
                    rank = utilities::statreader(t_sourcecode[i + 1]);
                    if (rank < minrank) {
                        search_for_more = false;
                        break;
                    } else if (rank <= maxrank && rank >= minrank) {
                        if (single_user.size() > 0) {
                            list.insert(std::make_pair(single_user, rank));
                            numbplrs++;
                        }
                    }
                }
            }
        }

        friend std::ostream &operator<<(std::ostream &os, Vision &obj) {
            os << "Showing all players and their ranks: " << '\n';
            for (auto &element : obj.list) {
                os << element.first << ": " << element.second << '\n';
            }
            os << "Total of players: " << obj.list.size() << '\n';
            return os;
        }
    };

    class Engine {
    protected:
        Vision *eye;
        string db_partition;
        //Statistical Matrix
        std::vector<string> usernames;
        std::vector<string> rusernames;

        std::vector<int> roundsplayed;
        std::vector<int> rroundsplayed;
        std::vector<int> droundsplayed;
        std::vector<int> timeplayed;
        std::vector<int> rtimeplayed;
        std::vector<int> dtimeplayed;

        std::vector<int> revives;
        std::vector<int> rrevives;
        std::vector<int> drevives;
        std::vector<int> repairs;
        std::vector<int> rrepairs;
        std::vector<int> drepairs;
        std::vector<int> heals;
        std::vector<int> rheals;
        std::vector<int> dheals;
        std::vector<int> resupplies;
        std::vector<int> rresupplies;
        std::vector<int> dresupplies;
        std::vector<int> squadscore;
        std::vector<int> rsquadscore;
        std::vector<int> dsquadscore;
        std::vector<int> orders_comp;
        std::vector<int> rorders_comp;
        std::vector<int> dorders_comp;

        std::vector<int> flagcaptured;
        std::vector<int> rflagcaptured;
        std::vector<int> dflagcaptured;
        std::vector<int> flagdefended;
        std::vector<int> rflagdefended;
        std::vector<int> dflagdefended;
        std::vector<int> bombsplaced;
        std::vector<int> rbombsplaced;
        std::vector<int> dbombsplaced;
        std::vector<int> bombsdefused;
        std::vector<int> rbombsdefused;
        std::vector<int> dbombsdefused;
        std::vector<float> gamemodescore;
        std::vector<float> rgamemodescore;
        std::vector<float> dgamemodescore;
        std::vector<int> geographic_region;
        std::vector<int> rgeographic_region;
        //End of Statistical Matrix
        //Auxiliary Statistical Fetching
        std::vector<string> finder = {
                "<div class=\"value\" data-stat=\"BombsPlaced\">",
                "<div class=\"value\" data-stat=\"BombsDefused\">",
                "<div class=\"value\" data-stat=\"Resupplies\">",
                "<div class=\"value\" data-stat=\"OrdersCompleted\">",
        };
        DataFetcher *dftx;
        std::vector<string> middlemanstats;
        std::vector<int> garbagecollector;
        //End of Auxiliary Statistical Fetching
        //Calculated Indexes
        std::vector<float> vteamwork_index;
        std::vector<float> vptfo_index;
        std::vector<float> vactivity_index;
    public:
        ~Engine() { delete eye; }

        Engine(bool update) : dftx(nullptr) {
            bool status = true;
            if (update) {
                cout << "Synchronizing and uploading database players. This will take several minutes." << '\n';
                cout
                        << "========================================================================================================================"
                        << '\n';
                status = server_imodule();
                if (status) {
                    network_module(rusernames);
                    parser_module(usernames);
                    region_filter(usernames);
                    matrixequalizer();
                    Debug("");
                    Variation();
                } else {
                    cout << "The database is empty. The program will be terminated." << '\n';
                    exit(0);
                }
            } else {
                eye = new Raptor::Vision();
                network_module(eye->getusers());
                parser_module(usernames);
                region_filter(usernames);
                Debug("1");
                matrixequalizer();
                Debug("2");
                cout << "Do you wish to sync data with the UNKSO Database?" << '\n';
                string op;
                getline(cin, op);
                if (op == "y" || op == "Y") { server_emodule(); }
            }
        }

        //Engine
        void network_module(std::vector<string> n_users) {
            std::vector<cpr::Url> urls;
            string auxurl;
            auto container = std::vector<std::future<cpr::Response>>{};

            for (int i = 0; i < n_users.size(); i++) {
                auxurl = "https://battlefieldtracker.com/bf1/api/Stats/DetailedStats?platform=3&displayName=" +
                         n_users[i];
                urls.push_back(auxurl);
            }
            for (int i = 0; i < n_users.size(); ++i) {
                try {
                    container.emplace_back(cpr::GetAsync(urls[i], cpr::VerifySsl{false},
                                                         cpr::Header{{"TRN-Api-Key", "152de4e7-22a4-4a0a-b2b6-2bea2c7886b6"}}));
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    if (i % 50 == 0) { std::this_thread::sleep_for(std::chrono::seconds(3)); }
                    cout << i << " players' stats have been downloaded" << '\n';
                }
                catch (std::exception &e) { std::cerr << e.what() << '\n'; }
            }
            std::this_thread::sleep_for(std::chrono::seconds(5));
            cout << "All requests have been downloaded" << '\n';

            int i = 0;
            json pack;
            int bad_requests = 0;
            std::vector<int> gbcollec;
            for (auto &fr : container) {
                cout << i - bad_requests << " players have successfully been processed." << '\n';
                auto r = fr.get();
                if (r.status_code == 200) {
                    pack = json::parse(r.text);
                    roundsplayed.push_back(pack["result"]["roundsPlayed"]);
                    timeplayed.push_back(pack["result"]["basicStats"]["timePlayed"]);
                    repairs.push_back(pack["result"]["repairs"]);
                    heals.push_back(pack["result"]["heals"]);
                    revives.push_back(pack["result"]["revives"]);
                    squadscore.push_back(pack["result"]["squadScore"]);
                    flagcaptured.push_back(pack["result"]["flagsCaptured"]);
                    flagdefended.push_back(pack["result"]["flagsDefended"]);
                    usernames.push_back(pack["profile"]["displayName"]);
                    double singlegmodscore = 0.0;
                    double numb;
                    for (int j = 0; j < 7; j++) {
                        if (pack["result"]["gameModeStats"][j]["name"] == "Rush"
                            || pack["result"]["gameModeStats"][j]["name"] == "TeamDeathMatch"
                            || pack["result"]["gameModeStats"][j]["name"] == "Conquest"
                            || pack["result"]["gameModeStats"][j]["name"] == "Domination"
                            || pack["result"]["gameModeStats"][j]["name"] == "TugOfWar"
                            || pack["result"]["gameModeStats"][j]["name"] == "Possession"
                            || pack["result"]["gameModeStats"][j]["name"] == "Breakthrough") {
                            numb = pack["result"]["gameModeStats"][j]["score"];
                            singlegmodscore += numb;
                        }
                    }
                    gamemodescore.push_back(singlegmodscore);
                } else if (r.status_code != 200) {
                    roundsplayed.push_back(-1);
                    timeplayed.push_back(-1);
                    repairs.push_back(-1);
                    heals.push_back(-1);
                    revives.push_back(-1);
                    squadscore.push_back(-1);
                    flagcaptured.push_back(-1);
                    flagdefended.push_back(-1);
                    usernames.push_back("");
                    gamemodescore.push_back(-1);
                    bad_requests++; //gbcollec.push_back(i);
                }
                i++;
            }
            cout << "Number of bad requests: " << bad_requests << '\n';
            /*if (gbcollec.size() != 0) {
			int numb;
			for (int z = 0; z < gbcollec.size(); z++) {
			if (rusernames.size() != 0) {
			numb = gbcollec[z] - z;
			rusernames.erase(rusernames.begin() + numb);
			rtimeplayed.erase(rtimeplayed.begin() + numb);
			rrevives.erase(rrevives.begin() + numb);
			rrepairs.erase(rrepairs.begin() + numb);
			rheals.erase(rheals.begin() + numb);
			rresupplies.erase(rresupplies.begin() + numb);
			rsquadscore.erase(rsquadscore.begin() + numb);
			rroundsplayed.erase(rroundsplayed.begin() + numb);
			rorders_comp.erase(rorders_comp.begin() + numb);
			rflagcaptured.erase(rflagcaptured.begin() + numb);
			rflagdefended.erase(rflagdefended.begin() + numb);
			rbombsplaced.erase(rbombsplaced.begin() + numb);
			rbombsdefused.erase(rbombsdefused.begin() + numb);
			rgamemodescore.erase(rgamemodescore.begin() + numb);
			rgeographic_region.erase(rgeographic_region.begin() + numb);
			}
			}
			}*/
        }

        void parser_module(std::vector<string> n_users) {
            int j = 0;
            std::vector<cpr::Url> urls;
            string auxurl;
            json pm_pack;
            for (int i = 0; i < n_users.size(); i++) {
                auxurl = "https://battlefieldtracker.com/bf1/profile/pc/" + n_users[i];
                urls.push_back(auxurl);
            }
            cout << "Downloading Parsing Data" << '\n';
            auto container = std::vector<std::future<cpr::Response>>{};
            for (int i = 0; i < n_users.size(); ++i) {
                try {
                    container.emplace_back(cpr::GetAsync(urls[i], cpr::VerifySsl{false}));
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    cout << i << " players' additional stats have been downloaded" << '\n';
                }
                catch (std::exception &e) { std::cerr << e.what() << '\n'; }
            }
            cout << "Wait a few more seconds while the API refreshes and all requests are processed." << '\n';
            std::this_thread::sleep_for(std::chrono::seconds(5));
            cout << "All requests have been downloaded" << '\n';
            for (auto &fr : container) {
                cout << j << " players have been parsed." << '\n';
                auto r = fr.get();
                if (r.status_code == 200) {
                    try {
                        vector<int> addt_st = addt_stats(r.text);
                        if (addt_st.size() != 0) {
                            bombsplaced.push_back(addt_st[0]);
                            bombsdefused.push_back(addt_st[1]);
                            resupplies.push_back(addt_st[2]);
                            orders_comp.push_back(addt_st[3]);
                        } else {
                            cout << "Unable to parse this player" << '\n';
                            bombsplaced.push_back(-1);
                            bombsdefused.push_back(-1);
                            resupplies.push_back(-1);
                            orders_comp.push_back(-1);
                            std::ofstream fout("parseproblem" + std::to_string(j) + ".txt");
                            fout << r.text << '\n';
                            fout.close();
                        }
                        j++;
                    }
                    catch (std::exception &e) {
                        std::cerr << e.what() << '\n';
                    }
                } else {
                    cout << "Something bad has happened!" << '\n';
                    bombsplaced.push_back(-1);
                    bombsdefused.push_back(-1);
                    resupplies.push_back(-1);
                    orders_comp.push_back(-1);
                }
            }
        }

        void region_filter(std::vector<string> n_users) {
            std::vector<cpr::Url> urls;
            string auxurl;
            json pm_pack;
            for (int i = 0; i < n_users.size(); i++) {
                auxurl = "https://battlefieldtracker.com/bf1/profile/pc/" + usernames[i] + "/matches";
                urls.push_back(auxurl);
            }
            auto container = std::vector<std::future<cpr::Response>>{};
            cout << "Gathering geographic data pattern. This may take a while." << '\n';
            for (int i = 0; i < n_users.size(); ++i) {
                try {
                    container.emplace_back(cpr::GetAsync(urls[i], cpr::VerifySsl{false}));
                    cout << i << " players' additional stats have been downloaded" << '\n';
                    if (i % 50 == 0) { std::this_thread::sleep_for(std::chrono::milliseconds(2000)); }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                catch (std::exception &e) { std::cerr << e.what() << '\n'; }
            }
            cout << "Wait a few more seconds while the API refreshes and all requests are finished." << '\n';
            std::this_thread::sleep_for(std::chrono::seconds(5));
            cout << "Geographic data pattern downloaded for all players." << '\n';
            int j = 0;
            for (auto &fr : container) {
                cout << j << " players have been processed." << '\n';
                fr.wait();
                auto r = fr.get();
                if (r.status_code == 200) {
                    try {
                        geographic_region.push_back(region_filter(r.text));
                    }
                    catch (std::exception &e) {
                        std::cerr << e.what() << '\n';
                    }
                    j++;
                } else {
                    cout << "Assigning Unknown Region - Broken Data" << '\n';
                    geographic_region.push_back(-1);
                }
            }
            if (geographic_region.size() != n_users.size()) {
                cout << "Fatal Error has happened when fetching geo data" << '\n';
                cout << geographic_region.size() << " : " << n_users.size() << '\n';
            }
        }

        virtual void server_emodule() {
            int n_packs = usernames.size() / 500;
            int remainder = usernames.size() - (500 * n_packs);

            for (int pack = 0; pack < n_packs; pack++) {
                json data = exporter(500, 500 * pack);
                cout << "Sending Raptor Data Pack#" << pack << " to UNKSO Server" << '\n';
                auto r = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                                   cpr::Body{data.dump()},
                                   cpr::Header{{"content-type",  "application/json"},
                                               {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
                if (r.status_code == 200) { cout << "Data has been exported successfully!" << '\n'; }
                else {
                    cout << "Something went wrong when exporting the data. Status Code: " << r.status_code << '\n';
                    while (true) {
                        cout << "Attempt to upload again? Type either Y for Yes or N for No." << '\n';
                        string ans;
                        getline(cin, ans);
                        if (ans == "Y" || ans == "y") {
                            auto rx = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                                                cpr::Body{data.dump()},
                                                cpr::Header{{"content-type",  "application/json"},
                                                            {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
                            if (rx.status_code == 200) {
                                cout << "Data has been exported successfully!" << '\n';
                                break;
                            } else
                                cout << "Something went wrong when exporting the data. Status Code: " << r.status_code
                                     << '\n';
                        } else if (ans == "N" || ans == "n") {
                            cout
                                    << "You won't be uploading again. Please report this issue to UNKSO Engineering Department."
                                    << '\n';
                            break;
                        }
                    }
                }
            }
            json data = exporter(remainder, 500 * n_packs);
            cout << "Sending Raptor Data Remainder Pack to UNKSO Server" << '\n';
            auto r = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                               cpr::Body{data.dump()},
                               cpr::Header{{"content-type",  "application/json"},
                                           {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
            if (r.status_code == 200) { cout << "Data has been exported successfully!" << '\n'; }
            else {
                while (true) {
                    cout << "Attempt to upload again? Type either Y for Yes or N for No." << '\n';
                    string ans;
                    getline(cin, ans);
                    if (ans == "Y" || ans == "y") {
                        auto rx = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                                            cpr::Body{data.dump()},
                                            cpr::Header{{"content-type",  "application/json"},
                                                        {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
                        if (rx.status_code == 200) {
                            cout << "Data has been exported successfully!" << '\n';
                            break;
                        } else
                            cout << "Something went wrong when exporting the data. Status Code: " << r.status_code
                                 << '\n';
                    } else if (ans == "N" || ans == "n") {
                        cout
                                << "You won't be uploading again. Please report this issue to UNKSO Engineering Department."
                                << '\n';
                        break;
                    }
                }
            }
        }

        bool server_imodule() {
            cout << "Type in the database directory you're tracking your stats from: ";
            getline(cin, db_partition);
            cout << '\n';
            string url = "http://unkso.michaeljamesbondparsons.com/stats/bf1/latest?events[0]=" + db_partition;
            auto r = cpr::Get(cpr::Url{url}, cpr::Header{{"content-type",  "application/json"},
                                                         {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
            if (r.text != "{\"players\":[]}") {
                json import_package = json::parse(r.text);
                json subpack = import_package["players"];
                int i = -1;
                json aux_pack;
                bool flag = true;
                while (flag) {
                    try {
                        i++;
                        aux_pack = subpack[i]["general"];
                        rusernames.push_back(subpack[i].at("player"));
                        rroundsplayed.push_back(aux_pack.at("rounds_played"));
                        rtimeplayed.push_back(aux_pack.at("time_played"));
                        rrevives.push_back(aux_pack.at("revives"));
                        rrepairs.push_back(aux_pack.at("repairs"));
                        rheals.push_back(aux_pack.at("heals"));
                        rresupplies.push_back(aux_pack.at("resupplies"));
                        rsquadscore.push_back(aux_pack.at("squad_score"));
                        rorders_comp.push_back(aux_pack.at("orders_completed"));
                        rflagcaptured.push_back(aux_pack.at("flags_captured"));
                        rflagdefended.push_back(aux_pack.at("flags_defended"));
                        rbombsplaced.push_back(aux_pack.at("bombs_placed"));
                        rbombsdefused.push_back(aux_pack.at("bombs_defused"));
                        rgamemodescore.push_back(aux_pack.at("ptfo"));
                        rgeographic_region.push_back(aux_pack.at("region"));
                    }
                    catch (json::out_of_range &e) {
                        flag = false;
                    }
                    catch (std::exception &x) {}
                }
                cout << "There were " << rusernames.size() << " valid players in the database." << '\n';
                return true;
            } else { return false; }
        }

        void matrixequalizer() {
            cout << "This is the current Statistical Matrix" << '\n';
            cout << usernames.size() << " | " << rusernames.size() << '\n' <<
                 timeplayed.size() << " | " << rtimeplayed.size() << '\n' <<
                 revives.size() << " | " << rrevives.size() << '\n' <<
                 repairs.size() << " | " << rrepairs.size() << '\n' <<
                 heals.size() << " | " << rheals.size() << '\n' <<
                 resupplies.size() << " | " << rresupplies.size() << '\n' <<
                 squadscore.size() << " | " << rsquadscore.size() << '\n' <<
                 orders_comp.size() << " | " << rorders_comp.size() << '\n' <<
                 flagcaptured.size() << " | " << rflagcaptured.size() << '\n' <<
                 flagdefended.size() << " | " << rflagdefended.size() << '\n' <<
                 bombsplaced.size() << " | " << rbombsplaced.size() << '\n' <<
                 bombsdefused.size() << " | " << rbombsdefused.size() << '\n' <<
                 gamemodescore.size() << " | " << rgamemodescore.size() << '\n' <<
                 geographic_region.size() << " | " << rgeographic_region.size() << '\n';
            system("Pause");

            for (int i = 0; i < usernames.size(); i++) {
                if (usernames[i] == "" || timeplayed[i] == -1 || revives[i] == -1 || repairs[i] == -1 ||
                    heals[i] == -1 || resupplies[i] == -1 || squadscore[i] == -1 ||
                    orders_comp[i] == -1 || flagcaptured[i] == -1 || flagdefended[i] == -1 || bombsplaced[i] == -1 ||
                    bombsdefused[i] == -1 || gamemodescore[i] == -1 || geographic_region[i] == -1) {
                    usernames[i] = "";
                    timeplayed[i] = -1;
                    revives[i] = -1;
                    repairs[i] = -1;
                    heals[i] = -1;
                    resupplies[i] = -1;
                    squadscore[i] = -1;
                    roundsplayed[i] = -1;
                    orders_comp[i] = -1;
                    flagcaptured[i] = -1;
                    flagdefended[i] = -1;
                    bombsplaced[i] = -1;
                    bombsdefused[i] = -1;
                    gamemodescore[i] = -1;
                    if (rusernames.size() != 0) {
                        rusernames[i] = "";
                        rtimeplayed[i] = -1;
                        rrevives[i] = -1;
                        rrepairs[i] = -1;
                        rheals[i] = -1;
                        rresupplies[i] = -1;
                        rsquadscore[i] = -1;
                        rroundsplayed[i] = -1;
                        rorders_comp[i] = -1;
                        rflagcaptured[i] = -1;
                        rflagdefended[i] = -1;
                        rbombsplaced[i] = -1;
                        rbombsdefused[i] = -1;
                        rgamemodescore[i] = -1;
                    }
                }
            }
            /*if (usernames.size() == resupplies.size()) {
			for (int p = 0; p < resupplies.size(); p++) {
			if (resupplies[p] == -1) {
			garbagecollector.push_back(p);
			}
			}
			}
			else {
			cout << "Something bad has happened in MatrixEqualizer()" << '\n';
			}
			cout << "Number of bad records: " << garbagecollector.size() << '\n';*/
            /*if (garbagecollector.size() != 0) {
			int numb;
			for (int z = 0; z < garbagecollector.size(); z++) {
			numb = garbagecollector[z] - z;
			usernames.erase(usernames.begin() + numb);
			timeplayed.erase(timeplayed.begin() + numb);
			revives.erase(revives.begin() + numb);
			repairs.erase(repairs.begin() + numb);
			heals.erase(heals.begin() + numb);
			resupplies.erase(resupplies.begin() + numb);
			squadscore.erase(squadscore.begin() + numb);
			roundsplayed.erase(roundsplayed.begin() + numb);
			orders_comp.erase(orders_comp.begin() + numb);
			flagcaptured.erase(flagcaptured.begin() + numb);
			flagdefended.erase(flagdefended.begin() + numb);
			bombsplaced.erase(bombsplaced.begin() + numb);
			bombsdefused.erase(bombsdefused.begin() + numb);
			gamemodescore.erase(gamemodescore.begin() + numb);
			geographic_region.erase(geographic_region.begin() + numb);
			if (rusernames.size() != 0) {
			rusernames.erase(rusernames.begin() + numb);
			rtimeplayed.erase(rtimeplayed.begin() + numb);
			rrevives.erase(rrevives.begin() + numb);
			rrepairs.erase(rrepairs.begin() + numb);
			rheals.erase(rheals.begin() + numb);
			rresupplies.erase(rresupplies.begin() + numb);
			rsquadscore.erase(rsquadscore.begin() + numb);
			rroundsplayed.erase(rroundsplayed.begin() + numb);
			rorders_comp.erase(rorders_comp.begin() + numb);
			rflagcaptured.erase(rflagcaptured.begin() + numb);
			rflagdefended.erase(rflagdefended.begin() + numb);
			rbombsplaced.erase(rbombsplaced.begin() + numb);
			rbombsdefused.erase(rbombsdefused.begin() + numb);
			rgamemodescore.erase(rgamemodescore.begin() + numb);
			rgeographic_region.erase(rgeographic_region.begin() + numb);
			}
			}
			}*/
            cout << "Player Matrix has been normalized" << '\n';
            cout << usernames.size() << " | " << rusernames.size() << '\n' <<
                 timeplayed.size() << " | " << rtimeplayed.size() << '\n' <<
                 revives.size() << " | " << rrevives.size() << '\n' <<
                 repairs.size() << " | " << rrepairs.size() << '\n' <<
                 heals.size() << " | " << rheals.size() << '\n' <<
                 resupplies.size() << " | " << rresupplies.size() << '\n' <<
                 squadscore.size() << " | " << rsquadscore.size() << '\n' <<
                 orders_comp.size() << " | " << rorders_comp.size() << '\n' <<
                 flagcaptured.size() << " | " << rflagcaptured.size() << '\n' <<
                 flagdefended.size() << " | " << rflagdefended.size() << '\n' <<
                 bombsplaced.size() << " | " << rbombsplaced.size() << '\n' <<
                 bombsdefused.size() << " | " << rbombsdefused.size() << '\n' <<
                 gamemodescore.size() << " | " << rgamemodescore.size() << '\n' <<
                 geographic_region.size() << " | " << rgeographic_region.size() << '\n';
        }

        void Variation() {
            try {
                for (int t = 0; t < usernames.size(); t++) {
                    if (usernames[t] != rusernames[t]) { cout << "BAD MATRIX t= " << t << '\n'; }
                    dtimeplayed.push_back(timeplayed[t] - rtimeplayed[t]);
                    droundsplayed.push_back(roundsplayed[t] - rroundsplayed[t]);
                    drevives.push_back(revives[t] - rrevives[t]);
                    drepairs.push_back(repairs[t] - rrepairs[t]);
                    dheals.push_back(heals[t] - rheals[t]);
                    dresupplies.push_back(resupplies[t] - rresupplies[t]);
                    dsquadscore.push_back(squadscore[t] - rsquadscore[t]);
                    dorders_comp.push_back(orders_comp[t] - rorders_comp[t]);
                    dflagcaptured.push_back(flagcaptured[t] - rflagcaptured[t]);
                    dflagdefended.push_back(flagdefended[t] - rflagdefended[t]);
                    dbombsplaced.push_back(bombsplaced[t] - rbombsplaced[t]);
                    dbombsdefused.push_back(bombsdefused[t] - rbombsdefused[t]);
                    dgamemodescore.push_back(gamemodescore[t] - rgamemodescore[t]);
                }
            }
            catch (std::exception &e) {
                std::cerr << e.what() << '\n';
                std::cout << "Variation() Exception" << '\n';
            }
            if (rusernames != usernames) {
                std::ofstream fout("VariationLog.txt");
                for (int j = 0; j < usernames.size(); j++) {
                    fout << rusernames[j] << " | " << usernames[j] << '\n';
                }
                fout.close();
            }
        }

        void Debug(string str) {
            try {
                time_t now = time(0);
                char time[100];
                struct tm *timeinfo = localtime(&now);
                strftime(time, 100, "%d%b%y", timeinfo);
                string time_file = time;
                cout << "=================================================================================" << '\n';
                std::ofstream fout("RaptorDenseMatrix_Before" + str + time_file + '_' + ".csv");
                fout
                        << "Username, Rounds Played, Time Played, Revives, Repairs, Heals, Resupplies, Squad Score, Orders_Comp, Flag Captured, Flag Defended, Bombs Placed, Bombs Defused, Game Mode Score, Region, ActInd, PTFOInd, TWInd"
                        << '\n';
                for (int stream = 0; stream < usernames.size(); stream++) {
                    fout << usernames[stream] << ',' << roundsplayed[stream] << ',' << timeplayed[stream] << ','
                         << revives[stream] << ',' << repairs[stream] << ',' << heals[stream] << ','
                         << resupplies[stream] << ',' << squadscore[stream] <<
                         ',' << orders_comp[stream] << ',' << flagcaptured[stream] << ',' << flagdefended[stream] << ','
                         << bombsplaced[stream] << ',' << bombsdefused[stream] << ',' << gamemodescore[stream] << ','
                         << geographic_region[stream] << '\n';
                }
                fout.close();
                cout << "=================================================================================" << '\n';
            }
            catch (std::runtime_error &e) {
                std::cerr << e.what() << "\n";
            }
        }

        //class utilities
        bool statfinder(string data) {
            int pos = 0;
            for (int k = 0; k < finder.size(); k++) {
                pos = data.find(finder[k]);
                if (pos != -1) { return true; }
            }
            return false;
        }

        vector<int> addt_stats(string &sourcecode) {
            /*
			addt_st[0] = Bombs Placed
			addt_st[1] = Bombs Defused
			addt_st[2] = Resupplies
			addt_st[3] = Orders Completed
			*/
            bool flag;
            vector<int> addt_st;
            middlemanstats = eye->SourceCode2StringVector(sourcecode);
            for (int j = 0; j < middlemanstats.size(); j++) {
                flag = statfinder(middlemanstats[j]);
                if (flag == true) {
                    addt_st.push_back(utilities::statreader(middlemanstats[j + 1]));
                }
            }
            return addt_st;
        }

        int region_filter(string rawgeodata) {
            std::vector<string> searcher = {
                    "- US E. -", "- US W. -", "- EU -", "- JP -", "- HK -", "- OC -", "- S. Am. -", "Africa", "Europe",
                    "US East", "US West", "Oceania", "Japan"
            };
            std::vector<string> geo_data = eye->SourceCode2StringVector(rawgeodata);
            std::vector<int> region;
            for (auto &line : geo_data) {
                region.push_back(regionfinder(line, searcher));
            }
            return statistical_mode(region);
        }

        int regionfinder(string data, std::vector<string> &searcher) {
            int pos = 0;
            for (auto &region : searcher) {
                pos = data.find(region);
                if (pos != -1) {
                    if (region == "- US E. -") { return 1; }
                    else if (region == "- US W. -") { return 2; }
                    else if (region == "- EU -") { return 3; }
                    else if (region == "- JP -") { return 4; }
                    else if (region == "- HK -") { return 5; }
                    else if (region == "- OC -") { return 6; }
                    else if (region == "- S. Am. -") { return 7; }
                    else if (region == "US East") { return 1; }
                    else if (region == "US West") { return 2; }
                    else if (region == "Europe") { return 3; }
                    else if (region == "Japan") { return 4; }
                    else if (region == "Hong Kong") { return 5; }
                    else if (region == "Oceania") { return 6; }
                    else if (region == "South America") { return 7; }
                    else if (region == "Africa") { return 8; }
                    else return 0;
                }
            }

            return 0;
        }

        int statistical_mode(const std::vector<int> &v1) {
            std::vector<int> nregions;
            for (int region = 0; region < 9; region++) {
                nregions.push_back(std::count(v1.begin(), v1.end(), region));
            }
            int subregion0 = nregions[0];  //Unknown
            int subregion1 = 0;  //North America
            int subregion2 = nregions[3];  //Europe
            int subregion3 = 0;     //South America, Africa, Oceania and Asia

            for (int n_1region = 1; n_1region < 3; n_1region++) {
                subregion1 += nregions[n_1region];
            }
            for (int n_3region = 4; n_3region < 9; n_3region++) {
                subregion3 += nregions[n_3region];
            }

            if (subregion0 > subregion1 && subregion0 > subregion2 && subregion0 > subregion3) { return 0; }
            else if (subregion1 > subregion0 && subregion1 > subregion2 && subregion1 > subregion3) { return 1; }
            else if (subregion2 > subregion0 && subregion2 > subregion1 && subregion2 > subregion3) { return 2; }
            else if (subregion3 > subregion0 && subregion3 > subregion1 && subregion3 > subregion2) { return 3; }
            else return 0;
        }

        void csv_exporter() {
            string filename;
            cout << "Give a name for the CSV file to be exported (i.e. file.csv): ";
            getline(cin, filename);
            std::ofstream fout(filename);
            fout
                    << "Username, Rounds Played, Time Played, Revives, Repairs, Heals, Resupplies, Squad Score, Orders_Comp, Flag Captured, Flag Defended, Bombs Placed, Bombs Defused, Game Mode Score, Region"
                    << '\n';
            for (int stream = 0; stream < usernames.size(); stream++) {
                fout << usernames[stream] << ',' << droundsplayed[stream] << ',' << dtimeplayed[stream] << ','
                     << drevives[stream] << ',' << drepairs[stream] << ',' << dheals[stream] << ','
                     << dresupplies[stream] << ',' << dsquadscore[stream] <<
                     ',' << dorders_comp[stream] << ',' << dflagcaptured[stream] << ',' << dflagdefended[stream] << ','
                     << dbombsplaced[stream] << ',' << dbombsdefused[stream] << ',' << dgamemodescore[stream] << ','
                     << geographic_region[stream] << '\n';
            }
            fout.close();
        }

        virtual json exporter(int packsize, int starting_point) {
            try {
                // time stamp
                time_t now = time(0);
                string time = ctime(&now);

                json export_package;
                string s;
                cout << "Type in the database directory you will track your stats from: ";
                getline(cin, s);
                cout << '\n';
                export_package["event"] = s;
                int j = 0;
                for (int pos = starting_point; pos < starting_point + packsize; pos++) {
                    // json assembling
                    export_package["players"][j]["player"] = usernames[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["lastupdate"] = time;
                    export_package["players"][j]["games"]["bf1"]["general"]["squad_score"] = squadscore[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["rounds_played"] = roundsplayed[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["repairs"] = repairs[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["resupplies"] = resupplies[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["heals"] = heals[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["revives"] = revives[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["ptfo"] = gamemodescore[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["flags_captured"] = flagcaptured[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["flags_defended"] = flagdefended[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["bombs_placed"] = bombsplaced[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["bombs_defused"] = bombsdefused[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["orders_completed"] = orders_comp[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["time_played"] = timeplayed[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["region"] = geographic_region[pos];
                    j++;
                }
                return export_package;
            }
            catch (std::exception &e) {
                std::cerr << e.what() << '\n';
            }

            return json{};
        }
    };

    class Synchronizer : public Engine {
    protected:
        std::vector<float> vteamwork_index;
        std::vector<float> vptfo_index;
        std::vector<float> vactivity_index;
    public:
        Synchronizer(bool update) : Engine(update) {
            for (int plr = 0; plr < usernames.size(); plr++) {
                teamwork_index(plr);
                ptfo_index(plr);
                activity_index(plr);
                cout << "Indexing successful for plr: " << plr << '\n';
            }
            csv_exporter();
            cout << "Do you wish to sync data with the UNKSO Database? SYNC CLASS" << '\n';
            string op;
            getline(cin, op);
            if (op == "y" || op == "Y") { server_emodule(); }
        }

        void teamwork_index(int player) {
            float numb;
            if (droundsplayed[player] != 0) {
                numb = ((drevives[player] + drepairs[player] + dresupplies[player] + dheals[player]) /
                        droundsplayed[player]) + pow(dsquadscore[player] / droundsplayed[player], 0.25) +
                       2 * (dorders_comp[player] / droundsplayed[player]);
                if (numb >= 0) {
                    vteamwork_index.push_back(numb);
                } else vteamwork_index.push_back(0.0);
            } else {
                vteamwork_index.push_back(0.0);
            }
        }

        void ptfo_index(int player) {
            float numb;
            if (droundsplayed[player] != 0) {
                numb = 2 * (dflagcaptured[player] + dflagdefended[player]) / droundsplayed[player] +
                       4 * (dbombsdefused[player] + dbombsplaced[player]) / droundsplayed[player] +
                       0.15 * dgamemodescore[player] / (400 * log(dgamemodescore[player]));
                if (numb >= 0) {
                    vptfo_index.push_back(numb);
                } else vptfo_index.push_back(0.0);
            } else {
                vptfo_index.push_back(0);
            }
        }

        void activity_index(int player) {
            float numb = pow(dtimeplayed[player], 0.333) + droundsplayed[player];
            if (numb >= 0) {
                vactivity_index.push_back(numb);
            } else vactivity_index.push_back(0.0);
        }

        void server_emodule() {
            int n_packs = rusernames.size() / 500;
            int remainder = rusernames.size() - (500 * n_packs);

            for (int pack = 0; pack < n_packs; pack++) {
                json data = exporter(500, 500 * pack);
                cout << "Sending Raptor Data Pack#" << pack << " to UNKSO Server" << '\n';
                auto r = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                                   cpr::Body{data.dump()},
                                   cpr::Header{{"content-type",  "application/json"},
                                               {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
                if (r.status_code == 200) { cout << "Data has been exported successfully!" << '\n'; }
                else {
                    cout << "Something went wrong when exporting the data. Status Code: " << r.status_code << '\n';
                    cout << r.text << '\n';
                    while (true) {
                        cout << "Attempt to upload again? Type either Y for Yes or N for No." << '\n';
                        string ans;
                        getline(cin, ans);
                        if (ans == "Y" || ans == "y") {
                            auto rx = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                                                cpr::Body{data.dump()},
                                                cpr::Header{{"content-type",  "application/json"},
                                                            {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
                            if (rx.status_code == 200) {
                                cout << "Data has been exported successfully!" << '\n';
                                break;
                            } else
                                cout << "Something went wrong when exporting the data. Status Code: " << r.status_code
                                     << '\n';
                        } else if (ans == "N" || ans == "n") {
                            cout
                                    << "You won't be uploading again. Please report this issue to UNKSO Engineering Department."
                                    << '\n';
                            break;
                        }
                    }
                }
                data.clear();
            }

            json data = exporter(remainder, 500 * n_packs);
            cout << "Sending Raptor Data#" << n_packs + 1 << " to UNKSO Server" << '\n';
            auto r = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                               cpr::Body{data.dump()},
                               cpr::Header{{"content-type",  "application/json"},
                                           {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
            if (r.status_code == 200) { cout << "Data has been exported successfully!" << '\n'; }
            else {
                while (true) {
                    cout << "Attempt to upload again? Type either Y for Yes or N for No." << '\n';
                    string ans;
                    getline(cin, ans);
                    if (ans == "Y" || ans == "y") {
                        auto rx = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                                            cpr::Body{data.dump()},
                                            cpr::Header{{"content-type",  "application/json"},
                                                        {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
                        if (rx.status_code == 200) {
                            cout << "Data has been exported successfully!" << '\n';
                            break;
                        } else
                            cout << "Something went wrong when exporting the data. Status Code: " << r.status_code
                                 << '\n';
                    } else if (ans == "N" || ans == "n") {
                        cout
                                << "You won't be uploading again. Please report this issue to UNKSO Engineering Department."
                                << '\n';
                        break;
                    }
                }
            }
        }

        void csv_exporter() {
            time_t now = time(0);
            char time[100];
            struct tm *timeinfo = localtime(&now);
            strftime(time, 100, "%d%b%y", timeinfo);
            string time_file = time;
            cout << "=================================================================================" << '\n';
            std::ofstream fout("RaptorDenseMatrix_After" + time_file + '_' + db_partition.substr(6, 5) + ".csv");
            fout
                    << "Username, Rounds Played, Time Played, Revives, Repairs, Heals, Resupplies, Squad Score, Orders_Comp, Flag Captured, Flag Defended, Bombs Placed, Bombs Defused, Game Mode Score, Region, ActInd, PTFOInd, TWInd"
                    << '\n';
            for (int stream = 0; stream < usernames.size(); stream++) {
                fout << usernames[stream] << ',' << droundsplayed[stream] << ',' << dtimeplayed[stream] << ','
                     << drevives[stream] << ',' << drepairs[stream] << ',' << dheals[stream] << ','
                     << dresupplies[stream] << ',' << dsquadscore[stream] <<
                     ',' << dorders_comp[stream] << ',' << dflagcaptured[stream] << ',' << dflagdefended[stream] << ','
                     << dbombsplaced[stream] << ',' << dbombsdefused[stream] << ',' << dgamemodescore[stream] << ','
                     << geographic_region[stream] << ',' << vactivity_index[stream] << ',' << vptfo_index[stream] << ','
                     << vteamwork_index[stream] << '\n';
            }
            fout.close();
            cout << "=================================================================================" << '\n';
        }

        json exporter(int packsize, int starting_point) {
            try {
                // time stamp
                time_t now = time(0);
                string time = ctime(&now);

                json export_package;
                string s;
                cout << "Type in the database directory you will track your stats from: ";
                getline(cin, s);
                cout << '\n';
                export_package["event"] = s;
                int j = 0;
                for (int pos = starting_point; pos < starting_point + packsize; pos++) {
                    export_package["players"][j]["player"] = usernames[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["lastupdate"] = time;
                    export_package["players"][j]["games"]["bf1"]["general"]["squad_score"] = squadscore[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["rounds_played"] = roundsplayed[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["repairs"] = repairs[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["resupplies"] = resupplies[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["heals"] = heals[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["revives"] = revives[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["ptfo"] = gamemodescore[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["flags_captured"] = flagcaptured[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["flags_defended"] = flagdefended[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["bombs_placed"] = bombsplaced[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["bombs_defused"] = bombsdefused[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["orders_completed"] = orders_comp[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["time_played"] = timeplayed[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["teamwork_index"] = vteamwork_index[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["ptfo_index"] = vptfo_index[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["activity_index"] = vactivity_index[pos];
                    export_package["players"][j]["games"]["bf1"]["general"]["region"] = geographic_region[pos];
                    j++;
                }
                return export_package;
            }
            catch (std::exception &e) {
                std::cerr << e.what() << '\n';
            }

            return json{};
        }
    };

    class Player {
    protected:
        double tmwork_ind;
        double ptfo_ind;
        double activity_ind;
        string username;
        int region;
    public:
        Player(string _username, double tmw, double ptfo, double activity, int _region) : username(_username),
                                                                                          tmwork_ind(tmw),
                                                                                          ptfo_ind(ptfo),
                                                                                          activity_ind(activity),
                                                                                          region(_region) {}

        friend class Assembler;

        double GTMW() { return tmwork_ind; }

        double GPTFO() { return ptfo_ind; }

        double GACT() { return activity_ind; }

        int GReg() { return region; }

        string GUSN() { return username; }
    };

    class Assembler : public Synchronizer {
    protected:
        std::vector<Player> raptorlist;
    public:
        Assembler(bool update) : Synchronizer(update) {
            Classifyer();
            Exporter();
        }

        void Classifyer() {
            for (int plr = 0; plr < usernames.size(); plr++) {
                raptorlist.emplace_back(rusernames[plr], vteamwork_index[plr], vptfo_index[plr], vactivity_index[plr],
                                        geographic_region[plr]);
            }
            std::sort(raptorlist.begin(), raptorlist.end(),
                      [](Player const &pl1, Player const &pl2) { return pl1.activity_ind > pl2.activity_ind; });
        }

        void Exporter() {
            int op;
            string filename;
            bool flag = true;
            while (flag) {
                cout << "==========================================================================" << '\n';
                cout << "Do you wish to create a Raptor List for players of which region?" << '\n';
                cout << "1 - North America" << '\n';
                cout << "2 - Europe" << '\n';
                cout << "3 - Asia, Oceania, Africa and South America" << '\n';
                cout << "4 - Unknown Regions" << '\n';
                cout << "5 - Exit Exporter Module" << '\n';
                cout << "==========================================================================" << '\n';
                cin >> op;
                cin.get();
                switch (op) {
                    case 1: {
                        cout << "You will export a Raptor List for North American Players" << '\n';
                        time_t now = time(0);
                        char time[100];
                        struct tm *timeinfo = localtime(&now);
                        strftime(time, 100, "%d%b%y", timeinfo);
                        string time_file = time;
                        cout << "================================================================================="
                             << '\n';
                        std::ofstream fout("RaptorNA" + time_file + '_' + db_partition.substr(6, 5) + ".csv");
                        fout
                                << "Username, Activity Index, Teamwork Index, PTFO Index, Region, Sent Friend Request, Accepted Invitation, Sent Message, Replied to Message, Applied to Clan"
                                << '\n';
                        for (auto &plr : raptorlist) {
                            if (plr.region == 1 && plr.activity_ind > 20 && plr.tmwork_ind > 0 && plr.ptfo_ind > 0) {
                                fout << plr.username << ',' << plr.activity_ind << ',' << plr.tmwork_ind << ','
                                     << plr.ptfo_ind << ',' << plr.region << '\n';
                            }
                        }
                        fout.close();
                        cout << "==========================================================================" << '\n';
                        break;
                    }
                    case 2: {
                        cout << "You will export a Raptor List for European Players" << '\n';
                        time_t now = time(0);
                        char time[100];
                        struct tm *timeinfo = localtime(&now);
                        strftime(time, 100, "%d%b%y", timeinfo);
                        string time_file = time;
                        cout << "================================================================================="
                             << '\n';
                        std::ofstream fout("RaptorEU" + time_file + '_' + db_partition.substr(6, 5) + ".csv");
                        fout
                                << "Username, Activity Index, Teamwork Index, PTFO Index, Region, Sent Friend Request, Accepted Invitation, Sent Message, Replied to Message, Applied to Clan"
                                << '\n';
                        for (auto &plr : raptorlist) {
                            if (plr.region == 2 && plr.activity_ind > 20 && plr.tmwork_ind > 0 && plr.ptfo_ind > 0) {
                                fout << plr.username << ',' << plr.activity_ind << ',' << plr.tmwork_ind << ','
                                     << plr.ptfo_ind << ',' << plr.region << '\n';
                            }
                        }
                        fout.close();
                        cout << "==========================================================================" << '\n';
                        break;
                    }
                    case 3: {
                        cout << "You will export a Raptor List for AASAO Players" << '\n';
                        time_t now = time(0);
                        char time[100];
                        struct tm *timeinfo = localtime(&now);
                        strftime(time, 100, "%d%b%y", timeinfo);
                        string time_file = time;
                        cout << "================================================================================="
                             << '\n';
                        std::ofstream fout("RaptorAASAO" + time_file + '_' + db_partition.substr(6, 5) + ".csv");
                        fout
                                << "Username, Activity Index, Teamwork Index, PTFO Index, Region, Sent Friend Request, Accepted Invitation, Sent Message, Replied to Message, Applied to Clan"
                                << '\n';
                        for (auto &plr : raptorlist) {
                            if (plr.region == 3 && plr.activity_ind > 20 && plr.tmwork_ind > 0 && plr.ptfo_ind > 0) {
                                fout << plr.username << ',' << plr.activity_ind << ',' << plr.tmwork_ind << ','
                                     << plr.ptfo_ind << ',' << plr.region << '\n';
                            }
                        }
                        fout.close();
                        cout << "==========================================================================" << '\n';
                        break;
                    }
                    case 4: {
                        cout << "You will export a Raptor List for Unknown Players" << '\n';
                        time_t now = time(0);
                        char time[100];
                        struct tm *timeinfo = localtime(&now);
                        strftime(time, 100, "%d%b%y", timeinfo);
                        string time_file = time;
                        cout << "================================================================================="
                             << '\n';
                        std::ofstream fout("RaptorUnknown" + time_file + '_' + db_partition.substr(6, 5) + ".csv");
                        fout
                                << "Username, Activity Index, Teamwork Index, PTFO Index, Region, Sent Friend Request, Accepted Invitation, Sent Message, Replied to Message, Applied to Clan"
                                << '\n';
                        for (auto &plr : raptorlist) {
                            if (plr.region == 0 && plr.activity_ind > 20 && plr.tmwork_ind > 0 && plr.ptfo_ind > 0) {
                                fout << plr.username << ',' << plr.activity_ind << ',' << plr.tmwork_ind << ','
                                     << plr.ptfo_ind << ',' << plr.region << '\n';
                            }
                        }
                        fout.close();
                        cout << "==========================================================================" << '\n';
                        break;
                    }
                    case 5: {
                        cout << "==========================================================================" << '\n';
                        flag = false;
                    }
                }
            }
        }

        friend std::ostream &operator<<(std::ostream &os, Assembler &obj) {
            string spacer = "   |   ";
            os << "Player     | Tmwork_Ind     | Ptfo_Ind      | Activity_Ind       | " << '\n';
            for (auto &player : obj.raptorlist) {
                os << player.GUSN() << spacer << player.GTMW() << spacer << player.GPTFO() << spacer << player.GACT()
                   << '\n';
            }
            return os;
        }
    };
}

namespace RaptorR6 {
    class Player {
    public:
        string username;
        double mp; // matches played
        double rd; // reinforcements deployed
        double bd; // barricades deployed
        double gd; // gadgets destroyed
        double rv; // revives
        double ka; // kill assists
        double kl; // kills
        double ds; // deaths
        double hs; // headshots
        double mk; // melee kills
        double pk; // penetration kills
        double rank;
        double mmr;
        double bop; // best operator
        double region;
        double a_index;
        double t_index;
        double c_index;

        Player() = default;

        Player(string usr, std::vector<double> ps) : username(usr), mp(ps[2]), rd(ps[5]), bd(ps[6]), gd(ps[9]),
                                                     rv(ps[10]), ka(ps[4]), kl(ps[0]), ds(ps[1]), hs(ps[3]), mk(ps[8]),
                                                     pk(ps[7]), rank(ps[11]), mmr(ps[12]), region(ps[13]) {}

        //obj1 = newest; obj2 = oldest
        friend Player operator-(Player &obj1, Player &obj2) {
            Player obj3;
            obj3.mp = obj1.mp - obj2.mp;
            obj3.rd = obj1.rd - obj2.rd;
            obj3.bd = obj1.bd - obj2.bd;
            obj3.gd = obj1.gd - obj2.gd;
            obj3.rv = obj1.rv - obj2.rv;
            obj3.ka = obj1.ka - obj2.ka;
            obj3.kl = obj1.kl - obj2.kl;
            obj3.ds = obj1.ds - obj2.ds;
            obj3.hs = obj1.hs - obj2.hs;
            obj3.mk = obj1.mk - obj2.mk;
            obj3.pk = obj1.pk - obj2.pk;
            obj3.rank = obj1.rank;
            obj3.mmr = obj1.mmr;
            obj3.bop = obj1.bop;
            obj3.region = obj1.region;
            return obj3;
        }

        friend std::ofstream &operator<<(std::ofstream &fout, const Player &p) {
            fout << p.username << ',' << p.mp << ',' << p.rd << ',' << p.bd << ',' << p.gd << ',' << p.rv << ',' << p.ka
                 << ',' << p.kl << ',' << p.ds << ',' << p.hs << ',' << p.mk << ',' << p.pk << ',' << p.rank << ','
                 << p.mmr << ',' << p.region << '\n';
            return fout;
        }
    };

    typedef std::vector<Player> rPack;

    namespace ar6 {
        // Transforms Sourcecode string to Sourcecode Vector
        std::vector<string> SourceCode2StringVector(string sourcecode) {
            std::vector<string> tx_sourcecode;
            std::string line;
            std::istringstream stream(sourcecode);
            while (std::getline(stream, line)) {
                tx_sourcecode.push_back(line);
            }
            return tx_sourcecode;
        }

        // Returns vector of Sourcecodes
        std::vector<std::vector<string>> player_fetcher(const std::vector<string> &n_users) {
            int j = 0;
            std::vector<cpr::Url> urls;
            string auxurl;
            for (int i = 0; i < n_users.size(); i++) {
                auxurl = "https://r6.tracker.network/profile/pc/" + n_users[i];
                urls.push_back(auxurl);
            }
            cout << "Downloading Parsing Data" << '\n';
            auto container = std::vector<std::future<cpr::Response>>{};
            for (int i = 0; i < n_users.size(); ++i) {
                try {
                    container.emplace_back(cpr::GetAsync(urls[i], cpr::VerifySsl{false}));
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    cout << i << " players' additional stats have been downloaded" << '\n';
                }
                catch (std::exception &e) { std::cerr << e.what() << '\n'; }
            }
            cout << "Wait a few more seconds while the API refreshes and all requests are processed." << '\n';
            std::this_thread::sleep_for(std::chrono::seconds(5));
            cout << "All requests have been downloaded" << '\n';
            std::vector<std::vector<string>> sync_pack;
            for (auto &response : container) {
                auto r = response.get();
                sync_pack.push_back(SourceCode2StringVector(r.text));
            }
            return sync_pack;
        }

        // Transforms Sourcecode into Vector of Stats
        std::vector<double> Processor(std::vector<string> source) {
            std::vector<double> stats;
            vector<string> st(11);
            st[0] = "<div class=\"value\" data-stat=\"PVPKills\">"; //796
            st[1] = "<div class=\"value\" data-stat=\"PVPDeaths\">"; //808
            st[2] = "<div class=\"value\" data-stat=\"PVPMatchesPlayed\">"; //309, 460, 611, 868
            st[3] = "<div class=\"value\" data-stat=\"PVPHeadshots\">"; //892
            st[4] = "<div class=\"value\" data-stat=\"PVPKillAssists\">"; //904
            st[5] = "<div class=\"value\" data-stat=\"PVPReinforcementsDeployed\">"; //1436
            st[6] = "<div class=\"value\" data-stat=\"PVPBarricadesDeployed\">"; //1448
            st[7] = "<div class=\"value\" data-stat=\"PVPPenetrationKills\">"; //1472
            st[8] = "<div class=\"value\" data-stat=\"PVPMeleeKills\">"; //1520
            st[9] = "<div class=\"value\" data-stat=\"PVPGadgetsDestroyed\">"; //1532
            st[10] = "<div class=\"value\" data-stat=\"PVPRevives\">"; //1544

            string aux;
            double rank;
            double mmr;
            double region = 0;
            for (int i = 0; i < source.size(); i++) {
                for (auto &e : st) {
                    if (source[i].find(e) != -1) {
                        aux = source[i + 1];
                        aux.erase(std::remove(aux.begin(), aux.end(), ','), aux.end());
                        stats.push_back(std::stod(aux));
                    }
                }
                if (source[i] == "<div class=\"current-rank\">") {
                    aux = source[i + 1].substr(79, 2);
                    aux.erase(std::remove(aux.begin(), aux.end(), '-'), aux.end());
                    rank = std::stoi(aux);
                    mmr = utilities::statreader(source[i + 4].substr(44));
                }
                if (source[i] == "<option selected=\"selected\" value=\"1\">") {
                    region = 1;
                }
                if (source[i] == "<option selected=\"selected\" value=\"2\">") {
                    region = 2;
                }
                if (source[i] == "<option selected=\"selected\" value=\"3\">") {
                    region = 3;
                }
            }
            stats.push_back(rank);
            stats.push_back(mmr);
            stats.push_back(region);
            stats.erase(stats.begin(), stats.begin() + 3);
            return stats;
        }

        // Auxiliary function that finds stats in Sourcecode (Player Profile)
        bool loggerstatfinder(string dataline) {
            string line_to_find1 = "<div class=\"player\">";
            int pos;
            pos = dataline.find(line_to_find1);
            if (pos != -1) { return 1; }
            return 0;
        }

        // Auxiliary functions that finds players in Sourcecode (Leaderboard Page)
        std::vector<string> Log_Processor(std::vector<string> t_sourcecode, std::vector<string> dbusernames) {
            bool flag;
            int pos_x;
            int pos_xdx;
            string single_user;
            std::vector<string>::iterator unq_plr;
            std::vector<string> log_users;
            for (int i = 0; i < t_sourcecode.size(); i++) {
                flag = loggerstatfinder(t_sourcecode[i]);
                if (flag == 1) {
                    pos_x = t_sourcecode[i + 1].find('>');
                    pos_xdx = t_sourcecode[i + 1].rfind('<');
                    if (pos_x != -1 && pos_xdx != -1) {
                        single_user = t_sourcecode[i + 1].substr(pos_x + 1, pos_xdx - pos_x - 1);
                        unq_plr = std::find(dbusernames.begin(), dbusernames.end(), single_user);
                        if ((unq_plr - dbusernames.begin()) < (dbusernames.end() - dbusernames.begin())) {
                            cout << "A non-unique player has been found : [ID]= " << unq_plr - dbusernames.begin()
                                 << '\n';
                            single_user.clear();
                        } else {
                            log_users.push_back(single_user);
                        }
                    }
                }
            }
            return log_users;
        }

        // Returns vector of already logged Players from UNKSO DB
        std::vector<string> name_fetcher(rPack db) {
            std::vector<string> db_users;
            for (auto &stats : db) {
                db_users.push_back(stats.username);
            }
            return db_users;
        }

        void Exporter(rPack p) {
            string rdb;
            cout << "Type the name of the Raptor Database: ";
            getline(cin, rdb);
            cout << '\n';
            std::ofstream fout("R" + rdb + ".csv");
            if (fout.good()) {
                fout << "UNKSO - Raptor Project" << '\n';
                fout << "Rainbow Six Siege Database - #" + rdb << '\n';
                fout
                        << "================================================================================================================="
                        << '\n';
                fout
                        << "Username, Matches Played, Reinforcements Deployed, Barricades Deployed, Gadgets Destroyed, Revives, Kill Assists, Kills, Deaths, Headshots, Melee Kills, Penetration Kills, Rank, MMR, Region"
                        << '\n';
                fout
                        << "================================================================================================================="
                        << '\n';
                for (auto &pl : p) {
                    fout << pl;
                }
                fout.close();
            } else {
                cout << "Error while Exporting Data" << '\n';
                fout.close();
                Exporter(p);
            }
        }

        rPack Importer() {
            string filename;
            string str;
            vector<double> stats(14, 0);
            string user;
            rPack log;
            cout << "Type the name of the file you want to read: ";
            getline(cin, filename);
            cout << '\n';
            std::ifstream fin(filename);
            if (fin.good()) {
                for (int i = 0; i < 5; i++) { fin.ignore(5000, '\n'); }
                while (!fin.eof()) {
                    cout << "PLAYER" << '\n';
                    getline(fin, str, ',');
                    user = str;
                    getline(fin, str, ',');
                    stats[2] = std::stod(str);
                    getline(fin, str, ',');
                    stats[5] = std::stod(str);
                    getline(fin, str, ',');
                    stats[6] = std::stod(str);
                    getline(fin, str, ',');
                    stats[9] = std::stod(str);
                    getline(fin, str, ',');
                    stats[10] = std::stod(str);
                    getline(fin, str, ',');
                    stats[4] = std::stod(str);
                    getline(fin, str, ',');
                    stats[0] = std::stod(str);
                    getline(fin, str, ',');
                    stats[1] = std::stod(str);
                    getline(fin, str, ',');
                    stats[3] = std::stod(str);
                    getline(fin, str, ',');
                    stats[8] = std::stod(str);
                    getline(fin, str, ',');
                    stats[7] = std::stod(str);
                    getline(fin, str, ',');
                    stats[11] = std::stod(str);
                    getline(fin, str, ',');
                    stats[12] = std::stod(str);
                    getline(fin, str, '\n');
                    stats[13] = std::stod(str);
                    cout << "END OF PLAYER" << '\n';
                    Player p(user, stats);
                    log.emplace_back(p);
                }
                fin.close();
            } else { cout << "Error" << "\n"; }
            return log;
        }
    }

    namespace Routines {
        rPack Logger(int max_players = 1000) {
            auto container = std::vector<std::future<cpr::Response>>{};
            std::vector<string> users;
            std::vector<string> dbusers = ar6::name_fetcher(ar6::Importer()); //PROBLEM
            rPack log_plrs;
            string baseurl = "https://r6.tracker.network/leaderboards/pvp/pc/PVPWLRatio?page=";

            int j;
            cout << "Where do you wish to start? (Page number)" << '\n';
            cin >> j;
            cin.get();

            std::vector<string> aux_users;
            while (true) {
                auto fr = cpr::GetAsync(baseurl + std::to_string(j), cpr::VerifySsl{false});
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                auto r = fr.get();
                aux_users = ar6::Log_Processor(ar6::SourceCode2StringVector(r.text), dbusers);

                int remainder = max_players - users.size();
                if (remainder > aux_users.size()) {
                    remainder = aux_users.size();
                }
                for (int i = 0; i < remainder; i++) {
                    users.push_back(aux_users[j]);
                }
                j++;
                cout << "Number of users logged by the Raptor Tool: " << users.size() << '\n';
                if (users.size() == max_players) { break; }
            }
            for (auto &plr : ar6::player_fetcher(users)) {
                Player p("thing", ar6::Processor(plr)); // FIXME: Use a correct username string
                log_plrs.emplace_back(p); //?
            }
            ar6::Exporter(log_plrs);
            return log_plrs;
        }

        rPack Sync() {
            rPack sync_plrs;
            try {
                std::vector<string> plrs = ar6::name_fetcher(ar6::Importer());
                int i = 0;
                for (auto &plr : ar6::player_fetcher(plrs)) {
                    sync_plrs.emplace_back(plrs[i], ar6::Processor(plr));
                    i += 1;
                }
            }
            catch (std::exception &e) { std::cerr << e.what() << '\n'; }
            return sync_plrs;
        }

        rPack Filter() {
            rPack diff;
            try {
                rPack log = ar6::Importer();
                rPack sync = Sync();
                for (int j = 0; j < log.size(); j++) {
                    diff.push_back(sync[j] - log[j]);
                }
            }
            catch (std::exception &e) { std::cerr << e.what() << "Filter()" << '\n'; }
            return diff;
        }

        rPack Algorithm(rPack p) {
            try {
                for (auto &plr : p) {
                    plr.a_index = utilities::cap(plr.mp, 100);
                    plr.t_index = utilities::cap(
                            (6 * plr.rv + 4.5 * plr.gd + 5.7 * plr.bd + 1.5625 * plr.rd + 7.5 * plr.ka) / plr.mp, 100);
                    if (plr.ds == 0) { plr.ds = 1; }
                    plr.c_index = utilities::cap((plr.kl / plr.ds) * 6 + 0.5 * plr.hs + plr.mk + 0.2 * plr.pk, 100);
                }
            }
            catch (std::exception &e) { std::cerr << e.what() << "Algorithm()" << '\n'; }
            return p;
        }

        void Assembler(rPack fp) {
            try {
                std::sort(fp.begin(), fp.end(),
                          [](Player const &pl1, Player const &pl2) { return pl1.a_index > pl2.a_index; });

                int op;
                string filename;
                cout << "==========================================================================" << '\n';
                cout << "Do you wish to create a Raptor List for players of which region?" << '\n';
                cout << "1 - Americas" << '\n';
                cout << "2 - Europe" << '\n';
                cout << "3 - Asia" << '\n';
                cout << "4 - Exit Assembler Module" << '\n';
                cout << "==========================================================================" << '\n';
                cin >> op;
                cin.get();
                if (op != 4) {
                    cout << "==========================================================================" << '\n';
                    cout << "Type a filename for the Exported Raptor List" << '\n';
                    cout << "Input: ";
                    getline(cin, filename);
                    cout << "==========================================================================" << '\n';
                    std::ofstream fout(filename + ".csv");

                    for (auto &p : fp) {
                        if (p.region == op && p.a_index > 20 && p.t_index > 0 && p.c_index > 0) {
                            fout << p.username << ',' << p.a_index << ',' << p.t_index << ',' << p.c_index << ','
                                 << p.rank << ',' << p.mmr << '\n';
                        }
                    }

                    fout.close();
                    cout << "Do you wish to update UNKSO's DB? (Y/N)" << '\n';
                    string s;
                    getline(cin, s);
                    if (s == "Y" || s == "y") { ar6::Exporter(fp); }
                }
            }
            catch (std::exception &e) { std::cerr << e.what() << "Assembler()" << '\n'; }
        }
    }

    void Header() {
        int op;
        cout << "Welcome to the Raptor Tool! Read the instructions and choose an option to begin working." << '\n';
        cout
                << "========================================================================================================================"
                << '\n';
        cout << "Instructions of Use: " << '\n';
        cout
                << "The Logger module is responsible for searching the Rainbow Six Leaderboards for players to be classified. The Raptor List will not be generated in this step."
                << "The Sync module is responsible for comparing the stats of the players in our database with the stats of an initial time and the most recents stats from the R6 API."
                << "After the update. This module will reunite those who meet the adequeate criteria for being recruited into the =US= and export those players into a .csv file which is what is called the 'Raptor List'.\n";
        cout
                << "========================================================================================================================"
                << '\n';
        cout << "1 - Find and Log more players (Logger)" << '\n';
        cout << "2 - Update Database and Generate Raptor List (Sync)" << '\n';
        cout << "3 - Exit" << '\n';
        cout << "Input: ";
        cin >> op;
        cin.get();
        cout
                << "========================================================================================================================"
                << '\n';
        if (op == 1) {
            Routines::Logger();
        } else if (op == 2) {
            Routines::Assembler(Routines::Algorithm(Routines::Filter()));
        } else {
            cout << "You have not selected a valid option." << '\n';
            cout
                    << "====================================================================================================================================================================================================="
                    << '\n';
        }
    }
}

class Interface {
    //Implement Branch Perf Index
    //Implement BF1 & BF4 DAccuracy
protected:
    int game;
    bool flag;
    BF1::BF1Tracker *bf1tracker;
    BF4Tracker *bf4tracker;
    Siege::SiegeTracker *r6tracker;
    //
    std::vector<BF1::BF1Tracker *> bf1container;
    std::vector<BF4Tracker *> bf4container;
    std::vector<Siege::SiegeTracker *> r6container;
    string username;
    int updateoption;
public:
    ~Interface() {
        delete bf1tracker;
        delete bf4tracker;
        delete r6tracker;
    }

    Interface() {
        string s;
        while (true) {
            bf1tracker = nullptr;
            bf4tracker = nullptr;
            r6tracker = nullptr;
            cout << "========================================================================" << '\n';
            cout << "To start, choose a game to track statistics from: " << '\n';
            cout << "1 - Battlefield 1 (Standard username = 'RANK_NAME')" << '\n';
            cout << "2 - Battlefield 4 (Standard username = 'RANK_NAME')" << '\n';
            cout << "3 - Rainbow Six Siege (Standard username = 'NAME.US')" << '\n';
            cout << "4 - Exit PROGRAM" << '\n';
            cout << "========================================================================" << '\n';
            cout << "Input: ";
            cin >> game;
            cin.get();
            if (game == 4) { break; }
            cout << "========================================================================" << '\n';
            cout << "Input username: ";
            getline(cin, username);
            cout << "========================================================================" << '\n';
            if (username == "exit") { break; }
            flag = check_player(username, game);  //check to see if player exists
            if (flag == true) {
                string ans;
                switch (game) {
                    case 1: {
                        while (true) {
                            BF1Interface(username);
                            bf1container.push_back(bf1tracker);
                            cout << "========================================================================" << '\n';
                            cout << "Would you like to continue with another BF1 player? (Y/N)" << '\n';
                            cout << "========================================================================" << '\n';
                            cout << "Input: ";
                            getline(cin, ans);
                            cout << "========================================================================" << '\n';
                            if (ans == "Y" || ans == "y") {
                                cout << "Type in the next username: ";
                                getline(cin, username);
                                flag = check_player(username, game);
                                if (flag == false) { break; }
                            } else { break; }
                        }
                        break;
                    }
                    case 2: {
                        while (true) {
                            BF4Interface(username);
                            bf4container.push_back(bf4tracker);
                            cout << "========================================================================" << '\n';
                            cout << "Would you like to continue with another BF4 player? (Y/N)" << '\n';
                            cout << "========================================================================" << '\n';
                            cout << "Answer: ";
                            getline(cin, ans);
                            if (ans == "Y" || ans == "y") {
                                cout << "Type in the next username: ";
                                getline(cin, username);
                                flag = check_player(username, game);
                                if (flag == false) { break; }
                            } else { break; }
                        }
                        break;
                    }
                    case 3: {
                        while (true) {
                            SiegeInterface(username);
                            r6container.push_back(r6tracker);
                            cout << "========================================================================" << '\n';
                            cout << "Would you like to continue with another Siege player? (Y/N)" << '\n';
                            cout << "========================================================================" << '\n';
                            cout << "Answer: ";
                            getline(cin, ans);
                            if (ans == "Y" || ans == "y") {
                                cout << "Type in the next username: ";
                                getline(cin, username);
                                flag = check_player(username, game);
                                if (flag == false) { break; }
                            } else { break; }
                        }
                        break;
                    }
                }
                cout << "========================================================================" << '\n';
            }
            cout << "Do you desire to see all the stats you have so far?" << "\n";
            getline(cin, s);
            if (s == "Y" || s == "y") {
                cout << "Select a game: " << '\n';
                cout << "1 - Battlefield 1" << '\n';
                cout << "2 - Battlefield 4" << '\n';
                cout << "3 - Rainbow Six Siege" << '\n';
                cin >> game;
                cin.get();
                switch (game) {
                    case 1: {
                        for (int i = 0; i < bf1container.size(); i++) {
                            cout << bf1container[i]->username << ": " << bf1container[i]->perfindex << '\n';
                        }
                        break;
                    }
                    case 2: {
                        for (int i = 0; i < bf4container.size(); i++) {
                            cout << bf4container[i]->username << ": " << bf4container[i]->perfindex << '\n';
                        }
                        break;
                    }
                    case 3: {
                        for (int i = 0; i < r6container.size(); i++) {
                            cout << r6container[i]->username << ": " << r6container[i]->perfindex << '\n';
                        }
                        break;
                    }
                }
            }
        }
    }

    Interface(std::vector<string> namelist, std::vector<int> gamelist, int u_option) : updateoption(u_option) {
        cout << "========================================================================" << '\n';
        cout << "Processing all players..." << '\n';
        cout << "========================================================================" << '\n';
        int game;
        string s;
        if (namelist.size() == gamelist.size()) {
            for (int j = 0; j < namelist.size(); j++) {
                bf1tracker = nullptr;
                bf4tracker = nullptr;
                r6tracker = nullptr;
                flag = check_player(namelist[j], gamelist[j]);
                if (flag == true) {
                    switch (gamelist[j]) {
                        case 1: {
                            mBF1Interface(namelist[j]);
                            bf1container.push_back(bf1tracker);
                            break;
                        }
                        case 2: {
                            mBF4Interface(namelist[j]);
                            bf4container.push_back(bf4tracker);
                            break;
                        }
                        case 3: {
                            mSiegeInterface(namelist[j]);
                            r6container.push_back(r6tracker);
                            break;
                        }
                    }
                }
            }
        } else { cout << "Namelist Vector is not the same in size as Gamelist vector!" << '\n'; }
        cout << "========================================================================" << '\n';
        cout << "Do you desire to see all the stats you have so far?" << "\n";
        cout << "Input: ";
        getline(cin, s);
        cout << "========================================================================" << '\n';
        if (s == "Y" || s == "y") {
            while (true) {
                cout << "Select a game: " << '\n';
                cout << "1 - Battlefield 1" << '\n';
                cout << "2 - Battlefield 4" << '\n';
                cout << "3 - Rainbow Six Siege" << '\n';
                cout << "4 - Exit" << '\n';
                cout << "========================================================================" << '\n';
                cout << "Input: ";
                cin >> game;
                cin.get();
                if (game == 4) { break; }
                switch (game) {
                    case 1: {
                        cout << "========================================================================" << '\n';
                        cout << "Individual BF1 Players and their Performance Indexes" << '\n';
                        cout << "----------------------------------------------------" << '\n';
                        for (int i = 0; i < bf1container.size(); i++) {
                            cout << bf1container[i]->username << ": " << bf1container[i]->perfindex << '\n';
                            cout << "   -------" << '\n';
                        }
                        cout << "========================================================================" << '\n';
                        break;
                    }
                    case 2: {
                        cout << "========================================================================" << '\n';
                        cout << "Individual BF4 Players and their Performance Indexes" << '\n';
                        cout << "----------------------------------------------------" << '\n';
                        for (int i = 0; i < bf4container.size(); i++) {
                            cout << bf4container[i]->username << ": " << bf4container[i]->perfindex << '\n';
                            cout << "   -------" << '\n';
                        }
                        cout << "========================================================================" << '\n';
                        break;
                    }
                    case 3: {
                        cout << "========================================================================" << '\n';
                        cout << "Individual Siege Players and their Performance Indexes" << '\n';
                        cout << "------------------------------------------------------" << '\n';
                        for (int i = 0; i < r6container.size(); i++) {
                            cout << r6container[i]->username << ": " << r6container[i]->perfindex << '\n';
                            cout << "   -------" << '\n';
                        }
                        cout << "========================================================================" << '\n';
                        break;
                    }
                }
            }
        }
        cout << "Calculating weighted branch performance index!" << '\n';
        cout << "Navy (BF1): " << utilities::branchperfcalculator<std::vector<BF1::BF1Tracker *>>(bf1container) << '\n';
        cout << "Air Force (BF4): " << utilities::branchperfcalculator<std::vector<BF4Tracker *>>(bf4container) << '\n';
        cout << "Coast Guard (R6 Siege): "
             << utilities::branchperfcalculator<std::vector<Siege::SiegeTracker *>>(r6container) << '\n';
        system("Pause");
    }

    //Single User Interface Functions
    void BF4Interface(string username) {
        string ans;
        cout << "========================================================================" << '\n';
        cout << "                        AVAILABLE OPTIONS                               " << '\n';
        cout << "                        -----------------								 " << '\n';
        cout << "1 - MWR IBCC Tournament Tracking" << '\n';
        cout << "2 - UNKSO Leaderboards (NOT IMPLEMENTED)" << '\n';
        cout << "3 - Combat MOS Tracking (NOT IMPLEMENTED)" << '\n';
        cout << "4 - Basic Combat Training Tracking (NOT IMPLEMENTED)" << '\n';
        cout << "5 - EXIT TO GAME INTERFACE" << '\n';
        cout << "========================================================================" << '\n';
        cout << "Input: ";
        getline(cin, ans);
        cout << "========================================================================" << '\n';
        if (ans == "1") {
            bf4tracker = new BF4Tracker(username);
            if (bf4tracker->flag == false) {
                cout << "You have not started tracking the player: " << username
                     << ". Do you desire to do it now? (Y/N)" << '\n';
                getline(cin, ans);
                if (ans == "Y" || ans == "y") {
                    data_exporter(username);
                    cout << "This player is being tracked!" << '\n';
                } else {
                    cout << "You will not be tracking this player. The Performance Index will not be calculated."
                         << '\n';
                }
            } else if (bf4tracker->flag == true) {
                cout
                        << "Tracking for this player is possible. Would you like to calculate his/hers performance index? (Y/N)"
                        << '\n';
                getline(cin, ans);
                if (ans == "Y" || ans == "y") { bf4tracker->PerfIndexBF4(); }
                else { cout << "The Performance Index will not be calculated." << '\n'; }
                cout << "Would you like to update " << username << "'s data? (Y/N)" << '\n';
                getline(cin, ans);
                if (ans == "Y" || ans == "y") {
                    data_exporter(username);
                } else { cout << "Data for this player will not be updated!" << '\n'; }
            }
        } else if (ans == "5") { cout << "Exiting..." << '\n'; }
        else { cout << "ACCESS DENIED!" << '\n'; }
    }

    void BF1Interface(string username) {
        string ans;
        cout << "========================================================================" << '\n';
        cout << "                        AVAILABLE OPTIONS                               " << '\n';
        cout << "                        -----------------								 " << '\n';
        cout << "1 - MWR IBCC Tournament Tracking" << '\n';
        cout << "2 - UNKSO Leaderboards (NOT IMPLEMENTED)" << '\n';
        cout << "3 - Combat MOS Tracking (NOT IMPLEMENTED)" << '\n';
        cout << "4 - Basic Combat Training Tracking (NOT IMPLEMENTED)" << '\n';
        cout << "5 - EXIT TO GAME INTERFACE" << '\n';
        cout << "========================================================================" << '\n';
        cout << "Input: ";
        getline(cin, ans);
        cout << "========================================================================" << '\n';
        if (ans == "1") {
            bf1tracker = new BF1::BF1Tracker(username);
            if (bf1tracker->flag == false) {
                cout << "You have not started tracking the player: " << username
                     << ". Do you desire to do it now? (Y/N)" << '\n';
                getline(cin, ans);
                if (ans == "Y" || ans == "y") {
                    data_exporter(username);
                    cout << "This player is being tracked!" << '\n';
                } else {
                    cout << "You will not be tracking this player. The Performance Index will not be calculated."
                         << '\n';
                }
            } else if (bf1tracker->flag == true) {
                cout
                        << "Tracking for this player is possible. Would you like to calculate his/hers performance index? (Y/N)"
                        << '\n';
                getline(cin, ans);
                if (ans == "Y" || ans == "y") { bf1tracker->PerfIndexBF1(); }
                else { cout << "The Performance Index will not be calculated." << '\n'; }
                cout << "Would you like to update " << username << "'s data? (Y/N)" << '\n';
                getline(cin, ans);
                if (ans == "Y" || ans == "y") {
                    data_exporter(username);
                } else { cout << "Data for this player will not be updated!" << '\n'; }
            }
        } else if (ans == "5") { cout << "Exiting..." << '\n'; }
        else { cout << "ACCESS DENIED!" << '\n'; }
    }

    void SiegeInterface(string username) {
        string ans;
        cout << "========================================================================" << '\n';
        cout << "                        AVAILABLE OPTIONS                               " << '\n';
        cout << "                        -----------------								 " << '\n';
        cout << "1 - MWR IBCC Tournament Tracking" << '\n';
        cout << "2 - UNKSO Leaderboards (NOT IMPLEMENTED)" << '\n';
        cout << "3 - Combat MOS Tracking (NOT IMPLEMENTED)" << '\n';
        cout << "4 - Basic Combat Training Tracking (NOT IMPLEMENTED)" << '\n';
        cout << "5 - EXIT TO GAME INTERFACE" << '\n';
        cout << "========================================================================" << '\n';
        cout << "Input: ";
        getline(cin, ans);
        cout << "========================================================================" << '\n';
        if (ans == "1") {
            r6tracker = new Siege::SiegeTracker(username);
            if (r6tracker->trackerflag == false) {
                cout << "You have not started tracking the player: " << username
                     << ". Do you desire to do it now? (Y/N)" << '\n';
                getline(cin, ans);
                if (ans == "Y" || ans == "y") {
                    data_exporter(username);
                    cout << "This player is being tracked!" << '\n';
                } else {
                    cout << "You will not be tracking this player. The Performance Index will not be calculated."
                         << '\n';
                }
            } else if (r6tracker->trackerflag == true) {
                cout
                        << "Tracking for this player is possible. Would you like to calculate his/hers performance index? (Y/N)"
                        << '\n';
                getline(cin, ans);
                if (ans == "Y" || ans == "y") { r6tracker->PerfIndexSiege(); }
                else { cout << "The Performance Index will not be calculated." << '\n'; }
                cout << "Would you like to update " << username << "'s data? (Y/N)" << '\n';
                getline(cin, ans);
                if (ans == "Y" || ans == "y") {
                    data_exporter(username);
                } else { cout << "Data for this player will not be updated!" << '\n'; }
            }
        } else if (ans == "5") { cout << "Exiting..." << '\n'; }
        else { cout << "ACCESS DENIED!" << '\n'; }
    }

    //Multi-user Interface Functions
    void mBF4Interface(string username) {
        try {
            string option;
            bf4tracker = new BF4Tracker(username);
            if (bf4tracker->flag == false) { data_exporter(username); }
            else if (bf4tracker->flag == true) {
                bf4tracker->PerfIndexBF4(false);
                if (updateoption == 1) {
                    data_exporter(username);
                } else if (updateoption == 2) {
                    cout << "========================================================================" << '\n';
                    cout << "Update " << username << "'s data?" << '\n';
                    getline(cin, option);
                    cout << "========================================================================" << '\n';
                    if (option == "Y" || option == "y") {
                        data_exporter(username);
                    }
                    cout << "========================================================================" << '\n';
                }
                //----------------------------------------------------//
            }
        }
        catch (std::exception &e) {
            std::cerr << e.what() << '\n';
        }
    }

    void mBF1Interface(string username) {
        try {
            string option;
            bf1tracker = new BF1::BF1Tracker(username);
            if (bf1tracker->flag == false) { data_exporter(username); }
            else if (bf1tracker->flag == true) {
                bf1tracker->PerfIndexBF1(false);
                if (updateoption == 1) {
                    data_exporter(username);
                } else if (updateoption == 2) {
                    cout << "========================================================================" << '\n';
                    cout << "Update " << username << "'s data?" << '\n';
                    getline(cin, option);
                    cout << "========================================================================" << '\n';
                    if (option == "Y" || option == "y") {
                        data_exporter(username);
                    }
                    cout << "========================================================================" << '\n';
                }
            }
        }
        catch (std::exception &e) {
            std::cerr << e.what() << '\n';
        }
    }

    void mSiegeInterface(string username) {
        try {
            string option;
            r6tracker = new Siege::SiegeTracker(username);
            if (r6tracker->trackerflag == false) { data_exporter(username); }
            else if (r6tracker->trackerflag == true) {
                r6tracker->PerfIndexSiege(false);
                if (updateoption == 1) {
                    data_exporter(username);
                } else if (updateoption == 2) {
                    cout << "========================================================================" << '\n';
                    cout << "Update " << username << "'s data?" << '\n';
                    getline(cin, option);
                    cout << "========================================================================" << '\n';
                    if (option == "Y" || option == "y") {
                        data_exporter(username);
                    }
                    cout << "========================================================================" << '\n';
                } else {
                    cout << "Players will not be synchronized with UNKSO Database!" << '\n';
                    cout << "===========================================================================" << '\n';
                }
            }
        }
        catch (std::exception &e) {
            std::cerr << e.what() << '\n';
        }
    }

    //Class Utilities
    json package_exp(string username) {
        json pack;
        pack["event"] = "ibcc";
        if (bf1tracker != nullptr) {
            pack["players"][0]["player"] = username_stripper(username);
            pack["players"][0]["games"]["bf1"]["general"] = bf1tracker->exporter();
        }
        if (bf4tracker != nullptr) {
            pack["players"][0]["player"] = username_stripper(username);
            pack["players"][0]["games"]["bf4"]["general"] = bf4tracker->exporter();
        }
        if (r6tracker != nullptr) {
            pack["players"][0]["player"] = username_stripper(username);
            pack["players"][0]["games"]["r6siege"]["general"] = r6tracker->exporter();
        }
        return pack;
    }

    void data_exporter(string username) {
        json data = package_exp(username);
        auto r = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                           cpr::Body{data.dump()},
                           cpr::Header{{"content-type",  "application/json"},
                                       {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
        if (r.status_code == 200) { cout << "Data has been exported successfully!" << '\n'; }
        else { cout << "Something went wrong when exporting the data. Status Code: " << r.status_code << '\n'; }
    }

    string username_stripper(string &username) {
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

    bool check_player(string username, int game) {
        if (game == 1) {
            DataFetcher test_fet(
                    "https://battlefieldtracker.com/bf1/api/Stats/DetailedStats?platform=3&displayName=" + username,
                    "TRN-Api-Key", "152de4e7-22a4-4a0a-b2b6-2bea2c7886b6");
            cout << "Checking BF1 Online Database for player: " << username << '\n';
            if (test_fet.GStatusCode() != 200) {
                cout
                        << "This player has not been found! Please ensure your username adheres to UNKSO standards or that you have the correct updated username."
                        << '\n';
                cout << "Status Code" << test_fet.GStatusCode() << '\n';
                return false;
            } else {
                cout << "Player has been found in online database!" << '\n';
                return true;
            }
        }
        if (game == 2) {
            DataFetcher test_fet("http://api.bf4stats.com/api/playerInfo?plat=pc&name=" + username + "&output=json");
            cout << "Checking BF4 Online Database for player: " << username << '\n';
            if (test_fet.GStatusCode() != 200) {
                cout
                        << "This player has not been found! Please ensure your username adheres to UNKSO standards or that you have the correct updated username."
                        << '\n';
                return false;
            } else {
                cout << "Player has been found in online database!" << '\n';
                return true;
            }
        }
        if (game == 3) {
            DataFetcher test_fet("https://r6.tracker.network/profile/pc/" + username);
            cout << "Checking Siege Online Database for player: " << username << '\n';
            int pos = test_fet.GetSource().find(
                    "We could not find your stats, please ensure your platform and name are correct");
            if (pos != -1) {
                cout
                        << "Player has not been found in Online Database! Please ensure your username adheres to UNKSO standards or that you have the correct updated username."
                        << '\n';
                return false;
            } else {
                cout << "Player has been found in Online Database" << '\n';
                return true;
            }
        }

        return false;
    }
};

void API() {
    string st;
    std::vector<int> size;
    std::vector<string> lstupdate;
    std::vector<string> dirname;
    string inconsistency;
    cout << "Perform Update Inconsistency Analysis? ";
    getline(cin, inconsistency);
    cout << '\n';
    if (inconsistency == "Y" || inconsistency == "y") {
        while (true) {
            cout << "Type in the database directory you're tracking your stats from: ";
            getline(cin, st);
            cout << '\n';
            if (st == "exit") { break; }
            string url = "http://unkso.michaeljamesbondparsons.com/stats/bf1/latest?events[0]=" + st;
            auto r = cpr::Get(cpr::Url{url}, cpr::Header{{"content-type",  "application/json"},
                                                         {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
            if (r.text != "{\"players\":[]}") {
                json pack = json::parse(r.text);
                pack = pack["players"];
                cout << "Player " << "|" << " Updated at" << '\n';
                for (int j = 0; j < pack.size(); j++) {
                    cout << pack[j]["general"]["updated_at"] << '\n';
                }
            }
        }
    }
    while (true) {
        cout << "Type in the database directory you're tracking your stats from: ";
        getline(cin, st);
        cout << '\n';
        if (st == "exit") { break; }
        string url = "http://unkso.michaeljamesbondparsons.com/stats/bf1/latest?events[0]=" + st;
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
    cout << "Directory Name | Nº of Players | Last Update " << '\n';
    for (int i = 0; i < dirname.size(); i++) {
        cout << std::setw(14) << dirname[i] << " | " << std::setw(13) << size[i] << " players" << " | " << std::setw(11)
             << lstupdate[i] << '\n';
    }
}

void InterfaceHeader() {
    int op;
    std::vector<string> players;
    string player;
    std::vector<int> games;
    int game;
    cout << "Choose an option: " << '\n';
    cout << "1 - Single user Interface (Useful for first-time logging)" << '\n';
    cout << "2 - Multi-user Interface (Useful for update process for experienced users)" << '\n';
    cout << "3 - Speedy Interface" << '\n';
    cout << "===========================================================================" << '\n';
    cout << "Input: ";
    cin >> op;
    cin.get();
    cout << "===========================================================================" << '\n';
    if (op == 1) { Interface intf; }
    else if (op == 2) {
        while (true) {
            cout
                    << "Input the name of the desired players, for each of them, press ENTER to confirm. Type 'Exit' to leave."
                    << '\n';
            cout << "Input: ";
            getline(cin, player);
            if (player == "exit" || player == "EXIT" || player == "Exit") {
                break;
            }
            players.push_back(player);
            cout << "========================================================================" << '\n';
            cout << "For player " << player << " choose an option: " << '\n';
            cout << "1 - Battlefield 1" << '\n';
            cout << "2 - Battlefield 4" << '\n';
            cout << "3 - Rainbow Six Siege" << '\n';
            cout << "========================================================================" << '\n';
            cout << "Input: ";
            cin >> game;
            cin.get();
            games.push_back(game);
            cout << "========================================================================" << '\n';
        }
        int option;
        cout << "Synchronize all players with UNKSO Online Database?" << '\n';
        cout << "1 - Yes" << '\n';
        cout << "2 - Mixed" << '\n';
        cout << "3 - No" << '\n';
        cin >> option;
        cin.get();
        Interface intf(players, games, option);
    } else if (op == 3) {
        string filename;
        cout << "Input the filename plus extension, please. (Ex.: 'Input.txt')" << '\n';
        cout << "Input: ";
        getline(cin, filename);
        cout << "========================================================================" << '\n';
        cout << "             " + filename + "'s contents:" << '\n';
        std::ifstream fin(filename);
        if (fin.good()) {
            for (int i = 0; i < 15; i++) {
                fin.ignore(5000, '\n');
            }
            while (!fin.eof()) {
                getline(fin, player, ',');
                players.push_back(player);
                fin >> game;
                games.push_back(game);
                fin.ignore(5000, '\n');
                cout << "Player: " << player << "  " << "Game: " << game << '\n';
            }
            cout << "Total of Players: " << players.size() << '\n';
            cout << "========================================================================" << '\n';
        }
        fin.close();
        int option;
        cout << "Synchronize all players with UNKSO Online Database?" << '\n';
        cout << "1 - Yes" << '\n';
        cout << "2 - Mixed" << '\n';
        cout << "3 - No" << '\n';
        cout << "========================================================================" << '\n';
        cout << "Input: " << '\n';
        cin >> option;
        cin.get();
        Interface intf(players, games, option);
    }
}

void RaptorHeader() {
    int op;
    cout << "Welcome to the Raptor Tool! Read the instructions and choose an option to begin working." << '\n';
    cout
            << "========================================================================================================================"
            << '\n';
    cout << "Instructions of Use: " << '\n';
    cout
            << "The Logger module is responsible for searching the Battlefield 1 Leaderboards for players to be classified. The Raptor List will not be generated in this step."
            << "The Sync module is responsible for comparing the stats of the players in our database with the stats of an initial time and the most recents stats from the BF1 API."
            << "After the update. This module will reunite those who meet the adequeate criteria for being recruited into the =US= and export those players into a .csv file which is what is called the 'Raptor List'.\n";
    cout
            << "========================================================================================================================"
            << '\n';
    cout << "1 - Find and Log more players (Logger)" << '\n';
    cout << "2 - Update Database and Generate Raptor List (Sync)" << '\n';
    cout << "3 - Exit" << '\n';
    cout << "Input: ";
    cin >> op;
    cin.get();
    cout
            << "========================================================================================================================"
            << '\n';
    if (op == 1) {
        Raptor::Engine logger(false);
    } else if (op == 2) {
        Raptor::Assembler raptorlist(true);
    } else {
        cout << "You have not selected a valid option." << '\n';
        cout
                << "====================================================================================================================================================================================================="
                << '\n';
    }
}

void BF1API(string name) {
    auto r = cpr::Get(
            cpr::Url{"https://battlefieldtracker.com/bf1/api/Stats/DetailedStats?platform=3&displayName=" + name},
            cpr::VerifySsl{false}, cpr::Header{{"TRN-Api-Key", "152de4e7-22a4-4a0a-b2b6-2bea2c7886b6"}});
    cout << r.text << '\n';
    cout << r.status_code << '\n';
    //auto x = cpr::Get(cpr::Url{ "http://unkso.michaeljamesbondparsons.com/stats/bf1/latest?player[0]=" + name }, cpr::Header{ { "content-type", "application/json" },{ "Authorization","Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B" } });
    //cout << x.text << '\n';
}

int main() {
    //setlocale(LC_ALL, "Portuguese");
    system("color F0");
    string name;
    //string plr;
    /*while (true) {
	cout << "Type da' name: "; getline(cin, plr); cout << '\n'; if (plr == "exit") { break;  }
	string url = "https://battlefieldtracker.com/bf1/api/Stats/DetailedStats?platform=3&displayName=" + plr;
	auto r = cpr::Get(cpr::Url{ url }, cpr::VerifySsl(false), cpr::Header{ { "content-type", "application/json" },{ "TRN-Api-Key", "152de4e7-22a4-4a0a-b2b6-2bea2c7886b6"} });
	cout << r.text << '\n';
	}*/
    time_t now = time(0);
    char time[100];
    struct tm *timeinfo = localtime(&now);
    strftime(time, 100, "%d%b%y", timeinfo);

    int answer;
    while (true) {
        cout
                << "========================================================================================================================"
                << '\n';
        cout << "UNKSO Tracker + Raptor Tool v1.00 - Engineering Prototype" << '\n';
        cout << "UNKSO Intelligence Corps" << '\n';
        cout << time << '\n';
        cout << "Set the window to fullscreen size for better view.\n";
        cout
                << "========================================================================================================================"
                << '\n';
        cout << "Choose one program: " << '\n';
        cout << "1 - UNKSO Tracker" << '\n';
        cout << "2 - Raptor Experiment" << '\n';
        cout << "3 - Database Management" << '\n';
        cout << "4 - BF4 API" << '\n';
        cout << "5 - Exit" << '\n';
        cout << "Input: ";
        cin >> answer;
        cin.get();
        cout
                << "========================================================================================================================"
                << '\n';
        if (answer == 1) { InterfaceHeader(); }
        else if (answer == 2) { RaptorHeader(); }
        else if (answer == 3) { API(); }
        else if (answer == 4) {
            cout << "Type it: ";
            getline(cin, name);
            cout << '\n';
            BF1API(name);
        } else if (answer == 5) { break; }
    }
    return 0;
}
