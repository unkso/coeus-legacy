//
// Created by Eric Dattore on 8/25/18.
//
#include "bf4.hpp"
#include "utilities.hpp"

BF4Tracker::~BF4Tracker() { delete dft; }

BF4Tracker::BF4Tracker(std::string _username) : username(_username) {
    //Reading updated data
    dft = new DataFetcher("http://api.bf4stats.com/api/playerInfo?plat=pc&name=" + username + "&output=json");
    if (dft->GStatusCode() == 200) {
        std::cout << "------------------------------------------------------------------------" << '\n';
        std::cout << "All statistics have been downloaded for player " << username << '\n';
        std::cout << "------------------------------------------------------------------------" << '\n';
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
        std::cout << "Something went wrong when fetching user's data from the online BF4 API" << '\n';
    }
}

void BF4Tracker::readsaveddata() {
    //
    std::string processed_username;
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
            std::cout << "This player's stats have been found in UNSKO Online Database" << '\n';
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
            std::cout << "This player has not been found in UNKSO Online Database" << '\n';
            flag = false;
        }
    } else {
        std::cout << "Something went wrong when trying to establish connection to WEB API. HTTP Status CODE: "
                  << r.status_code << '\n';
        flag = false;
    }
}

void BF4Tracker::savedata() {
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

void BF4Tracker::Variation() {
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

void BF4Tracker::PerfIndexBF4(bool outputflag) {
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
            std::cout << "========================================================================" << '\n';
            std::cout << "The Performance Index has been calculated. Choose an option to see the results." << '\n';
            std::cout << "1 - Basic Report" << '\n';
            std::cout << "2 - Detailed Report" << '\n';
            std::cout << "3 - Full Report" << '\n';
            std::cout << "========================================================================" << '\n';
            std::cout << "Input: ";
            std::cin >> op;
            std::cin.get();
            std::cout << "========================================================================" << '\n';
            switch (op) {
                case 1: {
                    std::cout << std::left;
                    std::cout << "Performance Index Basic Report" << '\n';
                    std::cout << "Player: " << username << '\n';
                    std::cout << "Perf.Index: " << perfindex << '\n';
                    std::cout << "End of Report" << '\n';
                    std::cout << "========================================================================" << '\n';
                    break;
                }
                case 2: {
                    std::cout << std::left;
                    std::cout << "Performance Index Detailed Report" << '\n';
                    std::cout << "Player: " << username << '\n';
                    std::cout << "Perf.Index: " << perfindex << '\n';
                    std::cout << "-----------------------------------" << '\n';
                    std::cout << "Individual Index Components" << '\n';
                    std::cout << "Score Index: " << indexss << '\n';
                    std::cout << "Team Stats Index: " << indexts << '\n';
                    std::cout << "Combat Stats Index: " << indexcts << '\n';
                    std::cout << "Activity Index (Rounds Played): " << indexrp << '\n';
                    std::cout << "PTFO Index: " << indexptfo << '\n';
                    std::cout << "End of Report" << '\n';
                    std::cout << "========================================================================" << '\n';
                    break;
                }
                case 3: {
                    std::cout << std::left;
                    std::cout << "Performance Index Full Report" << '\n';
                    std::cout << "Player: " << username << '\n';
                    std::cout << "Perf.Index: " << perfindex << '\n';
                    std::cout << "-----------------------------------" << '\n';
                    std::cout << "Individual Index Components" << '\n';
                    std::cout << "Score Index: " << indexss << '\n';
                    std::cout << "Team Stats Index: " << indexts << '\n';
                    std::cout << "Combat Stats Index: " << indexcts << '\n';
                    std::cout << "Activity Index (Rounds Played): " << indexrp << '\n';
                    std::cout << "PTFO Index: " << indexptfo << '\n';
                    std::cout << "-----------------------------------" << '\n';
                    std::cout << "Delta Factor Components" << '\n';
                    std::cout << "DScore: " << dss << '\n';
                    std::cout << "DRoundsPlayed: " << drp << '\n';
                    std::cout << "DSuppressionAssists: " << dsa << '\n';
                    std::cout << "DKillAssists: " << dka << '\n';
                    std::cout << "DResupplies: " << drsp << '\n';
                    std::cout << "DRevives: " << drvs << '\n';
                    std::cout << "DRepairs: " << drps << '\n';
                    std::cout << "DHeals: " << dhls << '\n';
                    std::cout << "DKills: " << dkills << '\n';
                    std::cout << "DHeadshots: " << dhs << '\n';
                    std::cout << "Accuracy: " << dacc << '\n';
                    std::cout << "DDogtagsTaken: " << ddgtgs << '\n';
                    std::cout << "DPTFO: " << dptfo << '\n';
                    std::cout << "End of Report" << '\n';
                    std::cout << "========================================================================" << '\n';
                    break;
                }
            }
        }
    }
}

json BF4Tracker::exporter() {
    // time stamp
    time_t now = time(0);
    std::string time = ctime(&now);
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

int BF4Tracker::GetRP() { return drp; }

double BF4Tracker::GetPIndex() { return perfindex; }
