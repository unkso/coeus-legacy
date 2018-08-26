//
// Created by Eric Dattore on 8/24/18.
//
#include "utilities.hpp"
#include "bf1.hpp"

namespace BF1 {
    BF1Stats::BF1Stats(std::string username) {
        std::cout << "------------------------------------------------------------------------" << '\n';
        dft_gs = new DataFetcher(
                "https://battlefieldtracker.com/bf1/api/Stats/DetailedStats?platform=3&displayName=" + username,
                "TRN-Api-Key", "152de4e7-22a4-4a0a-b2b6-2bea2c7886b6");
        std::cout << "General Stats have been downloaded for " << username << '\n';
        gen_stats = json::parse(dft_gs->GetSource());
        dft_gs = nullptr;
        std::cout << "------------------------------------------------------------------------" << '\n';
        dft_ws = new DataFetcher(
                "https://battlefieldtracker.com/bf1/api/Progression/GetWeapons?platform=3&displayName=" + username +
                "&game=tunguska", "TRN-Api-Key", "152de4e7-22a4-4a0a-b2b6-2bea2c7886b6");
        std::cout << "Weapon Stats have been downloaded for " << username << '\n';
        wpon_stats = json::parse(dft_ws->GetSource());
        dft_ws = nullptr;
        std::cout << "------------------------------------------------------------------------" << '\n';
        dft_vs = new DataFetcher(
                "https://battlefieldtracker.com/bf1/api/Progression/GetVehicles?platform=3&displayName=" +
                username + "&game=tunguska", "TRN-Api-Key", "152de4e7-22a4-4a0a-b2b6-2bea2c7886b6");
        std::cout << "Vehicle Stats have been downloaded for " << username << '\n';
        vhcl_stats = json::parse(dft_vs->GetSource());
        dft_vs = nullptr;
        std::cout << "------------------------------------------------------------------------" << '\n';
    }

    BF1Stats::~BF1Stats() {
        delete dft_gs;
        delete dft_ws;
        delete dft_vs;
    }

    Player::~Player() {
        delete sts;
        delete dfetcher;
    }

    Player::Player(std::string _username) : username(_username) {
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
        std::string st;
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

    void Player::Parser() {
        std::string data;
        double numb;
        bool flag;
        std::ifstream fin("BF1GenStats.txt");
        while (!fin.eof()) {
            getline(fin, data);
            label_stats.push_back(data);
        }
        fin.close();;
        dfetcher = new DataFetcher("https://battlefieldtracker.com/bf1/profile/pc/" + username);
        std::cout << "Additional General Stats have been downloaded for " << username << '\n';
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
        std::cout << "------------------------------------------------------------------------" << '\n';
    }

    bool Player::statsfinder(std::string _data) {
        for (std::string &s : label_stats) {
            if (_data == s) {
                return true;
            }
        }
        return false;
    }

    double Player::statreader(std::string _data) {
        double number;
        std::istringstream sstr(_data);
        if (!(sstr >> number)) {
            number = -1;
        }
        return number;
    }

    std::ostream &operator<<(std::ostream &os, Player *plr_ptr) {
        std::string spacer = " | ";
        std::string linebreaker = "-----------------------------------------------------------------------------------------------------------------------------------";
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
            std::cout << "An error has occurred!" << '\n';
            std::cerr << e.what() << '\n';
            return os;
        }
    }

    BF1Tracker::BF1Tracker(std::string _username) : Player(_username) {
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

    void BF1Tracker::ReadSavedData() {
        std::string processed_username;
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
                std::cout << "This player's stats have been found in UNSKO Online Database" << '\n';
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
                std::cout << "===========================================================================" << '\n';
            } else {
                std::cout << "This player has not been found in our Online Database" << '\n';
                std::cout << "===========================================================================" << '\n';
                flag = false;
            }
        } else {
            std::cout << "Something went wrong when trying to establish connection to WEB API. HTTP Status CODE: "
                      << r.status_code << '\n';
            flag = false;
        }
    }

    void BF1Tracker::savedata() {
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

    void BF1Tracker::Variation() {
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

    void BF1Tracker::PerfIndexBF1(bool outputflag) {
        if (flag == true) {
            std::cout << "Calculating Performance Index.../ " << '\n';
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
                std::cout << "========================================================================" << '\n';
                std::cout << "The Performance Index has been calculated. Choose an option to see the results."
                          << '\n';
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
                        std::cout << "========================================================================"
                                  << '\n';
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
                        std::cout << "========================================================================"
                                  << '\n';
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
                        std::cout << "DAccuracy: " << dacc << '\n';
                        std::cout << "DDogtagsTaken: " << ddgtgs << '\n';
                        std::cout << "DPTFO: " << dptfo << '\n';
                        std::cout << "End of Report" << '\n';
                        std::cout << "========================================================================"
                                  << '\n';
                        break;
                    }
                }
            }
        }
    }

    json BF1Tracker::exporter() {
        // time stamp
        time_t now = time(0);
        std::string time = ctime(&now);
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

    int BF1Tracker::GetRP() { return drp; }

    double BF1Tracker::GetPIndex() { return perfindex; }
}
