#ifndef URL_H
#define URL_H

#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>

#define MAX_SUM 200

class URL {
public:
    class Sum {
    public:
        Sum();
        double AAD(int i) const {
            return sum_aad[i];
        }
        double GAD(int i) const {
            return sum_gad[i];
        }
        double Pow2i1(int i) const {
            return pow_2_i_minus_1[i];
        }
    private:
        double sum_aad[MAX_SUM+1];
        double sum_gad[MAX_SUM+1];
        double pow_2_i_minus_1[MAX_SUM+1];
    };

    static const int kDefaultWindowSize = 32;
    static const Sum kSum;

    URL() : id(0),
            last_visit_(0),
            visits_(0),
            changes_(0),
            score(0),
            nad_cached_cycle_(-1),
            aad_cached_cycle_(-1),
            gad_cached_cycle_(-1) {
        history.reserve(60);
    }
    int id;
    double score;

    void Update(int cycle, bool changed) {
        #ifndef NDEBUG
        if(cycle == 0) {
            std::cerr << "Invalid cycle value: " << cycle << std::endl;
            abort();
        }
        #endif
        last_visit_ = cycle;
        visits_++;
        if(changed) {
            changes_++;
            history.push_back(cycle);

            // remove history older than kDefaulWindowSize
//            int end_window = cycle - kDefaultWindowSize;
//            while(!history.empty() && history.front() < end_window) {
//                history.pop_front();
//            }
        }
    }

    bool operator<(const URL &other) const {
        if (this->score > other.score)
            return true;
        else
            return false;
    }

    static bool ComparePtr(URL* a, URL* b) {
        if (a->score > b->score)
            return true;
        else
            return false;
    }

    double GetAge(int cycle) {
        return cycle-last_visit_;
    }

    double GetChangeProbabilityCho(int cycle) {
        return 1.0 - exp( - GetChoChangeRate() * GetAge(cycle));
    }

    double GetChangeProbabilityNAD(int cycle) {
        return 1.0 - exp( - GetNADChangeRate(cycle) * GetAge(cycle));
    }

    double GetChangeProbabilitySAD(int cycle) {
        return 1.0 - exp( - GetSADChangeRate(cycle) * GetAge(cycle));
    }

    double GetChangeProbabilityAAD(int cycle) {
        return 1.0 - exp( - GetAADChangeRate(cycle) * GetAge(cycle));
    }

    double GetChangeProbabilityGAD(int cycle) {
        return 1.0 - exp( - GetGADChangeRate(cycle) * GetAge(cycle));
    }

    // Widowed change probabilities

    double GetChangeProbabilityWNAD(int cycle) {
        return 1.0 - exp( - GetWindowedNADChangeRate(cycle) * GetAge(cycle));
    }

    double GetChangeProbabilityWAAD(int cycle) {
        return 1.0 - exp( - GetWindowedAADChangeRate(cycle) * GetAge(cycle));
    }

    double GetChangeProbabilityWGAD(int cycle) {
        return 1.0 - exp( - GetWindowedGADChangeRate(cycle) * GetAge(cycle));
    }


    // Cho's change rate estimator
    double GetChoChangeRate() {
        return -log((visits_ - changes_+0.5)/(visits_+0.5));
    }

    // Non adaptive change rate (NAD)
    double GetNADChangeRate(int cycle) {
        if(nad_cached_cycle_ == cycle) return nad_cached_value_;

//        std::cout << "ADDChangeRate()" << std::endl;

        double weight = 1.0 / (cycle-1);
        double nad = 0.0;
        for (int h = 0; h < history.size(); ++h) {
                nad += weight;
//                int i = history[h];
//                std::cout << " cycle=" << cycle
//                          << " i=" << i
//                          << " weight=" << weight
//                          << " nad=" << nad
//                          << std::endl;
        }
//        std::cout << "NAD=" << nad << std::endl;

        nad_cached_cycle_ = cycle;
        nad_cached_value_ = nad;

        return nad;
    }

    // Windowed non-adaptive change rate (NAD)
    double GetWindowedNADChangeRate(int cycle) {
        if(nad_cached_cycle_ == cycle) return nad_cached_value_;

//        std::cout << "WNADChangeRate()" << std::endl;

        int window_size = GetWindowSize(cycle);
        int window_start = cycle - 1 - window_size;

        double weight = 1.0 / (window_size);
        double nad = 0.0;
        for (int h = 0; h < history.size(); ++h) {
            if(history[h] > window_start) {
                nad += weight;
//                std::cout << " cycle=" << cycle
//                          << " window_size=" << window_size
//                          << " window_start=" << window_start
//                          << " weight=" << weight
//                          << " wnad=" << nad
//                          << std::endl;
            }
        }
//        std::cout << "WNAD=" << nad << std::endl;

        nad_cached_cycle_ = cycle;
        nad_cached_value_ = nad;

        return nad;
    }

