#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include "gpconfig.h"
#include "dataset.h"
#include "scorer.h"
#include "evaluation.h"

enum {BASIC_TERMINAL_SET = 1, FULL_TERMINAL_SET = 2};

const char *InfoFileName="result";
int FitnessFunction = MyGP::CHANGE_RATE; // Fitness used in training
int InitialVisits = 2;   // Number of initial visits to get basic statistics of change
double Resources = 0.05; // Percent of resources used in the simulation
int NumberOfFolds = 5;   // Number of cross validation folds
int MaxTopGPs = 50;      // Number of GPs maintained for validation
int TerminalSet = BASIC_TERMINAL_SET; // Fitness used in training
GPVariables cfg;
struct GPConfigVarInformation configArray[]=
{
{"PopulationSize", DATAINT, &cfg.PopulationSize},
{"NumberOfGenerations", DATAINT, &cfg.NumberOfGenerations},
{"CreationType", DATAINT, &cfg.CreationType},
{"CrossoverProbability", DATADOUBLE, &cfg.CrossoverProbability},
{"CreationProbability", DATADOUBLE, &cfg.CreationProbability},
{"MaximumDepthForCreation", DATAINT, &cfg.MaximumDepthForCreation},
{"MaximumDepthForCrossover", DATAINT, &cfg.MaximumDepthForCrossover},
{"SelectionType", DATAINT, &cfg.SelectionType},
{"TournamentSize", DATAINT, &cfg.TournamentSize},
{"DemeticGrouping", DATAINT, &cfg.DemeticGrouping},
{"DemeSize", DATAINT, &cfg.DemeSize},
{"DemeticMigProbability", DATADOUBLE, &cfg.DemeticMigProbability},
{"SwapMutationProbability", DATADOUBLE, &cfg.SwapMutationProbability},
{"ShrinkMutationProbability", DATADOUBLE, &cfg.ShrinkMutationProbability},
{"SteadyState", DATAINT, &cfg.SteadyState},
{"AddBestToNewPopulation", DATAINT, &cfg.AddBestToNewPopulation},
{"InfoFileName", DATASTRING, &InfoFileName},
{"FitnessFunction", DATAINT, &FitnessFunction},
{"InitialVisits", DATAINT, &InitialVisits},
{"Resources", DATADOUBLE, &Resources},
{"NumberOfFolds", DATAINT, &NumberOfFolds},
{"MaxTopGPs", DATAINT, &MaxTopGPs},
{"TerminalSet", DATAINT, &TerminalSet}
};

std::vector<std::string> names;
std::vector<Scorer*> scorers;

void SetBaselines() {

    scorers.push_back(new RandomScorer());
    scorers.push_back(new AgeScorer());
//    scorers.push_back(new ChoChangeProbScorer());
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
    GPConfiguration config(cout, "crawling.ini", configArray);

    std::string info_file_name = "baselines";
    EvaluationReport evaluation(info_file_name);

    DataArchive data_file;
    data_file.Init(dataset_filename);

    SetBaselines();

    for (int fold = 0; fold < NumberOfFolds; ++fold) {

        cout << "=======================================" << endl;
        cout << "Running fold " << fold << " out of " << NumberOfFolds << endl;
        cout << "---------------------------------------" << endl;
        Dataset test_set = data_file.dataset().testCV(NumberOfFolds, fold);

        evaluation.Evaluate(scorers, &test_set, Resources, InitialVisits, fold);


        cout << "Results written into files: " << info_file_name << ".*" << endl;
    }

    evaluation.Sumarize(names);

    cout << "\nResults are in " << info_file_name << ".*," << endl;

    return 0;
}

