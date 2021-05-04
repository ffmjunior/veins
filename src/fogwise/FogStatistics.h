#pragma once

/*
 * FogStatistics.h
 *
 * Sources
 * http://www.martinbroadhurst.com/how-to-split-a-string-in-c.html
 * https://stackoverflow.com/questions/24365331/how-can-i-generate-uuid-in-c-without-using-boost-library
 *
 *  Created on: 29 de abr de 2020
 *      Author: ffmju
 */

#ifndef SRC_FOGWISE_FOGSTATISTICS_H_
#define SRC_FOGWISE_FOGSTATISTICS_H_

#include <chrono>
#include <random>
#include <sstream>

#include <veins/base/utils/Coord.h>

class FogStatistics {

public:
    static double randomNormal(double mean = 100.0, double sd = 10.0)
    {
        unsigned seed =
                std::chrono::system_clock::now().time_since_epoch().count();
        int randomHelper = rand();
        std::default_random_engine generator(seed + randomHelper);
        std::normal_distribution<double> distribution(mean, sd);
        return distribution(generator);
    }

    static int random(uint64_t limit)
    {
        return rand() % limit;
    }

    static std::string generate_uuid_v4()
    {
        // static std::random_device rd;
        //static std::mt19937 gen(rd());
        unsigned seed =
                std::chrono::system_clock::now().time_since_epoch().count();
        int randomHelper = rand();
        std::default_random_engine gen(seed + randomHelper);
        static std::uniform_int_distribution<> dis(0, 15);
        static std::uniform_int_distribution<> dis2(8, 11);
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }

    template <class Container>
    static void split(const std::string& str, Container& cont, char delim = ';')
    {
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delim)) {
            cont.push_back(token);
        }
    }

    static Coord coordFromString(std::string coordString)
    {
        std::vector<std::string> words;
            FogStatistics::split(coordString, words, ',');
            Coord c(std::stoi(words[0]), std::stoi(words[1]), std::stoi(words[2]));
            return c;
    }

};

#endif /* SRC_FOGWISE_FOGSTATISTICS_H_ */
