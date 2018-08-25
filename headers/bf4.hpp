//
// Created by Eric Dattore on 8/25/18.
//

#ifndef RAPTOR_NEW_BF4_H
#define RAPTOR_NEW_BF4_H

#include "headers.hpp"

class BF4Tracker {
protected:
    DataFetcher *dft;
    std::string username;
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
    ~BF4Tracker();

    BF4Tracker(std::string);

    void readsaveddata();

    void savedata();

    void Variation();

    void PerfIndexBF4(bool = true);

    json exporter();

    int GetRP();

    double GetPIndex();

    friend class Interface;
};

#endif //RAPTOR_NEW_BF4_H