    // Shortsighted adaptive change rate (SAD)
    double GetSADChangeRate(int cycle) {
        if(!history.empty() && history.back() == cycle-1) {
//            std::cout << "SAD=" << 1 << std::endl;
            return 1.0;
        } else {
//            std::cout << "SAD=" << 0 << std::endl;
            return 0.0;
        }
    }

    // Arithmetically adaptive change rate (AAD)
    double GetAADChangeRate(int cycle) {
        if(aad_cached_cycle_ == cycle) return aad_cached_value_;
//        std::cout << "ADDChangeRate()" << std::endl;
        double aad = 0.0;
        for (int h = 0; h < history.size(); ++h) {
            double i = history[h];
            double weight = i / kSum.AAD(cycle-1);
            aad += weight;
//            std::cout << " cycle=" << cycle
//                      << " i=" << i
//                      << " sum_aad[i]=" << kSum.AAD(i)
//                      << " weight=" << weight
//                      << " aad=" << aad
//                      << std::endl;
        }
//        std::cout << "ADD=" << aad << std::endl;

        aad_cached_cycle_ = cycle;
        aad_cached_value_ = aad;

        return aad;
    }

    // Windowed arithmetically adaptive change rate (AAD)
    double GetWindowedAADChangeRate(int cycle) {
        if(aad_cached_cycle_ == cycle) return aad_cached_value_;
//        std::cout << "WADDChangeRate()" << std::endl;
        int window_size = GetWindowSize(cycle);
        int window_start = cycle - 1 - window_size;

        double aad = 0.0;
        for (int h = 0; h < history.size(); ++h) {
            if(history[h] > window_start) {
                double i = history[h] - window_start;
//                double i = history[h];

//                double weight = i / kSum.AAD(cycle-1);
                double weight = i / kSum.AAD(window_size);

                aad += weight;
//                std::cout << " cycle=" << cycle
//                          << " i=" << i
//                          << " window_size=" << window_size
//                          << " window_start=" << window_start
//                          << " sum_aad[i]=" << kSum.AAD(i)
//                          << " weight=" << weight
//                          << " wad=" << aad
//                          << std::endl;
            }
        }
//        std::cout << "ADD=" << aad << std::endl;

        aad_cached_cycle_ = cycle;
        aad_cached_value_ = aad;

        return aad;
    }

    // Geometrically adaptive change rate (GAD)
    double GetGADChangeRate(int cycle) {
        if(gad_cached_cycle_ == cycle) return gad_cached_value_;

//        std::cout << "GADChangeRate()" << std::endl;
        double gad = 0.0;
        for (int h = 0; h < history.size(); ++h) {

            double i = history[h];
            double weight = kSum.Pow2i1(i) / kSum.GAD(cycle-1);
            gad += weight;
//            std::cout << " cycle=" << cycle
//                      << " i=" << i
//                      << " sum_gad[i]=" << kSum.GAD(i)
//                      << " weight=" << weight
//                      << " gad=" << gad
//                      << std::endl;
        }
//        std::cout << "GAD=" << gad << std::endl;

        gad_cached_cycle_ = cycle;
        gad_cached_value_ = gad;

        return gad;
    }

    int GetWindowSize(int cycle) {
        return std::min(kDefaultWindowSize, cycle-1);
    }

    // Windowed geometrically adaptive change rate (GAD)
    double GetWindowedGADChangeRate(int cycle) {
        if(gad_cached_cycle_ == cycle) return gad_cached_value_;

//        std::cout << "WGADChangeRate()" << std::endl;
        int window_size = GetWindowSize(cycle);
        int window_start = cycle - 1 - window_size;

        double gad = 0.0;
        for (int h = 0; h < history.size(); ++h) {
            if(history[h] > window_start) {
//                double i = history[h];
                double i = history[h] - window_start;

                double weight = kSum.Pow2i1(i) / kSum.GAD(window_size);
                gad += weight;
//                std::cout << " cycle=" << cycle
//                          << " i=" << i
//                          << " window_size=" << window_size
//                          << " window_start=" << window_start
//                          << " sum_gad[i]=" << kSum.GAD(i)
//                          << " weight=" << weight
//                          << " wgad=" << gad
//                          << std::endl;
            }
        }
//        std::cout << "WGAD=" << gad << std::endl;

        gad_cached_cycle_ = cycle;
        gad_cached_value_ = gad;

        return gad;
    }

    int visits() {
        return visits_;
    }

    int changes(){
        return changes_;
    }

    int last_visit() {
        return last_visit_;
    }

private:
    int last_visit_;
    int visits_;
    int changes_;
    std::vector<int> history;

    int nad_cached_cycle_;
    double nad_cached_value_;

    int aad_cached_cycle_;
    double aad_cached_value_;

    int gad_cached_cycle_;
    double gad_cached_value_;
};



#endif // URL_H
