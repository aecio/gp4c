#ifndef CRAWL_SIMULATION_H
#define CRAWL_SIMULATION_H

#include <algorithm>
#include <vector>

class Scorer;
class Dataset;

class CrawlSimulation {
public:

    void Run(Scorer* scorer, Dataset* dataset, double resources, int warm_up);

    double AverageChangeRate() {
        return VectorAverage(change_rate_);
    }

    double AverageNDCG() {
        return VectorAverage(ndcg_);
    }

    const std::vector<double>& ChangeRates() {
        return change_rate_;
    }

    const std::vector<double>& NDCGs() {
        return ndcg_;
    }

private:

    double VectorAverage(std::vector<double>& vec) {
        if(vec.size() == 0) {
            return 0;
        }
        double sum = 0.0;
        for (int i = 0; i < vec.size(); ++i) {
            sum += vec[i];
        }
        return sum / vec.size();
    }

    std::vector<double> change_rate_;
    std::vector<double> ndcg_;
};


#endif // CRAWL_SIMULATION_H
