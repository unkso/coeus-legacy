//
// Created by Eric Dattore on 8/25/18.
//

#ifndef RAPTOR_NEW_RAPTOR_H
#define RAPTOR_NEW_RAPTOR_H

#include "headers.hpp"

namespace Raptor {
    class User {
    public:
        std::string username;
        int roundsplayed;
        int timeplayed;
        int revives;
        int repairs;
        int heals;
        int resupplies;
        int squadscore;
        int orders_comp;
        int flagcaptured;
        int flagdefended;
        int bombsplaced;
        int bombsdefused;
        float gamemodescore;
        int geographic_region;

        /*===================================================================================================*/
        User();

        User(std::string, int, int, int, int, int, int, float, int, int, int, int, int, int);
    };

    class Vision {
    protected:
        DataFetcher *logdata;
        std::vector<std::string> t_sourcecode; //transformed source code
        int maxrank;
        int minrank;
        int rank;
        int numb;
        int numbplrs;
        bool search_for_more;
        std::map<std::string, int> list;
        std::vector<std::string> users;
        std::vector<std::string> dbusernames;
    public:
        ~Vision();

        Vision();

        std::vector<std::string> getusers();

        int loggerstatfinder(std::string);

        std::vector<std::string> SourceCode2StringVector(std::string);

        void t_sourcecode_processor();

        friend std::ostream &operator<<(std::ostream&, Vision&);
    };

    class Engine {
    protected:
        Vision *eye;
        std::string db_partition;
        //Statistical Matrix
        std::vector<std::string> usernames;
        std::vector<std::string> rusernames;

        std::vector<int> roundsplayed;
        std::vector<int> rroundsplayed;
        std::vector<int> droundsplayed;
        std::vector<int> timeplayed;
        std::vector<int> rtimeplayed;
        std::vector<int> dtimeplayed;

        std::vector<int> revives;
        std::vector<int> rrevives;
        std::vector<int> drevives;
        std::vector<int> repairs;
        std::vector<int> rrepairs;
        std::vector<int> drepairs;
        std::vector<int> heals;
        std::vector<int> rheals;
        std::vector<int> dheals;
        std::vector<int> resupplies;
        std::vector<int> rresupplies;
        std::vector<int> dresupplies;
        std::vector<int> squadscore;
        std::vector<int> rsquadscore;
        std::vector<int> dsquadscore;
        std::vector<int> orders_comp;
        std::vector<int> rorders_comp;
        std::vector<int> dorders_comp;

        std::vector<int> flagcaptured;
        std::vector<int> rflagcaptured;
        std::vector<int> dflagcaptured;
        std::vector<int> flagdefended;
        std::vector<int> rflagdefended;
        std::vector<int> dflagdefended;
        std::vector<int> bombsplaced;
        std::vector<int> rbombsplaced;
        std::vector<int> dbombsplaced;
        std::vector<int> bombsdefused;
        std::vector<int> rbombsdefused;
        std::vector<int> dbombsdefused;
        std::vector<float> gamemodescore;
        std::vector<float> rgamemodescore;
        std::vector<float> dgamemodescore;
        std::vector<int> geographic_region;
        std::vector<int> rgeographic_region;
        //End of Statistical Matrix
        //Auxiliary Statistical Fetching
        std::vector<std::string> finder = {
                "<div class=\"value\" data-stat=\"BombsPlaced\">",
                "<div class=\"value\" data-stat=\"BombsDefused\">",
                "<div class=\"value\" data-stat=\"Resupplies\">",
                "<div class=\"value\" data-stat=\"OrdersCompleted\">",
        };
        DataFetcher *dftx;
        std::vector<std::string> middlemanstats;
        std::vector<int> garbagecollector;
        //End of Auxiliary Statistical Fetching
        //Calculated Indexes
        std::vector<float> vteamwork_index;
        std::vector<float> vptfo_index;
        std::vector<float> vactivity_index;
    public:
        ~Engine();

        Engine(bool);

        //Engine
        void network_module(std::vector<std::string>);

        void parser_module(std::vector<std::string>);

        void region_filter(std::vector<std::string>);

        virtual void server_emodule();

        bool server_imodule();

        void matrixequalizer();

        void Variation();

        void Debug(std::string);

        //class utilities
        bool statfinder(std::string);

        std::vector<int> addt_stats(std::string&);

        int region_filter(std::string);

        int regionfinder(std::string, std::vector<std::string>&);

        int statistical_mode(const std::vector<int>&);

        void csv_exporter();

        virtual json exporter(int, int);
    };

    class Synchronizer : public Engine {
    protected:
        std::vector<float> vteamwork_index;
        std::vector<float> vptfo_index;
        std::vector<float> vactivity_index;
    public:
        Synchronizer(bool);

        void teamwork_index(int);

        void ptfo_index(int);

        void activity_index(int);

        void server_emodule();

        void csv_exporter();

        json exporter(int, int);
    };

    class Player {
    protected:
        double tmwork_ind;
        double ptfo_ind;
        double activity_ind;
        std::string username;
        int region;
    public:
        Player(std::string, double, double, double, int);

        friend class Assembler;

        double GTMW();

        double GPTFO();

        double GACT();

        int GReg();

        std::string GUSN();
    };

    class Assembler : public Synchronizer {
    protected:
        std::vector<Player> raptorlist;
    public:
        Assembler(bool);

        void Classifyer();

        void Exporter();

        friend std::ostream &operator<<(std::ostream&, Assembler&);
    };
}

#endif //RAPTOR_NEW_RAPTOR_H
