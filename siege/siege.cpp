//
// Created by Eric Dattore on 8/25/18.
//
#include "siege.hpp"
#include "utilities.hpp"

namespace Siege {

    SiegeGenStats::SiegeGenStats(std::string username) {
        try {
            std::cout << "------------------------------------------------------------------------" << '\n';
            fet = new DataFetcher("https://r6.tracker.network/profile/pc/" + username);
            std::cout << "Downloading General Stats for player: " << username << '\n';
            fet->SalvaDisco("siege");
            std::cout << "------------------------------------------------------------------------" << '\n';
            std::string aux;
            std::ifstream fin("SiegeStats.txt");
            while (!fin.eof()) {
                getline(fin, aux);
                stats.push_back(aux);
            }
            fin.close();
            aux.clear();
            DiskKernelParser();
            if (stats.size() != vstats.size()) {
                std::cout << stats.size() << " | " << vstats.size() << '\n';
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
            std::cout << "------------------------------------------------------------------------" << '\n';
            stats.clear();
            vstats.clear();
            std::string aux;
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

    void SiegeGenStats::DiskKernelParser() {
        std::string data;
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

    double SiegeGenStats::statreader(std::string _data) {
        double number;
        std::istringstream sstr(_data);
        if (!(sstr >> number)) {
            number = -1;
        }
        return number;
    }

    bool SiegeGenStats::statsfinder(std::string data) {
        int pos;
        for (int i = 0; i < stats.size(); i++) {
            pos = data.find(stats[i]);
            if (pos != -1) { return true; }
        }
        return false;
    }

    void SiegeGenStats::Mostra() {
        for (int i = 0; i < vstats.size(); i++) {
            std::cout << stats[i] + ": " << vstats[i] << '\n';
        }
    }

    std::vector<float> SiegeGenStats::getvstats() { return vstats; }

    std::vector<std::string> SiegeGenStats::getstats() { return stats; }


    SiegeOperators::~SiegeOperators() { delete fetop; }

    SiegeOperators::SiegeOperators(std::string _username) {
        fetop = new DataFetcher("https://r6stats.com/stats/uplay/" + _username + "/operators");
        std::cout << "Downloading Operators Data for player: " << _username << '\n';
        fetop->SalvaDisco("siegeop");
        std::cout << "------------------------------------------------------------------------" << '\n';
        //Header of Variables
        int n1, n2, nb;
        int numb;
        std::vector<int> voper;
        std::map<int, int> operatorlines; //maps operatorId -> number of stats to read
        std::string aux;
        std::string aux2;

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

    double SiegeOperators::statreader(std::string _data) {
        double number;
        std::istringstream sstr(_data);
        if (!(sstr >> number)) {
            number = -1;
        }
        return number;
    }

    int SiegeOperators::findop(std::string data) {
        int pos;
        for (int k = 0; k < opfinder.size(); k++) {
            pos = data.find(opfinder[k]);
            if (pos != -1 && pos != 0) {
                return k;
            }
        }
        return -1;
    }

    double SiegeOperators::secondaryparser(std::string data) {
        int pos;
        std::string sdata;
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
    std::ostream &operator<<(std::ostream &os, SiegeOperators &obj) {
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

    std::vector<std::vector<int>> SiegeOperators::GOper() { return oper; }

    Player::~Player() {
        delete sp;
        delete sop;
    };

    Player::Player(std::string _username) : username(_username) {
        std::vector<float> values;
        std::vector<std::string> labels;
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

    void Player::additionalstats() {
        double aux;
        double aux2;
        double aux3;
        std::string rmp = "Ranked Matches Played";
        std::string mp = "Matches Played";
        std::string rwlr = "Ranked Win/Loss Ratio";
        std::string wlr = "Win/Loss Ratio";
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
        std::string strdata;
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
                    aux = utilities::stat_reader(strdata);
                    if (aux != -1) { statistics.insert(std::make_pair("LevelUpScore", aux)); }
                }
                if (levelpos != -1) {
                    getline(fin, strdata);
                    strdata = strdata.substr(19, strdata.size());
                    aux = utilities::stat_reader(strdata);
                    if (aux != -1) { statistics.insert(std::make_pair("Level", aux)); }
                }
            }
        }
    }

    void Player::makemap(std::vector<float> vstats, std::vector<std::string> stats) {
        for (int i = 0; i < stats.size(); i++) {
            statistics.insert(make_pair(stats[i], vstats[i]));
        }
    }

    std::ostream &operator<<(std::ostream &os, Player &plr) {
            os << plr.username << '\n';
            for (auto &element : plr.statistics) {
                os << element.first << ": " << element.second << '\n';
            }
            os << "End of Stats" << '\n';
            return os;
        }

    SiegeTracker::SiegeTracker(std::string _username) : Player(_username) {
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

    void SiegeTracker::ReadSavedData() {
        std::string processed_username;
        processed_username = utilities::username_stripper(username);

        auto r = cpr::Get(cpr::Url{
                                  "http://unkso.michaeljamesbondparsons.com/stats/r6siege/latest?players[0]=" + processed_username},
                          cpr::Header{{"content-type",  "application/json"},
                                      {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
        if (r.status_code == 200) {
            if (r.text != "[]") {
                json statpack;
                statpack = json::parse(r.text);
                std::cout << "This player's stats have been found in UNKSO Online Database" << '\n';
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
                std::cout << "This player has not been found in our Online Database" << '\n';
                trackerflag = false;
            }
        } else {
            std::cout << "Something went wrong when trying to establish connection to WEB API. HTTP Status CODE: "
                 << r.status_code << '\n';
            trackerflag = false;
        }
    }

    void SiegeTracker::savedata() {
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
    void SiegeTracker::Variation() {
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

    void SiegeTracker::PerfIndexSiege(bool outputflag) {
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
                        std::cout << "Score Index: " << indexscore << '\n';
                        std::cout << "Team Stats Index: " << indexts << '\n';
                        std::cout << "Combat Stats Index: " << indexcts << '\n';
                        std::cout << "Activity Index (Rounds Played): " << indexrp << '\n';
                        std::cout << "Wins Index: " << indexwns << '\n';
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
                        std::cout << "Score Index: " << indexscore << '\n';
                        std::cout << "Team Stats Index: " << indexts << '\n';
                        std::cout << "Combat Stats Index: " << indexcts << '\n';
                        std::cout << "Activity Index (Rounds Played): " << indexrp << '\n';
                        std::cout << "Wins Index: " << indexwns << '\n';
                        std::cout << "-----------------------------------" << '\n';
                        std::cout << "Delta Factor Components" << '\n';
                        std::cout << "DScore: " << dscore << '\n';
                        std::cout << "DRoundsPlayed: " << drounds_played << '\n';
                        std::cout << "DGadgetsDestroyed: " << dgadgets_destroyed << '\n';
                        std::cout << "DReinforcements: " << dreinforcements << '\n';
                        std::cout << "DBarricades: " << dbarricades << '\n';
                        std::cout << "DRevives: " << drevives << '\n';
                        std::cout << "DKillAssists: " << dkillassists << '\n';
                        std::cout << "DKills: " << dkills << '\n';
                        std::cout << "DHeadshots: " << dheadshots << '\n';
                        std::cout << "DMelee: " << dmelee_kills << '\n';
                        std::cout << "DDBNO (Down but not out): " << ddbno << '\n';
                        std::cout << "DPenetrationKills: " << dpenetration_kills << '\n';
                        std::cout << "End of Report: " << '\n';
                        std::cout << "========================================================================" << '\n';
                        break;
                    }
                }
            }
        }
    }

    json SiegeTracker::exporter() {
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

    int SiegeTracker::GetRP() { return drounds_played; }

    double SiegeTracker::GetPIndex() { return perfindex; }
}

namespace RaptorR6 {
    Player::Player(std::string usr, std::vector<double> ps) : username(usr), mp(ps[2]), rd(ps[5]), bd(ps[6]), gd(ps[9]),
                                                 rv(ps[10]), ka(ps[4]), kl(ps[0]), ds(ps[1]), hs(ps[3]), mk(ps[8]),
                                                 pk(ps[7]), rank(ps[11]), mmr(ps[12]), region(ps[13]) {}

    //obj1 = newest; obj2 = oldest
    Player operator-(Player &obj1, Player &obj2) {
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

    std::ofstream &operator<<(std::ofstream &fout, const Player &p) {
            fout << p.username << ',' << p.mp << ',' << p.rd << ',' << p.bd << ',' << p.gd << ',' << p.rv << ',' << p.ka
                 << ',' << p.kl << ',' << p.ds << ',' << p.hs << ',' << p.mk << ',' << p.pk << ',' << p.rank << ','
                 << p.mmr << ',' << p.region << '\n';
            return fout;
        }

    typedef std::vector<Player> rPack;

    namespace ar6 {
        // Transforms Sourcecode string to Sourcecode Vector
        std::vector<std::string> SourceCode2StringVector(std::string sourcecode) {
            std::vector<std::string> tx_sourcecode;
            std::string line;
            std::istringstream stream(sourcecode);
            while (std::getline(stream, line)) {
                tx_sourcecode.push_back(line);
            }
            return tx_sourcecode;
        }

        // Returns vector of Sourcecodes
        std::vector<std::vector<std::string>> player_fetcher(const std::vector<std::string> &n_users) {
            int j = 0;
            std::vector<cpr::Url> urls;
            std::string auxurl;
            for (int i = 0; i < n_users.size(); i++) {
                auxurl = "https://r6.tracker.network/profile/pc/" + n_users[i];
                urls.push_back(auxurl);
            }
            std::cout << "Downloading Parsing Data" << '\n';
            auto container = std::vector<std::future<cpr::Response>>{};
            for (int i = 0; i < n_users.size(); ++i) {
                try {
                    container.emplace_back(cpr::GetAsync(urls[i], cpr::VerifySsl{false}));
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    std::cout << i << " players' additional stats have been downloaded" << '\n';
                }
                catch (std::exception &e) { std::cerr << e.what() << '\n'; }
            }
            std::cout << "Wait a few more seconds while the API refreshes and all requests are processed." << '\n';
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::cout << "All requests have been downloaded" << '\n';
            std::vector<std::vector<std::string>> sync_pack;
            for (auto &response : container) {
                auto r = response.get();
                sync_pack.push_back(SourceCode2StringVector(r.text));
            }
            return sync_pack;
        }

        // Transforms Sourcecode into Vector of Stats
        std::vector<double> Processor(std::vector<std::string> source) {
            std::vector<double> stats;
            std::vector<std::string> st(11);
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

            std::string aux;
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
                    mmr = utilities::stat_reader(source[i + 4].substr(44));
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
        bool loggerstatfinder(std::string dataline) {
            std::string line_to_find1 = "<div class=\"player\">";
            int pos;
            pos = dataline.find(line_to_find1);
            if (pos != -1) { return 1; }
            return 0;
        }

        // Auxiliary functions that finds players in Sourcecode (Leaderboard Page)
        std::vector<std::string> Log_Processor(std::vector<std::string> t_sourcecode, std::vector<std::string> dbusernames) {
            bool flag;
            int pos_x;
            int pos_xdx;
            std::string single_user;
            std::vector<std::string>::iterator unq_plr;
            std::vector<std::string> log_users;
            for (int i = 0; i < t_sourcecode.size(); i++) {
                flag = loggerstatfinder(t_sourcecode[i]);
                if (flag == 1) {
                    pos_x = t_sourcecode[i + 1].find('>');
                    pos_xdx = t_sourcecode[i + 1].rfind('<');
                    if (pos_x != -1 && pos_xdx != -1) {
                        single_user = t_sourcecode[i + 1].substr(pos_x + 1, pos_xdx - pos_x - 1);
                        unq_plr = std::find(dbusernames.begin(), dbusernames.end(), single_user);
                        if ((unq_plr - dbusernames.begin()) < (dbusernames.end() - dbusernames.begin())) {
                            std::cout << "A non-unique player has been found : [ID]= " << unq_plr - dbusernames.begin()
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
        std::vector<std::string> name_fetcher(rPack db) {
            std::vector<std::string> db_users;
            for (auto &stats : db) {
                db_users.push_back(stats.username);
            }
            return db_users;
        }

        void Exporter(rPack p) {
            std::string rdb;
            std::cout << "Type the name of the Coeus Database: ";
            getline(std::cin, rdb);
            std::cout << '\n';
            std::ofstream fout("R" + rdb + ".csv");
            if (fout.good()) {
                fout << "UNKSO - Coeus Project" << '\n';
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
                std::cout << "Error while Exporting Data" << '\n';
                fout.close();
                Exporter(p);
            }
        }

        rPack Importer() {
            std::string filename;
            std::string str;
            std::vector<double> stats(14, 0);
            std::string user;
            rPack log;
            std::cout << "Type the name of the file you want to read: ";
            getline(std::cin, filename);
            std::cout << '\n';
            std::ifstream fin(filename);
            if (fin.good()) {
                for (int i = 0; i < 5; i++) { fin.ignore(5000, '\n'); }
                while (!fin.eof()) {
                    std::cout << "PLAYER" << '\n';
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
                    std::cout << "END OF PLAYER" << '\n';
                    Player p(user, stats);
                    log.emplace_back(p);
                }
                fin.close();
            } else { std::cout << "Error" << "\n"; }
            return log;
        }
    }

    namespace Routines {
        rPack Logger(int max_players) {
            auto container = std::vector<std::future<cpr::Response>>{};
            std::vector<std::string> users;
            std::vector<std::string> dbusers = ar6::name_fetcher(ar6::Importer()); //PROBLEM
            rPack log_plrs;
            std::string baseurl = "https://r6.tracker.network/leaderboards/pvp/pc/PVPWLRatio?page=";

            int j;
            std::cout << "Where do you wish to start? (Page number)" << '\n';
            std::cin >> j;
            std::cin.get();

            std::vector<std::string> aux_users;
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
                std::cout << "Number of users logged by the Coeus Tool: " << users.size() << '\n';
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
                std::vector<std::string> plrs = ar6::name_fetcher(ar6::Importer());
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
                std::string filename;
                std::cout << "==========================================================================" << '\n';
                std::cout << "Do you wish to create a Coeus List for players of which region?" << '\n';
                std::cout << "1 - Americas" << '\n';
                std::cout << "2 - Europe" << '\n';
                std::cout << "3 - Asia" << '\n';
                std::cout << "4 - Exit Assembler Module" << '\n';
                std::cout << "==========================================================================" << '\n';
                std::cin >> op;
                std::cin.get();
                if (op != 4) {
                    std::cout << "==========================================================================" << '\n';
                    std::cout << "Type a filename for the Exported Coeus List" << '\n';
                    std::cout << "Input: ";
                    getline(std::cin, filename);
                    std::cout << "==========================================================================" << '\n';
                    std::ofstream fout(filename + ".csv");

                    for (auto &p : fp) {
                        if (p.region == op && p.a_index > 20 && p.t_index > 0 && p.c_index > 0) {
                            fout << p.username << ',' << p.a_index << ',' << p.t_index << ',' << p.c_index << ','
                                 << p.rank << ',' << p.mmr << '\n';
                        }
                    }

                    fout.close();
                    std::cout << "Do you wish to update UNKSO's DB? (Y/N)" << '\n';
                    std::string s;
                    getline(std::cin, s);
                    if (s == "Y" || s == "y") { ar6::Exporter(fp); }
                }
            }
            catch (std::exception &e) { std::cerr << e.what() << "Assembler()" << '\n'; }
        }
    }

    void Header() {
        int op;
        std::cout << "Welcome to the Coeus Tool! Read the instructions and choose an option to begin working." << '\n';
        std::cout
                << "========================================================================================================================"
                << '\n';
        std::cout << "Instructions of Use: " << '\n';
        std::cout
                << "The Logger module is responsible for searching the Rainbow Six Leaderboards for players to be classified. The Coeus List will not be generated in this step."
                << "The Sync module is responsible for comparing the stats of the players in our database with the stats of an initial time and the most recents stats from the R6 API."
                << "After the update. This module will reunite those who meet the adequeate criteria for being recruited into the =US= and export those players into a .csv file which is what is called the 'Coeus List'.\n";
        std::cout
                << "========================================================================================================================"
                << '\n';
        std::cout << "1 - Find and Log more players (Logger)" << '\n';
        std::cout << "2 - Update Database and Generate Coeus List (Sync)" << '\n';
        std::cout << "3 - Exit" << '\n';
        std::cout << "Input: ";
        std::cin >> op;
        std::cin.get();
        std::cout
                << "========================================================================================================================"
                << '\n';
        if (op == 1) {
            Routines::Logger();
        } else if (op == 2) {
            Routines::Assembler(Routines::Algorithm(Routines::Filter()));
        } else {
            std::cout << "You have not selected a valid option." << '\n';
            std::cout
                    << "====================================================================================================================================================================================================="
                    << '\n';
        }
    }
}
