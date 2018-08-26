//
// Created by Eric Dattore on 8/24/18.
//

#ifndef RAPTOR_NEW_BF1_H
#define RAPTOR_NEW_BF1_H

#include "headers.hpp"

namespace BF1 {
    class BF1Stats {
    protected:
        DataFetcher *dft_gs; //general stats
        DataFetcher *dft_ws; //weapons stats
        DataFetcher *dft_vs; //vehicle stats
    public:
        json gen_stats;
        json wpon_stats;
        json vhcl_stats;
        std::vector<double> remainingstats;

        BF1Stats(std::string);

        ~BF1Stats();
    };

    class Player {
    public:
        BF1Stats *sts;
        DataFetcher *dfetcher;
        std::string username;
        //Stats
        double wins;
        double losses;
        double wlratio;
        double score;
        double kdr;
        double spm;
        double kpm;
        double hs_killP;
        double kill_hitP;
        double sfm;
        double hshots;
        double kassists;
        double rplayed;
        double kills;
        double deaths;
        double accuracy;
        double avenger_kills;
        double savior_kills;
        double nemesis_kills;
        double nemesis_streak;
        double longest_hs;
        double sassists;
        double repairs;
        double heals;
        double revives;
        double flag_cap;
        double flag_def;
        double squad_score;
        double time_played;
        double s_hit;
        double s_fired;
        double meleekills;
        // include killstreak in the future??
        //Kit Stats
        std::vector<std::vector<double> > kstats;
        std::vector<std::vector<double> > gmstats;
        //Weapon Stats
        std::vector<std::vector<double> > weapons;
        //Additional Statistics for Parsing Operation
        std::vector<std::string> label_stats;
        std::vector<double> addt_stats;

        /*
		addt_stats[0] = Infantry Kills
		addt_stats[1] = Vehicle Kills
		addt_stats[2] = Resupplies
		addt_stats[3] = Ace Squad
		addt_stats[4] = MVP
		addt_stats[5] = Orders Completed
		addt_stats[6] = Squad Spawns
		addt_stats[7] = Squad Wipes
		*/

        ~Player();

        Player(std::string);

        void Parser();

        bool statsfinder(std::string);

        double statreader(std::string);

        friend std::ostream &operator<<(std::ostream&, Player*);

        friend class BF1Tracker;
    };

    class BF1Tracker : public Player {
    public:
        //Old stats
        double oss;
        double orp;
        double osa;
        double oka;
        double orsp;
        double orps;
        double ohls;
        double orvs;
        double optfo;
        double owins;
        double okills;
        double ohs;
        double oacc;
        double odgtgs;
        //Newly updated stats
        double *nss;
        double *nrp;
        double *nsa;
        double *nka;
        double *nrsp;
        double *nrps;
        double *nhls;
        double *nrvs;
        double nptfo;
        double *nwins;
        double *nkills;
        double *nhs;
        double *nacc;
        double *ndgtgs;
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
        //
        bool flag;
        double indexss;
        double indexrp;
        double indexts;
        double indexwns;
        double indexcts;
        double indexptfo;

        BF1Tracker(std::string);

        void ReadSavedData();

        void savedata();

        void Variation();

        void PerfIndexBF1(bool = true);

        json exporter();

        int GetRP();

        double GetPIndex();

        friend class Interface;
    };
}

#endif //RAPTOR_NEW_BF1_H
