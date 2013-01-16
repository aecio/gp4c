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

    // Set up the dataset and crawling resources
    DataArchive data_file;
    EvaluationReport evaluation;

    data_file.Init(dataset_filename);

    const int warm_up = 4; // Nuber of initial visits to get basic statistics of change
    const double resources = 0.05;
    const int num_folds = 5;

    SetBaselines();

    std::string info_file_name = "baselines";
    for (int fold = 0; fold < num_folds; ++fold) {

        cout << "=============================================" << endl;
        cout << "Running fold " << fold << " out of " << num_folds << endl;

        ostringstream fold_filename;
        fold_filename << info_file_name << ".fold" << fold << ends;
        ofstream fold_out(fold_filename.str().c_str());

        Dataset test_set = data_file.dataset().testCV(num_folds, fold);

        evaluation.Evaluate(scorers, &test_set, resources, warm_up, fold_out);

        fold_out.close();
        cout << "Results written into file: " << fold_filename.str() << endl;
    }

    ostringstream fold_mean_filename;
    fold_mean_filename  << info_file_name << ".mean" << ends;
    ofstream fold_out(fold_mean_filename.str().c_str());

    evaluation.Sumarize(names, fold_out);

    cout << "\nResults are in " << info_file_name << ".*," << endl;

    return 0;
}

