// UNKSOTracker.cpp : Defines the entry point for the console application.
// Coded by PO1.JimAlmeida on behalf of the MWR Department in the Intelligence Corps
#include "headers.hpp"
#include "utilities.hpp"
#include "bf1.hpp"
#include "bf4.hpp"
#include "siege.hpp"
#include "raptor.hpp"
#include "interface.hpp"

int main() {
    std::string name;
    time_t now = time(nullptr);
    char time[100];
    struct tm *time_info = localtime(&now);
    strftime(time, 100, "%d%b%y", time_info);

    int answer;
    while (true) {
        std::cout
                << "========================================================================================================================"
                << '\n';
        std::cout << "UNKSO Tracker + Raptor Tool v1.00 - Engineering Prototype" << '\n';
        std::cout << "UNKSO Intelligence Corps" << '\n';
        std::cout << time << '\n';
        std::cout << "Set the window to full screen size for better view.\n";
        std::cout
                << "========================================================================================================================"
                << '\n';
        std::cout << "Choose one program: " << '\n';
        std::cout << "1 - UNKSO Tracker" << '\n';
        std::cout << "2 - Raptor Experiment" << '\n';
        std::cout << "3 - Database Management" << '\n';
        std::cout << "4 - BF4 API" << '\n';
        std::cout << "5 - Exit" << '\n';
        std::cout << "Input: ";
        std::cin >> answer;
        std::cin.get();
        std::cout
                << "========================================================================================================================"
                << '\n';
        if (answer == 1) { InterfaceHeader(); }
        else if (answer == 2) { RaptorHeader(); }
        else if (answer == 3) { utilities::API(); }
        else if (answer == 4) {
            std::cout << "Type it: ";
            getline(std::cin, name);
            std::cout << '\n';
            utilities::BF1API(name);
        } else if (answer == 5) { break; }
    }
    return 0;
}
