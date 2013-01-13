#ifndef URL_H
#define URL_H

#include <cmath>
#include <iostream>
#include <vector>

#define MAX_SUM 300

class URL {
public:
    class Sum {
    public:
        Sum();
        long long AAD(int i) const {
            return sum_aad[i];
        }
        long long GAD(int i) const {
            return sum_gad[i];
        }
    private:
        long long sum_aad[MAX_SUM+1];
        long long sum_gad[MAX_SUM+1];
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
        history.reserve(kDefaultWindowSize);
    }
    int id;
    double score;

    void Update(int cycle, bool changed) {
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

    // Cho's change rate estimator
    double GetChoChangeRate() {
        return -log((visits_ - changes_+0.5)/(visits_+0.5));
    }

    // Non adaptive change rate (NAD)
    double GetNADChangeRate(int cycle) {
        if(nad_cached_cycle_ == cycle) return nad_cached_value_;

        int window_size = (cycle > kDefaultWindowSize)? kDefaultWindowSize : cycle;
        int window_start = cycle - window_size;
//        cout << "window size: " << window_size << endl;
        double weight = 1.0 / window_size;
//        cout << "weight: " << weight << endl;
        double nad = 0.0;
        for (int h = 0; h < history.size(); ++h) {
            if(history[h] > window_start) {
                nad += weight;
//                double i = history[h] - window_start;
//                cout << "i: " << i
//                     << " hist[h]: " << history[h]
//                     << " cycle: " << cycle
//                     << " weight:" << weight << endl;
            }
        }

        nad_cached_cycle_ = cycle;
        nad_cached_value_ = nad;

        return nad;
    }

    // Shortsighted adaptive change rate (SAD)
    double GetSADChangeRate(int cycle) {
        if(!history.empty() && history.back() == cycle)
            return 1.0;
        else
            return 0.0;
    }

    // Arithmetically adaptive change rate (AAD)
    double GetAADChangeRate(int cycle) {
        if(aad_cached_cycle_ == cycle) return aad_cached_value_;

        int window_size = (cycle > kDefaultWindowSize)? kDefaultWindowSize : cycle;
        int window_start = cycle - window_size;

        double aad = 0.0;
        for (int h = 0; h < history.size(); ++h) {
            if(history[h] > window_start) {
                double i = history[h] - window_start;
                double weight = i / (double) kSum.AAD(window_size);
                aad += weight;
//                cout << "i: " << i
//                     << " hist[h]: " << history[h]
//                     << " cycle: " << cycle
//                     << " weight:" << weight
//                     << " sum_i:" << kSum.AAD(i)
//                     << endl;
            }
        }

        aad_cached_cycle_ = cycle;
        aad_cached_value_ = aad;

        return aad;
    }

    // Geometrically adaptive change rate (GAD)
    double GetGADChangeRate(int cycle) {
        if(gad_cached_cycle_ == cycle) return gad_cached_value_;

        int window_size = (cycle > kDefaultWindowSize)? kDefaultWindowSize : cycle;
        int window_start = cycle - window_size;

        double gad = 0.0;
        for (int h = 0; h < history.size(); ++h) {
            if(history[h] > window_start) {
                double i = history[h] - window_start;
                double weight = pow(2, i-1) / kSum.GAD(window_size);
                gad += weight;
//                cout << "i: " << i
//                     << " hist[h]: " << history[h]
//                     << " cycle: " << cycle
//                     << " weight:" << weight << endl;
            }
        }

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
