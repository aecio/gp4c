#include "crawl_simulation.h"

#include "dataset.h"
#include "scorer.h"
#include "url.h"

void CrawlSimulation::Run(Scorer* scorer, Dataset* dataset,
                          double resources, int warm_up) {

    int k = dataset->NumInstances()*resources;

    error_rate.clear();
    error_rate.reserve(dataset->NumCycles());

    std::vector<URL*> repository(dataset->NumInstances());
    for (int i = 0; i < dataset->NumInstances(); ++i) {
        repository[i] = new URL();
        repository[i]->id = i;
    }

    int cycle = 1;
    for (; cycle <= warm_up; ++cycle) {
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
//            std::cout << "URL["<<i<<"] cycle="<< cycle << endl;
//            std::cout << "nad=" << repository[i]->GetNADChangeRate(cycle) << endl;
//            std::cout << "sad=" << repository[i]->GetSADChangeRate(cycle) << endl;
//            std::cout << "aad=" << repository[i]->GetAADChangeRate(cycle) << endl;
//            std::cout << "gad=" << repository[i]->GetGADChangeRate(cycle) << endl;
//            std::cout << "cho=" << repository[i]->GetChoChangeRate() << endl;
        }
        error_rate.push_back(errors/k);

    }

    for (int i = 0; i < dataset->NumInstances(); ++i) {
        delete repository[i];
    }

}

double CrawlSimulation::AverageErrorRate() {
    if(error_rate.size() == 0) {
        return 0;
    }
    double sum = 0.0;
    for (int i = 0; i < error_rate.size(); ++i) {
        sum += error_rate[i];
    }
    return sum / error_rate.size();
}
