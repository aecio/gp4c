#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <new> // For the new-handler
#include <cstring>
#include <iomanip>

// Include header file of genetic programming system.
#include "gp.h"
#include "gpconfig.h"

#include "gp_selector.h"
#include "genetic.h"
#include "crawl_simulation.h"
#include "dataset.h"
#include "functions.h"
#include "scorer.h"
#include "terminals.h"
#include "timer.h"
#include "url.h"
#include "evaluation.h"

// Define configuration parameters and the neccessary array to
// read/write the configuration to a file. If you need more variables,
// just add them below and insert an entry in the configArray.

enum {BASIC_TERMINAL_SET = 1, FULL_TERMINAL_SET = 2};

const char *InfoFileName="result";
int FitnessFunction = MyGP::CHANGE_RATE; // Fitness used in training
int InitialVisits = 2;   // Number of initial visits to get basic statistics of change
double Resources = 0.05; // Percent of resources used in the simulation
int NumberOfFolds = 5;   // Number of cross validation folds
int MaxTopGPs = 50;      // Number of GPs maintained for validation
int TerminalSet = BASIC_TERMINAL_SET; // Fitness used in training
int SplitDatasetByTime = true; // If the dataset must be splited by time

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
{"TerminalSet", DATAINT, &TerminalSet},
{"SplitDatasetByTime", DATAINT, &SplitDatasetByTime}
};


// Create function and terminal set
void createNodeSet(GPAdfNodeSet& adfNs) {

    std::vector<Function*> functions;

    functions.push_back(new PlusFunction('+'));
    functions.push_back(new MinusFunction('-'));
    functions.push_back(new TimesFunction('*'));
    functions.push_back(new DivisionFunction('%'));
    functions.push_back(new LogFunction('l'));
    functions.push_back(new ExpFunction('e'));
    functions.push_back(new PowFunction('p'));

    std::vector<Terminal*> terminals;

    int id = 1;

    terminals.push_back(new TTimeSinceLastVisit(id++));
    terminals.push_back(new TChanges(id++));
    terminals.push_back(new TVisits(id++));

    if(TerminalSet == FULL_TERMINAL_SET) {
//    terminals.push_back(new TChangeProbabilityCho(id++));
//    terminals.push_back(new TChangeProbabilityNAD(id++));
//    terminals.push_back(new TChangeProbabilitySAD(id++));
//    terminals.push_back(new TChangeProbabilityAAD(id++));
//    terminals.push_back(new TChangeProbabilityGAD(id++));
//    terminals.push_back(new TChangeProbabilityWNAD(id++));
//    terminals.push_back(new TChangeProbabilityWAAD(id++));
//    terminals.push_back(new TChangeProbabilityWGAD(id++));
//    terminals.push_back(new TWindowedNADChangeRate(id++));
//    terminals.push_back(new TWindowedAADChangeRate(id++));
//    terminals.push_back(new TWindowedGADChangeRate(id++));
        terminals.push_back(new TChoChangeRate(id++));
        terminals.push_back(new TNADChangeRate(id++));
        terminals.push_back(new TSADChangeRate(id++));
        terminals.push_back(new TAADChangeRate(id++));
        terminals.push_back(new TGADChangeRate(id++));
    }

    terminals.push_back(new TConstValue(id++, 0.001));
    terminals.push_back(new TConstValue(id++, 0.01));
    terminals.push_back(new TConstValue(id++, 0.1));
    terminals.push_back(new TConstValue(id++, 0.5));
    terminals.push_back(new TConstValue(id++, 1.0));
    terminals.push_back(new TConstValue(id++, 2.71828182));
    terminals.push_back(new TConstValue(id++, 10));
    terminals.push_back(new TConstValue(id++, 100));
    terminals.push_back(new TConstValue(id++, 1000));

    // Reserve space for the node sets
    adfNs.reserveSpace(1);

    // Now define the function and terminal set for each ADF and place
    // function/terminal sets into overall ADF container
    GPNodeSet& ns0 = *new GPNodeSet(functions.size()+terminals.size());
    adfNs.put(0, ns0);
    for (int i = 0; i < functions.size(); ++i) {
        ns0.putNode(*functions[i]);
    }
    for (int i = 0; i < terminals.size(); ++i) {
        ns0.putNode(*terminals[i]);
    }
}


void newHandler () {
    cerr << "\nFatal error: Out of memory." << endl;
    exit(1);
}


