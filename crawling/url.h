#ifndef URL_H
#define URL_H

#include <cmath>
#include <iostream>
#include <vector>
#include <deque>

#define MAX_SUM 300

class URL {
public:
    class Sum {
    public:
        Sum() {
            for (int w = 1; w <= MAX_SUM; ++w) {
                sum_aad[w] = 0;
                for (int i = 1; i <= w; ++i) {
                    sum_aad[w] += i;
//                    cout << "aad:" << i << " " << sum_aad[w] << endl;
                }
                sum_gad[w] = 0;
                for (int i = 1; i <= w+1; ++i) {
                    sum_gad[w] += pow(2, i-1);
//                    cout << "gad:" << i << " " << sum_gad[w] << endl;
                }
            }
        }
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


    URL(): id(0), last_visit_(0), visits_(0), changes_(0), score(0) { }
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

    double GetChangeRate() {
        return -log((visits_ - changes_+0.5)/(visits_+0.5));
    }

    double GetChangeProbability() {
        return 1.0 - exp( - GetChangeRate());
    }

    double GetChangeProbability(int cycle) {
        return 1.0 - exp( - GetChangeRate() * GetAge(cycle));
    }

    // Non adaptive change rate (NAD)
    double GetNADChangeRate(int cycle) {

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
        return aad;
    }

    // Geometrically adaptive change rate (GAD)
    double GetGADChangeRate(int cycle) {

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
    std::deque<int> history;
};

const URL::Sum URL::kSum;

#endif // URL_H
