#include "crawl_simulation.h"

#include "dataset.h"
#include "scorer.h"
#include "url.h"

void CrawlSimulation::Run(Scorer* scorer, Dataset* dataset,
                          double resources, int warm_up) {

    int k = dataset->NumInstances()*resources;

    change_rate_.clear();
    change_rate_.reserve(dataset->NumCycles());
    ndcg_.reserve(dataset->NumCycles());
    ncg_.reserve(dataset->NumCycles());

    URL* data = new URL[dataset->NumInstances()];
    std::vector<URL*> repository(dataset->NumInstances());
    for (int i = 0; i < dataset->NumInstances(); ++i) {
        repository[i] = &data[i];
        repository[i]->id = i;
    }

    int cycle = 1;
    for (; cycle <= warm_up; ++cycle) {
        for (int i = 0; i < dataset->NumInstances(); ++i) {
            if( dataset->instance(
                        repository[i]->id)->ChangedIn(
                            repository[i]->last_visit(),
                            cycle)
               ) {
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
        int changes = 0;
        double dcg = 0;
        int i = 0;
        for (; i < k; ++i) {
            if( dataset->instance(repository[i]->id)->ChangedIn(
                            repository[i]->last_visit(), cycle) ) {
                ++changes;
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
                // dcg += 0
            }
//            std::cout << "URL["<<i<<"] cycle="<< cycle << endl;
//            std::cout << "nad=" << repository[i]->GetNADChangeRate(cycle) << endl;
//            std::cout << "sad=" << repository[i]->GetSADChangeRate(cycle) << endl;
//            std::cout << "aad=" << repository[i]->GetAADChangeRate(cycle) << endl;
//            std::cout << "gad=" << repository[i]->GetGADChangeRate(cycle) << endl;
//            std::cout << "cho=" << repository[i]->GetChoChangeRate() << endl;
        }
        change_rate_.push_back(changes/ ((double) k));

        // Compute remaining cg values until number of changed pages
        for(; i < dataset->PagesChanged(cycle); ++i) {
            ++changes;
        }
        ncg_.push_back(changes / ((double) dataset->PagesChanged(cycle)) );

        // Compute remaining dcg values
        for(; i < repository.size(); ++i) {
            if( dataset->instance(repository[i]->id)->ChangedIn(
                        repository[i]->last_visit(), cycle) ) {
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
        ndcg_.push_back(ndcg);
//        std::cout << "cycle=" << cycle << " NDCG=" << ndcg << endl;
    }
//    std::cout << "AverageNDCG=" << AverageNDCG() << endl;

//    for (int i = 0; i < dataset->NumInstances(); ++i) {
//        delete repository[i];
//    }
    delete[] data;
}