int main(int argc, char** argv) {

    if(argc != 3) {
        cerr << "Usage: "<< argv[0] << " <seed> <dataset_file>" << endl;
        exit(1);
    }
    std::string dataset_filename = argv[2];
    int seed = atoi(argv[1]);

    // We set up a new-handler, because we might need a lot of memory,
    // and we don't know it's there.
    set_new_handler(newHandler);

    cout << "================= GPC++ Config ==============" << endl;

    // Init GP system.
    GPInit(0, seed);

    // Declare the GP Variables, set defaults and read configuration
    // file.  The defaults will be overwritten by the configuration file
    // when read.  If it doesn't exist, the defaults will be written to
    // the file.
    GPConfiguration config(cout, "crawling.ini", configArray);

    // Open the main output file for data and statistics file. First set
    // up names for data file. We use also a TeX-file where the
    // equations are written to in TeX-style. Very nice to look at!
    // Remember we should delete the string from the stream, well just a
    // few bytes
    ostringstream strOutFile, strStatFile, strTeXFile;
    strOutFile  << InfoFileName << ".dat" << ends;
    strStatFile << InfoFileName << ".stc" << ends;
    strTeXFile  << InfoFileName << ".tex" << ends;
    ofstream fout(strOutFile.str().c_str());
    ofstream bout(strStatFile.str().c_str());
    ofstream tout;

    tout.open(strTeXFile.str().c_str(), ios::out);
    tout << endl
         << "\\documentstyle[a4]{article}" << endl
         << "\\begin{document}" << endl;

    // Print the configuration to the files just opened
    cout << cfg << endl;
    fout << cfg << endl;
    tout << "\\begin{verbatim}\n" << cfg << "\\end{verbatim}\n" << endl;

    cout << "Fitness function          = ";

    switch(FitnessFunction) {
    case MyGP::CHANGE_RATE:
        MyGP::fitness_function = MyGP::CHANGE_RATE;
        cout << "ChangeRate" << endl;
        break;
    case MyGP::NDCG:
        MyGP::fitness_function = MyGP::NDCG;
        cout << "NDCG" << endl;
        break;
    case MyGP::NCG:
        MyGP::fitness_function = MyGP::NCG;
        cout << "NCG" << endl;
        break;
    default:
        std::cerr << "Invalid fitness function." << std::endl;
        exit(1);
    }
    cout << "Terminal set              = ";
    switch(TerminalSet) {
    case BASIC_TERMINAL_SET:
        cout << "Basic" << endl;
        break;
    case FULL_TERMINAL_SET:
        cout << "Full" << endl;
        break;
    default:
        std::cerr << "Invalid teriminal set." << std::endl;
        exit(1);
    }
    cout << "Initial visits            = " << InitialVisits << endl;
    cout << "Resources                 = " << Resources << endl;
    cout << "Number of folds           = " << NumberOfFolds << endl;
    cout << "Max top GPs               = " << MaxTopGPs << endl;
    cout << "SplitDatasetByTime        = " << SplitDatasetByTime << endl;
    cout << "Seed value                = " << seed << endl;
    cout << endl;

    // Create the adf function/terminal set and print it out.
    GPAdfNodeSet adfNs;
    createNodeSet(adfNs);

    cout << adfNs << endl;
    fout << adfNs << endl;

    cout << "=============================================" << endl;

    // Set up the dataset and crawling resources
    DataArchive data_file;
    data_file.Init(dataset_filename);

    EvaluationReport evaluation(InfoFileName);


    ostringstream gps_filename;
    gps_filename  << InfoFileName << ".bestgps" << ends;
    ofstream gps_file(gps_filename.str().c_str());

    for (int fold = 0; fold < NumberOfFolds; ++fold) {

        cout << "=============================================" << endl;
        cout << "Running fold " << fold << " out of " << NumberOfFolds << endl;
        fout << "Fold "<< fold << " of " << NumberOfFolds << endl << endl;
        tout << "\\section{Fold "<<fold<<" of "<<NumberOfFolds<<"}\n" << endl;

        Dataset cv_train_set = data_file.dataset().trainCV(NumberOfFolds, fold);
        Dataset cv_test_set = data_file.dataset().testCV(NumberOfFolds, fold);

        Dataset validation_set;
        Dataset evolution_set;
        Dataset test_set;

        if(SplitDatasetByTime == 1) {
            cout << "Spliting dataset by TIME..." << endl;
            Dataset cv_evolution_set = cv_train_set.trainCV(4, 0);
            Dataset cv_validation_set = cv_train_set.testCV(4, 0);

            evolution_set  = cv_evolution_set.timeSplit(3, 0);
            validation_set = cv_validation_set.timeSplit(3, 1);
            test_set       = cv_test_set.timeSplit(3, 2);
        } else {
            cout << "Spliting dataset by PAGES..." << endl;
            evolution_set  = cv_train_set.trainCV(2, 0);
            validation_set = cv_train_set.testCV(2, 0);
            test_set       = cv_test_set;
        }


        cout << "==== data: evolution_set ====" << endl;
        evolution_set.Print();
        cout << "==== data: validation_set ===" << endl;
        validation_set.Print();

        cout << "==== data: test_set ====" << endl;
        test_set.Print();


        GPSelector selector(MaxTopGPs);

        // Create a population with this configuration

        START_TIMER(GP2C_All,  "GP2C-Process_All")
        START_TIMER(GP2C_Fold, "GP2C-Process_Fold_" << fold)

        cout << "Creating initial population ..." << endl;
        MyPopulation* pop = new MyPopulation(cfg, adfNs, &evolution_set,
                                             Resources, InitialVisits);
        pop->create();
        cout << "Ok." << endl;
        pop->createGenerationReport(1, 0, fout, bout);

        // Print the best of generation to the LaTeX-file.
        ((MyGP*) pop->NthGP(pop->bestOfPopulation))->printTeXStyle(tout);

        // This next for statement is the actual genetic programming system
        // which is in essence just repeated reproduction and crossover loop
        // through all the generations .....
        MyPopulation* newPop = NULL;

        for (int gen=1; gen <= cfg.NumberOfGenerations; gen++) {

            // Create a new generation from the old one by applying the
            // genetic operators
            if (!cfg.SteadyState) {
                newPop = new MyPopulation(cfg, adfNs, &evolution_set,
                                          Resources, InitialVisits);
            }
            pop->generate(*newPop);

            // Delete the old generation and make the new the old one
            if (!cfg.SteadyState) {
                delete pop;
                pop=newPop;
            }

            // Maintain the best GPs for the validation phase
            for (int i = 0; i < pop->containerSize(); ++i) {
                selector.Push((MyGP*) pop->NthGP(i));
            }

            // Print the best of generation to the LaTeX-file.
            tout << "Generation " << gen << ", fitness "
                   << pop->NthGP(pop->bestOfPopulation)->getFitness() << endl;
            ((MyGP*) pop->NthGP(pop->bestOfPopulation))->printTeXStyle(tout);


            // Create a report of this generation and how well it is doing
            pop->createGenerationReport (0, gen, fout, bout);
        }

        ostringstream bests_filename;
        bests_filename  << InfoFileName << ".bests" << fold << ends;
        ofstream bests_file(bests_filename.str().c_str());

        START_TIMER(GP2C_Validation, "GP2C-Validation")
        selector.Validate(&validation_set, bests_file);
        FINISH_TIMER(GP2C_Validation)

        FINISH_TIMER(GP2C_Fold)
        FINISH_TIMER(GP2C_All)

        MyGP* gp_sum = selector.BestGPSum();
        MyGP* gp_avg = selector.BestGPAvg();

        cout << endl;
        cout << "==========" << endl;
        cout << "GP2C_best:" << endl;
        cout << "==========" << endl;
        cout << *pop->NthGP(pop->bestOfPopulation);
        cout << "fitness:"
             << static_cast<MyGP*>(pop->NthGP(pop->bestOfPopulation))->fitness_e
             << endl;
        cout << "score:"
             << static_cast<MyGP*>(pop->NthGP(pop->bestOfPopulation))->fitness_e
             << endl << endl;

        cout << "==========" << endl;
        cout << "GP2C_sum:" << endl;
        cout << "==========" << endl;
        cout << *gp_sum;
        cout << "fitness_e:" << gp_sum->fitness_e << endl;
        cout << "fitness_v:" << gp_sum->fitness_v << endl;
        cout << "score:" << selector.best_sum_score() << endl << endl;

        cout << "==========" << endl;
        cout << "GP2C_avg:" << endl;
        cout << "==========" << endl;
        cout << *gp_avg;
        cout << "fitness_e:" << gp_avg->fitness_e << endl;
        cout << "fitness_v:" << gp_sum->fitness_v << endl;
        cout << "score:" << selector.best_avg_score() << endl << endl;

        gps_file << "fold " << fold
                 << " seed " << seed
                 << " gp2c_best "
                 << std::fixed << std::setprecision(10)
                 << static_cast<MyGP*>(pop->NthGP(pop->bestOfPopulation))->fitness_e
                 << " gp2c_avg "
                 << std::fixed << std::setprecision(10)
                 << selector.best_avg_score()
                 << " gp2c_sum "
                 << std::fixed << std::setprecision(10)
                 << selector.best_sum_score()
                 << endl;


        cout << "Testing best individuals..." << endl;

        GPScorer gp_best_scorer((MyGP*)pop->NthGP(pop->bestOfPopulation), "gp2c_best");
        GPScorer gp_sum_scorer(gp_sum,"gp2c_sum");
        GPScorer gp_avg_scorer(gp_avg, "gp2c_avg");

        std::vector<Scorer*> scorers;
        scorers.push_back(&gp_best_scorer);
        scorers.push_back(&gp_sum_scorer);
        scorers.push_back(&gp_avg_scorer);

        START_TIMER(GP2CTest, "GP2C-Test")
        evaluation.Evaluate(scorers, &test_set, Resources, InitialVisits, fold);
        FINISH_TIMER(GP2CTest)


    }


    std::vector<std::string> scorer_names;
    scorer_names.push_back("gp2c_best");
    scorer_names.push_back("gp2c_sum");
    scorer_names.push_back("gp2c_avg");

    evaluation.Sumarize(scorer_names);

    // TeX-file: end of document
    tout << endl << "\\end{document}"<< endl;
    tout.close ();
    gps_file.close();

    cout << "\nResults are in " << InfoFileName << ".*" << endl;

    cout << "===============" << endl;
    cout << "Resources usage "<< endl;
    cout << "===============" << endl;

    PrintTimes();

    return 0;
}
