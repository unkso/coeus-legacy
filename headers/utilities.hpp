//
// Created by Eric Dattore on 8/24/18.
//

#ifndef RAPTOR_NEW_UTILITIES_H
#define RAPTOR_NEW_UTILITIES_H

#include "headers.hpp"

namespace utilities {

    double cap(double, int);

    double stat_reader(std::string);

    std::string username_stripper(std::string &);

    template<typename T>
    double branch_performance_calculator(T container) {
        try {
            if (container.size() != 0) {
                std::vector<double> playingfrequency;
                double variancecoefficient;
                double scoreaverage = 0, percentileaverage = 0, stddeviation = 0, variance = 0;
                for (int j = 0; j < container.size(); j++) {
                    scoreaverage += container[j]->GetPIndex();
                    playingfrequency.push_back(container[j]->GetRP());
                }
                scoreaverage /= container.size();
                std::sort(playingfrequency.begin(), playingfrequency.end());
                std::reverse(playingfrequency.begin(), playingfrequency.end());
                int numb = playingfrequency.size() * 0.6;
                for (int k = 0; k < numb; k++) {
                    percentileaverage += playingfrequency[k];
                }
                percentileaverage /= numb;
                for (int z = 0; z < playingfrequency.size(); z++) {
                    variance += std::pow((playingfrequency[z] - percentileaverage), 2);
                }
                variance /= playingfrequency.size();
                stddeviation = std::sqrt(variance);
                variancecoefficient = stddeviation / percentileaverage;
                std::cout << "-------------" << '\n';
                std::cout << scoreaverage << '\n';
                std::cout << numb << '\n';
                std::cout << percentileaverage << '\n';
                std::cout << variance << '\n';
                std::cout << stddeviation << '\n';
                std::cout << variancecoefficient << '\n';
                if (variancecoefficient < 1) {
                    return (1 - variancecoefficient) * scoreaverage;
                } else return 0;
            } else return 0;
        }
        catch (std::exception &e) {
            std::cerr << e.what() << '\n';
            return -1;
        }
    }

    void BF1API(std::string);

    void API();
}

#endif //RAPTOR_NEW_UTILITIES_H
