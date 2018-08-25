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
    //setlocale(LC_ALL, "Portuguese");
    system("color F0");
    std::string name;
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
        std::cout
                << "========================================================================================================================"
                << '\n';
        std::cout << "UNKSO Tracker + Raptor Tool v1.00 - Engineering Prototype" << '\n';
        std::cout << "UNKSO Intelligence Corps" << '\n';
        std::cout << time << '\n';
        std::cout << "Set the window to fullscreen size for better view.\n";
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
