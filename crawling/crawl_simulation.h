#ifndef CRAWL_SIMULATION_H
#define CRAWL_SIMULATION_H

#include <algorithm>
#include <vector>

#include "dataset.h"
#include "scorer.h"
#include "url.h"

class CrawlSimulation {
public:
    CrawlSimulation(Dataset* dataset_): dataset(dataset_) { }

    void Run(Scorer* scorer, int k, int warm_up);
    double AverageErrorRate();

    const std::vector<double>& ErrorRates() {
        return error_rate;
    }

private:
    std::vector<double> error_rate;
    Dataset* dataset;
};

inline void CrawlSimulation::Run(Scorer* scorer, int k, int warm_up) {

    error_rate.clear();
    error_rate.reserve(k);

    std::vector<URL*> repository(dataset->NumInstances());
    for (int i = 0; i < dataset->NumInstances(); ++i) {
        repository[i] = new URL();
        repository[i]->id = i;
    }

    int cycle = 1;
    for (; cycle < warm_up; ++cycle) {
        for (int i = 0; i < dataset->NumInstances(); ++i) {
            if( dataset->instance(repository[i]->id)->ChangedIn(cycle) ) {
                repository[i]->Update(cycle, true);
            } else {
                repository[i]->Update(cycle, false);
            }
        }
    }
    for (; cycle <= dataset->NumCycles(); ++cycle) {
        for (int i = 0; i < dataset->NumInstances(); ++i) {
            repository[i]->score = scorer->Score(*repository[i], cycle);
        }
        std::partial_sort(repository.begin(), repository.begin()+k,
                          repository.end(), URL::ComparePtr);
        double errors = 0;
        for (int i = 0; i < k; ++i) {
            if( dataset->instance(repository[i]->id)->ChangedIn(cycle) ) {
                repository[i]->Update(cycle, true);
            } else {
                repository[i]->Update(cycle, false);
                errors++;
            }
        }
        error_rate.push_back(errors/k);
    }

    for (int i = 0; i < dataset->NumInstances(); ++i) {
        delete repository[i];
    }

}

inline double CrawlSimulation::AverageErrorRate() {
    if(error_rate.size() == 0) {
        return 0;
    }
    double sum = 0.0;
    for (int i = 0; i < error_rate.size(); ++i) {
        sum += error_rate[i];
    }
    return sum / error_rate.size();
}

#endif // CRAWL_SIMULATION_H
