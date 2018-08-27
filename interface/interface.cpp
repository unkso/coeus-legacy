//
// Created by Eric Dattore on 8/25/18.
//
#include "../headers/interface.hpp"
#include "../headers/utilities.hpp"
#include "coeus.hpp"

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
