//
// Created by Eric Dattore on 8/25/18.
//

#ifndef RAPTOR_NEW_INTERFACE_HPP
#define RAPTOR_NEW_INTERFACE_HPP

#include "bf1.hpp"
#include "bf4.hpp"
#include "siege.hpp"

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
    std::string username;
    int updateoption;
public:
    ~Interface();

    Interface();

    Interface(std::vector<std::string>, std::vector<int>, int);

    //Single User Interface Functions
    void BF4Interface(std::string);

    void BF1Interface(std::string) ;

    void SiegeInterface(std::string);

    //Multi-user Interface Functions
    void mBF4Interface(std::string);

    void mBF1Interface(std::string);

    void mSiegeInterface(std::string);

    //Class Utilities
    json package_exp(std::string);

    void data_exporter(std::string);

    std::string username_stripper(std::string&);

    bool check_player(std::string, int);
};

void RaptorHeader();

void InterfaceHeader();

#endif //RAPTOR_NEW_INTERFACE_HPP
