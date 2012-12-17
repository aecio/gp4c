#ifndef CRAWL_SIMULATION_H
#define CRAWL_SIMULATION_H

#include <algorithm>
#include <vector>

#include "dataset.h"
#include "scorer.h"

class CrawlSimulation {
public:
    CrawlSimulation(WebArchiveDataset* dataset_): dataset(dataset_) { }

    void Run(Scorer* scorer, int k);
    double AverageErrorRate();

    const std::vector<double>& ErrorRates() {
        return error_rate;
    }

private:
    std::vector<double> error_rate;
    WebArchiveDataset* dataset;
};

inline void CrawlSimulation::Run(Scorer* scorer, int k) {

    error_rate.clear();
    error_rate.reserve(k);

    std::vector<Instance*> repository(dataset->NumInstances());
    for (int i = 0; i < dataset->NumInstances(); ++i) {
        repository[i] = new Instance();
        repository[i]->id = i;
        repository[i]->last_visit = 0;
    }

    for (int cycle = 1; cycle <= dataset->NumCycles(); ++cycle) {
        for (int i = 0; i < dataset->NumInstances(); ++i) {
            repository[i]->score = scorer->Score(*repository[i], cycle);
        }
        std::partial_sort(repository.begin(), repository.begin()+k,
                          repository.end(), Instance::ComparePtr);
        double errors = 0;
        for (int i = 0; i < k; ++i) {
            repository[i]->visits++;
            repository[i]->last_visit = cycle;
            if(dataset->ChangedIn(repository[i]->id, cycle)) {
                repository[i]->changes++;
            } else {
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
