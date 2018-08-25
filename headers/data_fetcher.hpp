//
// Created by Eric Dattore on 8/24/18.
//

#ifndef RAPTOR_NEW_DATA_FETCHER_H
#define RAPTOR_NEW_DATA_FETCHER_H

#include <iostream>
#include <fstream>
#include <cpr/cpr.h>

class DataFetcher {
protected:
    std::string sourcecode;
    int statuscode;
public:
    explicit DataFetcher(std::string url) {
        try {
            auto r = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
            statuscode = r.status_code;
            sourcecode = r.text;
        }
        catch (std::exception &e) {
            std::cerr << e.what();
            sourcecode = "EmptyData";
        }
    }

    DataFetcher(std::string url, std::string authparameter, std::string apikey) {
        try {
            auto r = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false}, cpr::Header{{authparameter, apikey}});
            sourcecode = r.text;
            statuscode = r.status_code;
        }
        catch (std::exception &e) {
            std::cerr << e.what();
            sourcecode = "EmptyData";
        }
    }

    std::string GetSource() { return sourcecode; }

    void SalvaDisco(std::string filename) {
        std::string filepath = filename + ".txt";
        std::ofstream fout(filepath);
        fout << sourcecode;
        fout.close();
    }

    void pre_processed_disk_exporter(std::string filename) {
        std::string filepath = filename + ".txt";
        int pos;
        //
        pos = sourcecode.find("Please activate javascript to see everything.");
        sourcecode.erase(0, pos);
        //
        std::ofstream fout(filepath);
        fout << sourcecode;
        fout.close();
    }

    friend std::ostream &operator<<(std::ostream &os, DataFetcher &obj) {
        std::cout << obj.sourcecode;
        return os;
    }

    int GStatusCode() { return statuscode; }
};

#endif //RAPTOR_NEW_DATA_FETCHER_H
