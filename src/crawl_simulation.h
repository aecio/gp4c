#ifndef CRAWL_SIMULATION_H
#define CRAWL_SIMULATION_H

#include <algorithm>
#include <vector>

class Scorer;
class Dataset;

class CrawlSimulation {
public:

    void Run(Scorer* scorer, Dataset* dataset, double resources, int warm_up);

    double AverageErrorRate();

    const std::vector<double>& ErrorRates() {
        return error_rate;
    }

private:
    std::vector<double> error_rate;
};


#endif // CRAWL_SIMULATION_H
