//
// Created by Eric Dattore on 8/25/18.
//
#include "coeus.hpp"
#include "../headers/utilities.hpp"

namespace Coeus {
    User::User() : username(""), repairs(0), orders_comp(0), bombsdefused(0), roundsplayed(0), heals(0),
                   flagcaptured(0),
                   gamemodescore(0), timeplayed(0),
                   flagdefended(0), geographic_region(0), revives(0), squadscore(0), bombsplaced(0) {}

    User::User(std::string a, int b, int c, int d, int e, int f, int g, float h, int i, int j, int k, int l, int m,
               int n)
            : username(a), repairs(b), orders_comp(c), bombsdefused(d), roundsplayed(e), heals(f), flagcaptured(g),
              gamemodescore(h), timeplayed(i),
              flagdefended(j), geographic_region(k), revives(l), squadscore(m), bombsplaced(n) {}

    Vision::~Vision() { delete logdata; }

    Vision::Vision() : search_for_more(true), numbplrs(0) {
        int i, point, maxpoint;
        std::cout
                << "========================================================================================================================"
                << '\n';
        std::cout << "UNKSO RAPTOR" << '\n';
        std::cout
                << "========================================================================================================================"
                << '\n';
        std::cout << "Instructions of Use: " << '\n';
        std::cout
                << "You will filter players by their rank. Initial studies have shown that the rank margin of 100 - 120 will contain the best recruits for our clan.";
        std::cout
                << "The number of interactions is the number of times that the Coeus is going to go over a pack of 100 players in the database while making sure they meet certain criteria.";
        std::cout
                << "The starting point is the position in the database where you think those players in the rank margin are located. A good example would be values between 300-500.";
        std::cout
                << "If this phase takes long, it is because the database is slow. Still, this can take a few to several minutes depending on your search parameters.\n";
        std::cout
                << "========================================================================================================================"
                << '\n';
        std::cout << "What is the maximum rank for the search?" << '\n';
        std::cin >> maxrank;
        std::cin.get();
        std::cout << "What is the minimum rank for the search?" << '\n';
        std::cin >> minrank;
        std::cin.get();
        std::cout << "What is the maximum number of interactions for the search?" << '\n';
        std::cin >> numb;
        std::cin.get();
        std::cout << "Do you have a starting point (i.e. 50) for where you wish to start the search?" << '\n';
        std::cin >> point;
        std::cin.get();
        std::cout
                << "========================================================================================================================"
                << '\n';
        i = 0;
        std::string url;
        std::string logurl = "http://unkso.michaeljamesbondparsons.com/stats/bf1/latest";
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
            std::cout << "The Database has been downloaded with " << dbusernames.size() << " players in it." << '\n';
            std::cout
                    << "========================================================================================================================================================================="
                    << '\n';
        } // finds db players
        while (search_for_more) {
            url = "https://battlefieldtracker.com/bf1/leaderboards/pc/Rank?page=" + std::to_string(point);
            logdata = new DataFetcher(url);
            t_sourcecode = SourceCode2StringVector(logdata->GetSource());
            t_sourcecode_processor();
            t_sourcecode.clear();
            std::cout << list.size() << " players have been found and logged!" << '\n';
            std::cout
                    << "======================================================================================================================================================================="
                    << '\n';
            i++;
            point++;
            if (i >= numb) { break; }
        }
    }

    std::vector<std::string> Vision::getusers() {
        std::vector<std::string> exp_users;
        for (auto const &element : list) {
            exp_users.push_back(element.first);
        }
        return exp_users;
    }

    int Vision::loggerstatfinder(std::string dataline) {
        std::string line_to_find1 = "<div class=\"player\">";
        std::string line_to_find2 = "<div class=\"pull-right\">";
        int pos;
        pos = dataline.find(line_to_find1);
        if (pos != -1) { return 1; }
        pos = dataline.find(line_to_find2);
        if (pos != -1) { return 2; }
        return 0;
    }

    std::vector<std::string> Vision::SourceCode2StringVector(std::string sourcecode) {
        std::vector<std::string> tx_sourcecode;
        std::string line;
        std::istringstream stream(sourcecode);
        while (std::getline(stream, line)) {
            tx_sourcecode.push_back(line);
        }
        return tx_sourcecode;
    }

    void Vision::t_sourcecode_processor() {
        int flag;
        int pos_x;
        int pos_xdx;
        std::string single_user;
        std::vector<std::string>::iterator unq_plr;
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
                    }
                }
            } else if (flag == 2) {
                rank = utilities::stat_reader(t_sourcecode[i + 1]);
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

    std::ostream &operator<<(std::ostream &os, Vision &obj) {
        os << "Showing all players and their ranks: " << '\n';
        for (auto &element : obj.list) {
            os << element.first << ": " << element.second << '\n';
        }
        os << "Total of players: " << obj.list.size() << '\n';
        return os;
    }

    Engine::~Engine() { delete eye; }

    Engine::Engine(bool update) : dftx(nullptr) {
        bool status = true;
        if (update) {
            std::cout << "Synchronizing and uploading database players. This will take several minutes." << '\n';
            std::cout
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
                std::cout << "The database is empty. The program will be terminated." << '\n';
                exit(0);
            }
        } else {
            eye = new Vision();
            network_module(eye->getusers());
            parser_module(usernames);
            region_filter(usernames);
            Debug("1");
            matrixequalizer();
            Debug("2");
            std::cout << "Do you wish to sync data with the UNKSO Database?" << '\n';
            std::string op;
            getline(std::cin, op);
            if (op == "y" || op == "Y") { server_emodule(); }
        }
    }

    //Engine
    void Engine::network_module(std::vector<std::string> n_users) {
        std::vector<cpr::Url> urls;
        std::string auxurl;
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
                std::cout << i << " players' stats have been downloaded" << '\n';
            }
            catch (std::exception &e) { std::cerr << e.what() << '\n'; }
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "All requests have been downloaded" << '\n';

        int i = 0;
        json pack;
        int bad_requests = 0;
        std::vector<int> gbcollec;
        for (auto &fr : container) {
            std::cout << i - bad_requests << " players have successfully been processed." << '\n';
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
        std::cout << "Number of bad requests: " << bad_requests << '\n';
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

    void Engine::parser_module(std::vector<std::string> n_users) {
        int j = 0;
        std::vector<cpr::Url> urls;
        std::string auxurl;
        json pm_pack;
        for (int i = 0; i < n_users.size(); i++) {
            auxurl = "https://battlefieldtracker.com/bf1/profile/pc/" + n_users[i];
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
        for (auto &fr : container) {
            std::cout << j << " players have been parsed." << '\n';
            auto r = fr.get();
            if (r.status_code == 200) {
                try {
                    std::vector<int> addt_st = addt_stats(r.text);
                    if (addt_st.size() != 0) {
                        bombsplaced.push_back(addt_st[0]);
                        bombsdefused.push_back(addt_st[1]);
                        resupplies.push_back(addt_st[2]);
                        orders_comp.push_back(addt_st[3]);
                    } else {
                        std::cout << "Unable to parse this player" << '\n';
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
                std::cout << "Something bad has happened!" << '\n';
                bombsplaced.push_back(-1);
                bombsdefused.push_back(-1);
                resupplies.push_back(-1);
                orders_comp.push_back(-1);
            }
        }
    }

    void Engine::region_filter(std::vector<std::string> n_users) {
        std::vector<cpr::Url> urls;
        std::string auxurl;
        json pm_pack;
        for (int i = 0; i < n_users.size(); i++) {
            auxurl = "https://battlefieldtracker.com/bf1/profile/pc/" + usernames[i] + "/matches";
            urls.push_back(auxurl);
        }
        auto container = std::vector<std::future<cpr::Response>>{};
        std::cout << "Gathering geographic data pattern. This may take a while." << '\n';
        for (int i = 0; i < n_users.size(); ++i) {
            try {
                container.emplace_back(cpr::GetAsync(urls[i], cpr::VerifySsl{false}));
                std::cout << i << " players' additional stats have been downloaded" << '\n';
                if (i % 50 == 0) { std::this_thread::sleep_for(std::chrono::milliseconds(2000)); }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            catch (std::exception &e) { std::cerr << e.what() << '\n'; }
        }
        std::cout << "Wait a few more seconds while the API refreshes and all requests are finished." << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "Geographic data pattern downloaded for all players." << '\n';
        int j = 0;
        for (auto &fr : container) {
            std::cout << j << " players have been processed." << '\n';
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
                std::cout << "Assigning Unknown Region - Broken Data" << '\n';
                geographic_region.push_back(-1);
            }
        }
        if (geographic_region.size() != n_users.size()) {
            std::cout << "Fatal Error has happened when fetching geo data" << '\n';
            std::cout << geographic_region.size() << " : " << n_users.size() << '\n';
        }
    }

    void Engine::server_emodule() {
        int n_packs = usernames.size() / 500;
        int remainder = usernames.size() - (500 * n_packs);

        for (int pack = 0; pack < n_packs; pack++) {
            json data = exporter(500, 500 * pack);
            std::cout << "Sending Coeus Data Pack#" << pack << " to UNKSO Server" << '\n';
            auto r = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                               cpr::Body{data.dump()},
                               cpr::Header{{"content-type",  "application/json"},
                                           {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
            if (r.status_code == 200) { std::cout << "Data has been exported successfully!" << '\n'; }
            else {
                std::cout << "Something went wrong when exporting the data. Status Code: " << r.status_code << '\n';
                while (true) {
                    std::cout << "Attempt to upload again? Type either Y for Yes or N for No." << '\n';
                    std::string ans;
                    getline(std::cin, ans);
                    if (ans == "Y" || ans == "y") {
                        auto rx = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                                            cpr::Body{data.dump()},
                                            cpr::Header{{"content-type",  "application/json"},
                                                        {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
                        if (rx.status_code == 200) {
                            std::cout << "Data has been exported successfully!" << '\n';
                            break;
                        } else
                            std::cout << "Something went wrong when exporting the data. Status Code: " << r.status_code
                                      << '\n';
                    } else if (ans == "N" || ans == "n") {
                        std::cout
                                << "You won't be uploading again. Please report this issue to UNKSO Engineering Department."
                                << '\n';
                        break;
                    }
                }
            }
        }
        json data = exporter(remainder, 500 * n_packs);
        std::cout << "Sending Coeus Data Remainder Pack to UNKSO Server" << '\n';
        auto r = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                           cpr::Body{data.dump()},
                           cpr::Header{{"content-type",  "application/json"},
                                       {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
        if (r.status_code == 200) { std::cout << "Data has been exported successfully!" << '\n'; }
        else {
            while (true) {
                std::cout << "Attempt to upload again? Type either Y for Yes or N for No." << '\n';
                std::string ans;
                getline(std::cin, ans);
                if (ans == "Y" || ans == "y") {
                    auto rx = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                                        cpr::Body{data.dump()},
                                        cpr::Header{{"content-type",  "application/json"},
                                                    {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
                    if (rx.status_code == 200) {
                        std::cout << "Data has been exported successfully!" << '\n';
                        break;
                    } else
                        std::cout << "Something went wrong when exporting the data. Status Code: " << r.status_code
                                  << '\n';
                } else if (ans == "N" || ans == "n") {
                    std::cout
                            << "You won't be uploading again. Please report this issue to UNKSO Engineering Department."
                            << '\n';
                    break;
                }
            }
        }
    }

    bool Engine::server_imodule() {
        std::cout << "Type in the database directory you're tracking your stats from: ";
        getline(std::cin, db_partition);
        std::cout << '\n';
        std::string url = "http://unkso.michaeljamesbondparsons.com/stats/bf1/latest";//?events[0]=" + db_partition;
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
            std::cout << "There were " << rusernames.size() << " valid players in the database." << '\n';
            return true;
        } else { return false; }
    }

    void Engine::matrixequalizer() {
        std::cout << "This is the current Statistical Matrix" << '\n';
        std::cout << usernames.size() << " | " << rusernames.size() << '\n' <<
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
        std::cout << "Player Matrix has been normalized" << '\n';
        std::cout << usernames.size() << " | " << rusernames.size() << '\n' <<
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

    void Engine::Variation() {
        try {
            for (int t = 0; t < usernames.size(); t++) {
                if (usernames[t] != rusernames[t]) { std::cout << "BAD MATRIX t= " << t << '\n'; }
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

    void Engine::Debug(std::string str) {
        try {
            time_t now = time(0);
            char time[100];
            struct tm *timeinfo = localtime(&now);
            strftime(time, 100, "%d%b%y", timeinfo);
            std::string time_file = time;
            std::cout << "=================================================================================" << '\n';
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
            std::cout << "=================================================================================" << '\n';
        }
        catch (std::runtime_error &e) {
            std::cerr << e.what() << "\n";
        }
    }

    //class utilities
    bool Engine::statfinder(std::string data) {
        int pos = 0;
        for (int k = 0; k < finder.size(); k++) {
            pos = data.find(finder[k]);
            if (pos != -1) { return true; }
        }
        return false;
    }

    std::vector<int> Engine::addt_stats(std::string &sourcecode) {
        /*
        addt_st[0] = Bombs Placed
        addt_st[1] = Bombs Defused
        addt_st[2] = Resupplies
        addt_st[3] = Orders Completed
        */
        bool flag;
        std::vector<int> addt_st;
        middlemanstats = eye->SourceCode2StringVector(sourcecode);
        for (int j = 0; j < middlemanstats.size(); j++) {
            flag = statfinder(middlemanstats[j]);
            if (flag == true) {
                addt_st.push_back(utilities::stat_reader(middlemanstats[j + 1]));
            }
        }
        return addt_st;
    }

    int Engine::region_filter(std::string rawgeodata) {
        std::vector<std::string> searcher = {
                "- US E. -", "- US W. -", "- EU -", "- JP -", "- HK -", "- OC -", "- S. Am. -", "Africa", "Europe",
                "US East", "US West", "Oceania", "Japan"
        };
        std::vector<std::string> geo_data = eye->SourceCode2StringVector(rawgeodata);
        std::vector<int> region;
        for (auto &line : geo_data) {
            region.push_back(regionfinder(line, searcher));
        }
        return statistical_mode(region);
    }

    int Engine::regionfinder(std::string data, std::vector<std::string> &searcher) {
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

    int Engine::statistical_mode(const std::vector<int> &v1) {
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

    void Engine::csv_exporter() {
        std::string filename;
        std::cout << "Give a name for the CSV file to be exported (i.e. file.csv): ";
        getline(std::cin, filename);
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

    json Engine::exporter(int packsize, int starting_point) {
        try {
            // time stamp
            time_t now = time(0);
            std::string time = ctime(&now);

            json export_package;
            std::string s;
            std::cout << "Type in the database directory you will track your stats from: ";
            getline(std::cin, s);
            std::cout << '\n';
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

    Synchronizer::Synchronizer(bool update) : Engine(update) {
        for (int plr = 0; plr < usernames.size(); plr++) {
            teamwork_index(plr);
            ptfo_index(plr);
            activity_index(plr);
            std::cout << "Indexing successful for plr: " << plr << '\n';
        }
        csv_exporter();
        std::cout << "Do you wish to sync data with the UNKSO Database? SYNC CLASS" << '\n';
        std::string op;
        getline(std::cin, op);
        if (op == "y" || op == "Y") { server_emodule(); }
    }

    void Synchronizer::teamwork_index(int player) {
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

    void Synchronizer::ptfo_index(int player) {
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

    void Synchronizer::activity_index(int player) {
        float numb = pow(dtimeplayed[player], 0.333) + droundsplayed[player];
        if (numb >= 0) {
            vactivity_index.push_back(numb);
        } else vactivity_index.push_back(0.0);
    }

    void Synchronizer::server_emodule() {
        int n_packs = rusernames.size() / 500;
        int remainder = rusernames.size() - (500 * n_packs);

        for (int pack = 0; pack < n_packs; pack++) {
            json data = exporter(500, 500 * pack);
            std::cout << "Sending Coeus Data Pack#" << pack << " to UNKSO Server" << '\n';
            auto r = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                               cpr::Body{data.dump()},
                               cpr::Header{{"content-type",  "application/json"},
                                           {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
            if (r.status_code == 200) { std::cout << "Data has been exported successfully!" << '\n'; }
            else {
                std::cout << "Something went wrong when exporting the data. Status Code: " << r.status_code << '\n';
                std::cout << r.text << '\n';
                while (true) {
                    std::cout << "Attempt to upload again? Type either Y for Yes or N for No." << '\n';
                    std::string ans;
                    getline(std::cin, ans);
                    if (ans == "Y" || ans == "y") {
                        auto rx = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                                            cpr::Body{data.dump()},
                                            cpr::Header{{"content-type",  "application/json"},
                                                        {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
                        if (rx.status_code == 200) {
                            std::cout << "Data has been exported successfully!" << '\n';
                            break;
                        } else
                            std::cout << "Something went wrong when exporting the data. Status Code: " << r.status_code
                                      << '\n';
                    } else if (ans == "N" || ans == "n") {
                        std::cout
                                << "You won't be uploading again. Please report this issue to UNKSO Engineering Department."
                                << '\n';
                        break;
                    }
                }
            }
            data.clear();
        }

        json data = exporter(remainder, 500 * n_packs);
        std::cout << "Sending Coeus Data#" << n_packs + 1 << " to UNKSO Server" << '\n';
        auto r = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                           cpr::Body{data.dump()},
                           cpr::Header{{"content-type",  "application/json"},
                                       {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
        if (r.status_code == 200) { std::cout << "Data has been exported successfully!" << '\n'; }
        else {
            while (true) {
                std::cout << "Attempt to upload again? Type either Y for Yes or N for No." << '\n';
                std::string ans;
                getline(std::cin, ans);
                if (ans == "Y" || ans == "y") {
                    auto rx = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                                        cpr::Body{data.dump()},
                                        cpr::Header{{"content-type",  "application/json"},
                                                    {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
                    if (rx.status_code == 200) {
                        std::cout << "Data has been exported successfully!" << '\n';
                        break;
                    } else
                        std::cout << "Something went wrong when exporting the data. Status Code: " << r.status_code
                                  << '\n';
                } else if (ans == "N" || ans == "n") {
                    std::cout
                            << "You won't be uploading again. Please report this issue to UNKSO Engineering Department."
                            << '\n';
                    break;
                }
            }
        }
    }

    void Synchronizer::csv_exporter() {
        time_t now = time(0);
        char time[100];
        struct tm *timeinfo = localtime(&now);
        strftime(time, 100, "%d%b%y", timeinfo);
        std::string time_file = time;
        std::cout << "=================================================================================" << '\n';
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
        std::cout << "=================================================================================" << '\n';
    }

    json Synchronizer::exporter(int packsize, int starting_point) {
        try {
            // time stamp
            time_t now = time(0);
            std::string time = ctime(&now);

            json export_package;
            std::string s;
            std::cout << "Type in the database directory you will track your stats from: ";
            getline(std::cin, s);
            std::cout << '\n';
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

    Player::Player(std::string _username, double tmw, double ptfo, double activity, int _region) : username(_username),
                                                                                                   tmwork_ind(tmw),
                                                                                                   ptfo_ind(ptfo),
                                                                                                   activity_ind(
                                                                                                           activity),
                                                                                                   region(_region) {}

    double Player::GTMW() { return tmwork_ind; }

    double Player::GPTFO() { return ptfo_ind; }

    double Player::GACT() { return activity_ind; }

    int Player::GReg() { return region; }

    std::string Player::GUSN() { return username; }

    Assembler::Assembler(bool update) : Synchronizer(update) {
        Classifyer();
        Exporter();
    }

    void Assembler::Classifyer() {
        for (int plr = 0; plr < usernames.size(); plr++) {
            raptorlist.emplace_back(rusernames[plr], vteamwork_index[plr], vptfo_index[plr], vactivity_index[plr],
                                    geographic_region[plr]);
        }
        std::sort(raptorlist.begin(), raptorlist.end(),
                  [](Player const &pl1, Player const &pl2) { return pl1.activity_ind > pl2.activity_ind; });
    }

    void Assembler::Exporter() {
        int op;
        std::string filename;
        bool flag = true;
        while (flag) {
            std::cout << "==========================================================================" << '\n';
            std::cout << "Do you wish to create a Coeus List for players of which region?" << '\n';
            std::cout << "1 - North America" << '\n';
            std::cout << "2 - Europe" << '\n';
            std::cout << "3 - Asia, Oceania, Africa and South America" << '\n';
            std::cout << "4 - Unknown Regions" << '\n';
            std::cout << "5 - Exit Exporter Module" << '\n';
            std::cout << "==========================================================================" << '\n';
            std::cin >> op;
            std::cin.get();
            switch (op) {
                case 1: {
                    std::cout << "You will export a Coeus List for North American Players" << '\n';
                    time_t now = time(0);
                    char time[100];
                    struct tm *timeinfo = localtime(&now);
                    strftime(time, 100, "%d%b%y", timeinfo);
                    std::string time_file = time;
                    std::cout << "================================================================================="
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
                    std::cout << "==========================================================================" << '\n';
                    break;
                }
                case 2: {
                    std::cout << "You will export a Coeus List for European Players" << '\n';
                    time_t now = time(0);
                    char time[100];
                    struct tm *timeinfo = localtime(&now);
                    strftime(time, 100, "%d%b%y", timeinfo);
                    std::string time_file = time;
                    std::cout << "================================================================================="
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
                    std::cout << "==========================================================================" << '\n';
                    break;
                }
                case 3: {
                    std::cout << "You will export a Coeus List for AASAO Players" << '\n';
                    time_t now = time(0);
                    char time[100];
                    struct tm *timeinfo = localtime(&now);
                    strftime(time, 100, "%d%b%y", timeinfo);
                    std::string time_file = time;
                    std::cout << "================================================================================="
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
                    std::cout << "==========================================================================" << '\n';
                    break;
                }
                case 4: {
                    std::cout << "You will export a Coeus List for Unknown Players" << '\n';
                    time_t now = time(0);
                    char time[100];
                    struct tm *timeinfo = localtime(&now);
                    strftime(time, 100, "%d%b%y", timeinfo);
                    std::string time_file = time;
                    std::cout << "================================================================================="
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
                    std::cout << "==========================================================================" << '\n';
                    break;
                }
                case 5: {
                    std::cout << "==========================================================================" << '\n';
                    flag = false;
                }
            }
        }
    }

    std::ostream &operator<<(std::ostream &os, Assembler &obj) {
        std::string spacer = "   |   ";
        os << "Player     | Tmwork_Ind     | Ptfo_Ind      | Activity_Ind       | " << '\n';
        for (auto &player : obj.raptorlist) {
            os << player.GUSN() << spacer << player.GTMW() << spacer << player.GPTFO() << spacer << player.GACT()
               << '\n';
        }
        return os;
    }
}
