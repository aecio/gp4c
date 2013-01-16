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
//        std::partial_sort(repository.begin(), repository.begin()+k,
//                          repository.end(), URL::ComparePtr);
        std::sort(repository.begin(), repository.end(), URL::ComparePtr);
        double errors = 0;
        double dcg = 0;
        for (int i = 0; i < k; ++i) {
            if( dataset->instance(repository[i]->id)->ChangedIn(cycle) ) {
                repository[i]->Update(cycle, true);
                if (i == 0) {
                    // same as: pow(2, relevance=1) - 1;
                    dcg = 1;
                } else {
                    // same as: pow(2, relevance=0) - 1) / (log(i + 1)
                    dcg += 1 / (log((i+1) + 1));
                }
            } else {
                repository[i]->Update(cycle, false);
                errors++;
                // dcg += 0
            }
//            std::cout << "URL["<<i<<"] cycle="<< cycle << endl;
//            std::cout << "nad=" << repository[i]->GetNADChangeRate(cycle) << endl;
//            std::cout << "sad=" << repository[i]->GetSADChangeRate(cycle) << endl;
//            std::cout << "aad=" << repository[i]->GetAADChangeRate(cycle) << endl;
//            std::cout << "gad=" << repository[i]->GetGADChangeRate(cycle) << endl;
//            std::cout << "cho=" << repository[i]->GetChoChangeRate() << endl;
        }
        // Compute remaining dcg values
        for(int i = k; i < repository.size(); ++i) {
            if( dataset->instance(repository[i]->id)->ChangedIn(cycle) ) {
                // same as: pow(2, relevance=1) - 1) / (log(i + 1)
                dcg += 1 / (log((i+1) + 1));
            }
        }
        double ndcg;
        if(dataset->IDCG(cycle) == 0) {
            ndcg = 0;
        } else {
            ndcg = dcg / dataset->IDCG(cycle);
        }
        error_rate.push_back(errors/k);
        ndcg_.push_back(ndcg);
//        std::cout << "cycle=" << cycle << " NDCG=" << ndcg << endl;
    }
//    std::cout << "AverageNDCG=" << AverageNDCG() << endl;

    for (int i = 0; i < dataset->NumInstances(); ++i) {
        delete repository[i];
    }
}
