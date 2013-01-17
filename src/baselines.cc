#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include "dataset.h"
#include "scorer.h"
#include "evaluation.h"

std::vector<std::string> names;
std::vector<Scorer*> scorers;

void SetBaselines() {

    scorers.push_back(new RandomScorer());
    scorers.push_back(new AgeScorer());
    scorers.push_back(new ChoChangeProbScorer());
    scorers.push_back(new NADChangeProbScorer());
    scorers.push_back(new SADChangeProbScorer());
    scorers.push_back(new AADChangeProbScorer());
    scorers.push_back(new GADChangeProbScorer());
    scorers.push_back(new ChoChangeRateScorer());

    for (int i = 0; i < scorers.size(); ++i) {
        names.push_back(scorers[i]->Name());
    }
}

int main(int argc, char** argv) {

    if(argc != 2) {
        cerr << "Usage: "<< argv[0] << " <dataset_file>" << endl;
        exit(1);
    }
    std::string dataset_filename = argv[1];

    std::string info_file_name = "baselines";
    EvaluationReport evaluation(info_file_name);

    DataArchive data_file;
    data_file.Init(dataset_filename);

    const int warm_up = 2; // Nuber of initial visits to get basic statistics of change
    const double resources = 0.05;
    const int num_folds = 5;

    SetBaselines();

    for (int fold = 0; fold < num_folds; ++fold) {

        cout << "=============================================" << endl;
        cout << "Running fold " << fold << " out of " << num_folds << endl;

        Dataset test_set = data_file.dataset().testCV(num_folds, fold);

        evaluation.Evaluate(scorers, &test_set, resources, warm_up, fold);


        cout << "Results written into files: " << info_file_name << ".*" << endl;
    }

    evaluation.Sumarize(names);

    cout << "\nResults are in " << info_file_name << ".*," << endl;

    return 0;
}

