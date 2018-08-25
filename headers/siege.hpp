//
// Created by Eric Dattore on 8/25/18.
//

#ifndef RAPTOR_NEW_SIEGE_H
#define RAPTOR_NEW_SIEGE_H

#include "headers.hpp"

namespace Siege {
    class SiegeGenStats {
    protected:
        std::vector<std::string> stats; //label
        std::vector<float> vstats; //number
        DataFetcher *fet;
    public:
        SiegeGenStats(std::string);

        void DiskKernelParser();

        double statreader(std::string);

        bool statsfinder(std::string);

        void Mostra();

        std::vector<float> getvstats();

        std::vector<std::string> getstats();

    };

    class SiegeOperators {
    protected:
        std::vector<std::string> opfinder;
        std::vector<int> values;
        std::vector<std::vector<int>> oper; //vector of operators
        DataFetcher *fetop;
        /*
		oper[0]=
		voper[0] = operatorId
		voper[1] = kills
		voper[2] = deaths
		voper[3] = K/D
		voper[4] = Wins
		voper[5] = Losses
		voper[6] = W/L Ratio
		voper[7] = Playtime
		voper[8] = special_ability#1
		voper[9] = special_ability#2 (if available)
		voper[10] = special_ability#3 (if available)
		*/
    public:
        ~SiegeOperators();

        SiegeOperators(std::string);

        double statreader(std::string);

        int findop(std::string);

        double secondaryparser(std::string);
        friend std::ostream &operator<<(std::ostream&, SiegeOperators&);

        std::vector<std::vector<int>> GOper();
    };

    class Player {
    public:
        std::map<std::string, float> statistics;
        std::map<int, int> scoredict;
        SiegeGenStats *sp;
        SiegeOperators *sop;
        std::string username;
        DataFetcher *scorestats;
    public:
        ~Player();

        Player(std::string);

        void additionalstats();

        void makemap(std::vector<float>, std::vector<std::string>);

        friend std::ostream &operator<<(std::ostream&, Player&);
    };

    class SiegeTracker : public Player {
    public:
        bool trackerflag;
        //  SAVED DATA				 API DATA				FILTERED DATA
        float oscore;
        float nscore;
        float dscore;
        float orounds_played;
        float nrounds_played;
        float drounds_played;
        float owins;
        float nwins;
        float dwins;
        float oreinforcements;
        float *nreinforcements;
        float dreinforcements;
        float obarricades;
        float *nbarricades;
        float dbarricades;
        float ogadgets_destroyed;
        float *ngadgets_destroyed;
        float dgadgets_destroyed;
        float orevives;
        float *nrevives;
        float drevives;
        float okillassists;
        float *nkillassists;
        float dkillassists;
        float okills;
        float *nkills;
        float dkills;
        float oheadshots;
        float *nheadshots;
        float dheadshots;
        float omelee_kills;
        float *nmelee_kills;
        float dmelee_kills;
        float odbno;
        float *ndbno;
        float ddbno;
        float openetration_kills;
        float *npenetration_kills;
        float dpenetration_kills;
        //PERFINDEX DATA
        double perfindex, indexscore, indexrp, indexts, indexwns, indexcts;

        SiegeTracker(std::string);

        void ReadSavedData();

        void savedata();
        void Variation();

        void PerfIndexSiege(bool = true);

        json exporter();

        int GetRP();

        double GetPIndex();

        friend class Interface;
    };
}

namespace RaptorR6 {
    class Player {
    public:
        std::string username;
        double mp; // matches played
        double rd; // reinforcements deployed
        double bd; // barricades deployed
        double gd; // gadgets destroyed
        double rv; // revives
        double ka; // kill assists
        double kl; // kills
        double ds; // deaths
        double hs; // headshots
        double mk; // melee kills
        double pk; // penetration kills
        double rank;
        double mmr;
        double bop; // best operator
        double region;
        double a_index;
        double t_index;
        double c_index;

        Player() = default;

        Player(std::string, std::vector<double>);

        //obj1 = newest; obj2 = oldest
        friend Player operator-(Player&, Player&);

        friend std::ofstream &operator<<(std::ofstream&, const Player&);
    };

    typedef std::vector<Player> rPack;

    namespace ar6 {
        // Transforms Sourcecode string to Sourcecode Vector
        std::vector<std::string> SourceCode2StringVector(std::string);

        // Returns vector of Sourcecodes
        std::vector<std::vector<std::string>> player_fetcher(const std::vector<std::string>&);

        // Transforms Sourcecode into Vector of Stats
        std::vector<double> Processor(std::vector<std::string>);

        // Auxiliary function that finds stats in Sourcecode (Player Profile)
        bool loggerstatfinder(std::string);

        // Auxiliary functions that finds players in Sourcecode (Leaderboard Page)
        std::vector<std::string> Log_Processor(std::vector<std::string>, std::vector<std::string>);

        // Returns vector of already logged Players from UNKSO DB
        std::vector<std::string> name_fetcher(rPack);

        void Exporter(rPack);

        rPack Importer();
    }

    namespace Routines {
        rPack Logger(int = 1000);

        rPack Sync();

        rPack Filter();

        rPack Algorithm(rPack);

        void Assembler(rPack);
    }

    void Header();
}

#endif //RAPTOR_NEW_SIEGE_H
