//
// Created by Eric Dattore on 8/25/18.
//
#include "../headers/interface.hpp"
#include "../headers/utilities.hpp"
#include "coeus.hpp"

Interface::~Interface() {
    delete bf1tracker;
    delete bf4tracker;
    delete r6tracker;
}

Interface::Interface() {
    std::string s;
    while (true) {
        bf1tracker = nullptr;
        bf4tracker = nullptr;
        r6tracker = nullptr;
        std::cout << "========================================================================" << '\n';
        std::cout << "To start, choose a game to track statistics from: " << '\n';
        std::cout << "1 - Battlefield 1 (Standard username = 'RANK_NAME')" << '\n';
        std::cout << "2 - Battlefield 4 (Standard username = 'RANK_NAME')" << '\n';
        std::cout << "3 - Rainbow Six Siege (Standard username = 'NAME.US')" << '\n';
        std::cout << "4 - Exit PROGRAM" << '\n';
        std::cout << "========================================================================" << '\n';
        std::cout << "Input: ";
        std::cin >> game;
        std::cin.get();
        if (game == 4) { break; }
        std::cout << "========================================================================" << '\n';
        std::cout << "Input username: ";
        getline(std::cin, username);
        std::cout << "========================================================================" << '\n';
        if (username == "exit") { break; }
        flag = check_player(username, game);  //check to see if player exists
        if (flag == true) {
            std::string ans;
            switch (game) {
                case 1: {
                    while (true) {
                        BF1Interface(username);
                        bf1container.push_back(bf1tracker);
                        std::cout << "========================================================================" << '\n';
                        std::cout << "Would you like to continue with another BF1 player? (Y/N)" << '\n';
                        std::cout << "========================================================================" << '\n';
                        std::cout << "Input: ";
                        getline(std::cin, ans);
                        std::cout << "========================================================================" << '\n';
                        if (ans == "Y" || ans == "y") {
                            std::cout << "Type in the next username: ";
                            getline(std::cin, username);
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
                        std::cout << "========================================================================" << '\n';
                        std::cout << "Would you like to continue with another BF4 player? (Y/N)" << '\n';
                        std::cout << "========================================================================" << '\n';
                        std::cout << "Answer: ";
                        getline(std::cin, ans);
                        if (ans == "Y" || ans == "y") {
                            std::cout << "Type in the next username: ";
                            getline(std::cin, username);
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
                        std::cout << "========================================================================" << '\n';
                        std::cout << "Would you like to continue with another Siege player? (Y/N)" << '\n';
                        std::cout << "========================================================================" << '\n';
                        std::cout << "Answer: ";
                        getline(std::cin, ans);
                        if (ans == "Y" || ans == "y") {
                            std::cout << "Type in the next username: ";
                            getline(std::cin, username);
                            flag = check_player(username, game);
                            if (flag == false) { break; }
                        } else { break; }
                    }
                    break;
                }
            }
            std::cout << "========================================================================" << '\n';
        }
        std::cout << "Do you desire to see all the stats you have so far?" << "\n";
        getline(std::cin, s);
        if (s == "Y" || s == "y") {
            std::cout << "Select a game: " << '\n';
            std::cout << "1 - Battlefield 1" << '\n';
            std::cout << "2 - Battlefield 4" << '\n';
            std::cout << "3 - Rainbow Six Siege" << '\n';
            std::cin >> game;
            std::cin.get();
            switch (game) {
                case 1: {
                    for (int i = 0; i < bf1container.size(); i++) {
                        std::cout << bf1container[i]->username << ": " << bf1container[i]->perfindex << '\n';
                    }
                    break;
                }
                case 2: {
                    for (int i = 0; i < bf4container.size(); i++) {
                        std::cout << bf4container[i]->username << ": " << bf4container[i]->perfindex << '\n';
                    }
                    break;
                }
                case 3: {
                    for (int i = 0; i < r6container.size(); i++) {
                        std::cout << r6container[i]->username << ": " << r6container[i]->perfindex << '\n';
                    }
                    break;
                }
            }
        }
    }
}

Interface::Interface(std::vector<std::string> namelist, std::vector<int> gamelist, int u_option) : updateoption(u_option) {
    std::cout << "========================================================================" << '\n';
    std::cout << "Processing all players..." << '\n';
    std::cout << "========================================================================" << '\n';
    int game;
    std::string s;
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
    } else { std::cout << "Namelist Vector is not the same in size as Gamelist vector!" << '\n'; }
    std::cout << "========================================================================" << '\n';
    std::cout << "Do you desire to see all the stats you have so far?" << "\n";
    std::cout << "Input: ";
    getline(std::cin, s);
    std::cout << "========================================================================" << '\n';
    if (s == "Y" || s == "y") {
        while (true) {
            std::cout << "Select a game: " << '\n';
            std::cout << "1 - Battlefield 1" << '\n';
            std::cout << "2 - Battlefield 4" << '\n';
            std::cout << "3 - Rainbow Six Siege" << '\n';
            std::cout << "4 - Exit" << '\n';
            std::cout << "========================================================================" << '\n';
            std::cout << "Input: ";
            std::cin >> game;
            std::cin.get();
            if (game == 4) { break; }
            switch (game) {
                case 1: {
                    std::cout << "========================================================================" << '\n';
                    std::cout << "Individual BF1 Players and their Performance Indexes" << '\n';
                    std::cout << "----------------------------------------------------" << '\n';
                    for (int i = 0; i < bf1container.size(); i++) {
                        std::cout << bf1container[i]->username << ": " << bf1container[i]->perfindex << '\n';
                        std::cout << "   -------" << '\n';
                    }
                    std::cout << "========================================================================" << '\n';
                    break;
                }
                case 2: {
                    std::cout << "========================================================================" << '\n';
                    std::cout << "Individual BF4 Players and their Performance Indexes" << '\n';
                    std::cout << "----------------------------------------------------" << '\n';
                    for (int i = 0; i < bf4container.size(); i++) {
                        std::cout << bf4container[i]->username << ": " << bf4container[i]->perfindex << '\n';
                        std::cout << "   -------" << '\n';
                    }
                    std::cout << "========================================================================" << '\n';
                    break;
                }
                case 3: {
                    std::cout << "========================================================================" << '\n';
                    std::cout << "Individual Siege Players and their Performance Indexes" << '\n';
                    std::cout << "------------------------------------------------------" << '\n';
                    for (int i = 0; i < r6container.size(); i++) {
                        std::cout << r6container[i]->username << ": " << r6container[i]->perfindex << '\n';
                        std::cout << "   -------" << '\n';
                    }
                    std::cout << "========================================================================" << '\n';
                    break;
                }
            }
        }
    }
    std::cout << "Calculating weighted branch performance index!" << '\n';
    std::cout << "Navy (BF1): " << utilities::branch_performance_calculator<std::vector<BF1::BF1Tracker *>>(bf1container) << '\n';
    std::cout << "Air Force (BF4): " << utilities::branch_performance_calculator<std::vector<BF4Tracker *>>(bf4container) << '\n';
    std::cout << "Coast Guard (R6 Siege): "
         << utilities::branch_performance_calculator<std::vector<Siege::SiegeTracker *>>(r6container) << '\n';
}

//Single User Interface Functions
void Interface::BF4Interface(std::string username) {
    std::string ans;
    std::cout << "========================================================================" << '\n';
    std::cout << "                        AVAILABLE OPTIONS                               " << '\n';
    std::cout << "                        -----------------								 " << '\n';
    std::cout << "1 - MWR IBCC Tournament Tracking" << '\n';
    std::cout << "2 - UNKSO Leaderboards (NOT IMPLEMENTED)" << '\n';
    std::cout << "3 - Combat MOS Tracking (NOT IMPLEMENTED)" << '\n';
    std::cout << "4 - Basic Combat Training Tracking (NOT IMPLEMENTED)" << '\n';
    std::cout << "5 - EXIT TO GAME INTERFACE" << '\n';
    std::cout << "========================================================================" << '\n';
    std::cout << "Input: ";
    getline(std::cin, ans);
    std::cout << "========================================================================" << '\n';
    if (ans == "1") {
        bf4tracker = new BF4Tracker(username);
        if (bf4tracker->flag == false) {
            std::cout << "You have not started tracking the player: " << username
                 << ". Do you desire to do it now? (Y/N)" << '\n';
            getline(std::cin, ans);
            if (ans == "Y" || ans == "y") {
                data_exporter(username);
                std::cout << "This player is being tracked!" << '\n';
            } else {
                std::cout << "You will not be tracking this player. The Performance Index will not be calculated."
                     << '\n';
            }
        } else if (bf4tracker->flag == true) {
            std::cout
                    << "Tracking for this player is possible. Would you like to calculate his/hers performance index? (Y/N)"
                    << '\n';
            getline(std::cin, ans);
            if (ans == "Y" || ans == "y") { bf4tracker->PerfIndexBF4(); }
            else { std::cout << "The Performance Index will not be calculated." << '\n'; }
            std::cout << "Would you like to update " << username << "'s data? (Y/N)" << '\n';
            getline(std::cin, ans);
            if (ans == "Y" || ans == "y") {
                data_exporter(username);
            } else { std::cout << "Data for this player will not be updated!" << '\n'; }
        }
    } else if (ans == "5") { std::cout << "Exiting..." << '\n'; }
    else { std::cout << "ACCESS DENIED!" << '\n'; }
}

void Interface::BF1Interface(std::string username) {
    std::string ans;
    std::cout << "========================================================================" << '\n';
    std::cout << "                        AVAILABLE OPTIONS                               " << '\n';
    std::cout << "                        -----------------								 " << '\n';
    std::cout << "1 - MWR IBCC Tournament Tracking" << '\n';
    std::cout << "2 - UNKSO Leaderboards (NOT IMPLEMENTED)" << '\n';
    std::cout << "3 - Combat MOS Tracking (NOT IMPLEMENTED)" << '\n';
    std::cout << "4 - Basic Combat Training Tracking (NOT IMPLEMENTED)" << '\n';
    std::cout << "5 - EXIT TO GAME INTERFACE" << '\n';
    std::cout << "========================================================================" << '\n';
    std::cout << "Input: ";
    getline(std::cin, ans);
    std::cout << "========================================================================" << '\n';
    if (ans == "1") {
        bf1tracker = new BF1::BF1Tracker(username);
        if (bf1tracker->flag == false) {
            std::cout << "You have not started tracking the player: " << username
                 << ". Do you desire to do it now? (Y/N)" << '\n';
            getline(std::cin, ans);
            if (ans == "Y" || ans == "y") {
                data_exporter(username);
                std::cout << "This player is being tracked!" << '\n';
            } else {
                std::cout << "You will not be tracking this player. The Performance Index will not be calculated."
                     << '\n';
            }
        } else if (bf1tracker->flag == true) {
            std::cout
                    << "Tracking for this player is possible. Would you like to calculate his/hers performance index? (Y/N)"
                    << '\n';
            getline(std::cin, ans);
            if (ans == "Y" || ans == "y") { bf1tracker->PerfIndexBF1(); }
            else { std::cout << "The Performance Index will not be calculated." << '\n'; }
            std::cout << "Would you like to update " << username << "'s data? (Y/N)" << '\n';
            getline(std::cin, ans);
            if (ans == "Y" || ans == "y") {
                data_exporter(username);
            } else { std::cout << "Data for this player will not be updated!" << '\n'; }
        }
    } else if (ans == "5") { std::cout << "Exiting..." << '\n'; }
    else { std::cout << "ACCESS DENIED!" << '\n'; }
}

void Interface::SiegeInterface(std::string username) {
    std::string ans;
    std::cout << "========================================================================" << '\n';
    std::cout << "                        AVAILABLE OPTIONS                               " << '\n';
    std::cout << "                        -----------------								 " << '\n';
    std::cout << "1 - MWR IBCC Tournament Tracking" << '\n';
    std::cout << "2 - UNKSO Leaderboards (NOT IMPLEMENTED)" << '\n';
    std::cout << "3 - Combat MOS Tracking (NOT IMPLEMENTED)" << '\n';
    std::cout << "4 - Basic Combat Training Tracking (NOT IMPLEMENTED)" << '\n';
    std::cout << "5 - EXIT TO GAME INTERFACE" << '\n';
    std::cout << "========================================================================" << '\n';
    std::cout << "Input: ";
    getline(std::cin, ans);
    std::cout << "========================================================================" << '\n';
    if (ans == "1") {
        r6tracker = new Siege::SiegeTracker(username);
        if (r6tracker->trackerflag == false) {
            std::cout << "You have not started tracking the player: " << username
                 << ". Do you desire to do it now? (Y/N)" << '\n';
            getline(std::cin, ans);
            if (ans == "Y" || ans == "y") {
                data_exporter(username);
                std::cout << "This player is being tracked!" << '\n';
            } else {
                std::cout << "You will not be tracking this player. The Performance Index will not be calculated."
                     << '\n';
            }
        } else if (r6tracker->trackerflag == true) {
            std::cout
                    << "Tracking for this player is possible. Would you like to calculate his/hers performance index? (Y/N)"
                    << '\n';
            getline(std::cin, ans);
            if (ans == "Y" || ans == "y") { r6tracker->PerfIndexSiege(); }
            else { std::cout << "The Performance Index will not be calculated." << '\n'; }
            std::cout << "Would you like to update " << username << "'s data? (Y/N)" << '\n';
            getline(std::cin, ans);
            if (ans == "Y" || ans == "y") {
                data_exporter(username);
            } else { std::cout << "Data for this player will not be updated!" << '\n'; }
        }
    } else if (ans == "5") { std::cout << "Exiting..." << '\n'; }
    else { std::cout << "ACCESS DENIED!" << '\n'; }
}

//Multi-user Interface Functions
void Interface::mBF4Interface(std::string username) {
    try {
        std::string option;
        bf4tracker = new BF4Tracker(username);
        if (bf4tracker->flag == false) { data_exporter(username); }
        else if (bf4tracker->flag == true) {
            bf4tracker->PerfIndexBF4(false);
            if (updateoption == 1) {
                data_exporter(username);
            } else if (updateoption == 2) {
                std::cout << "========================================================================" << '\n';
                std::cout << "Update " << username << "'s data?" << '\n';
                getline(std::cin, option);
                std::cout << "========================================================================" << '\n';
                if (option == "Y" || option == "y") {
                    data_exporter(username);
                }
                std::cout << "========================================================================" << '\n';
            }
            //----------------------------------------------------//
        }
    }
    catch (std::exception &e) {
        std::cerr << e.what() << '\n';
    }
}

void Interface::mBF1Interface(std::string username) {
    try {
        std::string option;
        bf1tracker = new BF1::BF1Tracker(username);
        if (bf1tracker->flag == false) { data_exporter(username); }
        else if (bf1tracker->flag == true) {
            bf1tracker->PerfIndexBF1(false);
            if (updateoption == 1) {
                data_exporter(username);
            } else if (updateoption == 2) {
                std::cout << "========================================================================" << '\n';
                std::cout << "Update " << username << "'s data?" << '\n';
                getline(std::cin, option);
                std::cout << "========================================================================" << '\n';
                if (option == "Y" || option == "y") {
                    data_exporter(username);
                }
                std::cout << "========================================================================" << '\n';
            }
        }
    }
    catch (std::exception &e) {
        std::cerr << e.what() << '\n';
    }
}

void Interface::mSiegeInterface(std::string username) {
    try {
        std::string option;
        r6tracker = new Siege::SiegeTracker(username);
        if (r6tracker->trackerflag == false) { data_exporter(username); }
        else if (r6tracker->trackerflag == true) {
            r6tracker->PerfIndexSiege(false);
            if (updateoption == 1) {
                data_exporter(username);
            } else if (updateoption == 2) {
                std::cout << "========================================================================" << '\n';
                std::cout << "Update " << username << "'s data?" << '\n';
                getline(std::cin, option);
                std::cout << "========================================================================" << '\n';
                if (option == "Y" || option == "y") {
                    data_exporter(username);
                }
                std::cout << "========================================================================" << '\n';
            } else {
                std::cout << "Players will not be synchronized with UNKSO Database!" << '\n';
                std::cout << "===========================================================================" << '\n';
            }
        }
    }
    catch (std::exception &e) {
        std::cerr << e.what() << '\n';
    }
}

//Class Utilities
json Interface::package_exp(std::string username) {
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

void Interface::data_exporter(std::string username) {
    json data = package_exp(username);
    auto r = cpr::Post(cpr::Url{"http://unkso.michaeljamesbondparsons.com/stats"},
                       cpr::Body{data.dump()},
                       cpr::Header{{"content-type",  "application/json"},
                                   {"Authorization", "Bearer 3C2B19E2946893CBE1AA14A7023867DAFDA0D4F1EEA9D4FF9C54EB4D09074C2B"}});
    if (r.status_code == 200) { std::cout << "Data has been exported successfully!" << '\n'; }
    else { std::cout << "Something went wrong when exporting the data. Status Code: " << r.status_code << '\n'; }
}

std::string Interface::username_stripper(std::string &username) {
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

bool Interface::check_player(std::string username, int game) {
    if (game == 1) {
        DataFetcher test_fet(
                "https://battlefieldtracker.com/bf1/api/Stats/DetailedStats?platform=3&displayName=" + username,
                "TRN-Api-Key", "152de4e7-22a4-4a0a-b2b6-2bea2c7886b6");
        std::cout << "Checking BF1 Online Database for player: " << username << '\n';
        if (test_fet.GStatusCode() != 200) {
            std::cout
                    << "This player has not been found! Please ensure your username adheres to UNKSO standards or that you have the correct updated username."
                    << '\n';
            std::cout << "Status Code" << test_fet.GStatusCode() << '\n';
            return false;
        } else {
            std::cout << "Player has been found in online database!" << '\n';
            return true;
        }
    }
    if (game == 2) {
        DataFetcher test_fet("http://api.bf4stats.com/api/playerInfo?plat=pc&name=" + username + "&output=json");
        std::cout << "Checking BF4 Online Database for player: " << username << '\n';
        if (test_fet.GStatusCode() != 200) {
            std::cout
                    << "This player has not been found! Please ensure your username adheres to UNKSO standards or that you have the correct updated username."
                    << '\n';
            return false;
        } else {
            std::cout << "Player has been found in online database!" << '\n';
            return true;
        }
    }
    if (game == 3) {
        DataFetcher test_fet("https://r6.tracker.network/profile/pc/" + username);
        std::cout << "Checking Siege Online Database for player: " << username << '\n';
        int pos = test_fet.GetSource().find(
                "We could not find your stats, please ensure your platform and name are correct");
        if (pos != -1) {
            std::cout
                    << "Player has not been found in Online Database! Please ensure your username adheres to UNKSO standards or that you have the correct updated username."
                    << '\n';
            return false;
        } else {
            std::cout << "Player has been found in Online Database" << '\n';
            return true;
        }
    }

    return false;
}

void RaptorHeader() {
    int op;
    std::cout << "Welcome to the Coeus Tool! Read the instructions and choose an option to begin working." << '\n';
    std::cout
            << "========================================================================================================================"
            << '\n';
    std::cout << "Instructions of Use: " << '\n';
    std::cout
            << "The Logger module is responsible for searching the Battlefield 1 Leaderboards for players to be classified. The Coeus List will not be generated in this step."
            << "The Sync module is responsible for comparing the stats of the players in our database with the stats of an initial time and the most recents stats from the BF1 API."
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
        Coeus::Engine logger(false);
    } else if (op == 2) {
        Coeus::Assembler raptorlist(true);
    } else {
        std::cout << "You have not selected a valid option." << '\n';
        std::cout
                << "====================================================================================================================================================================================================="
                << '\n';
    }
}
