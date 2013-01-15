#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <new> // For the new-handler
#include <cstring>

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
#include "url.h"
#include "evaluation.h"


// Define configuration parameters and the neccessary array to
// read/write the configuration to a file. If you need more variables,
// just add them below and insert an entry in the configArray.
GPVariables cfg;
const char *InfoFileName="data";
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
{"", DATAINT, NULL}
};


// Create function and terminal set
void createNodeSet(GPAdfNodeSet& adfNs) {

    // Define functions/terminals

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

    terminals.push_back(new TAge(id++));
    terminals.push_back(new TChanges(id++));
    terminals.push_back(new TVisits(id++));

    terminals.push_back(new TChangeProbabilityCho(id++));
    terminals.push_back(new TChangeProbabilityNAD(id++));
    terminals.push_back(new TChangeProbabilitySAD(id++));
    terminals.push_back(new TChangeProbabilityAAD(id++));
    terminals.push_back(new TChangeProbabilityGAD(id++));

    terminals.push_back(new TChangeProbabilityWNAD(id++));
    terminals.push_back(new TChangeProbabilityWAAD(id++));
    terminals.push_back(new TChangeProbabilityWGAD(id++));

    terminals.push_back(new TChoChangeRate(id++));

    terminals.push_back(new TNADChangeRate(id++));
    terminals.push_back(new TSADChangeRate(id++));
    terminals.push_back(new TAADChangeRate(id++));
    terminals.push_back(new TGADChangeRate(id++));

    terminals.push_back(new TWindowedNADChangeRate(id++));
    terminals.push_back(new TWindowedAADChangeRate(id++));
    terminals.push_back(new TWindowedGADChangeRate(id++));

    terminals.push_back(new TConstValue(id++, 0.001));
    terminals.push_back(new TConstValue(id++, 0.01));
    terminals.push_back(new TConstValue(id++, 0.1));
    terminals.push_back(new TConstValue(id++, 0.5));
    terminals.push_back(new TConstValue(id++, 1.0));
    terminals.push_back(new TConstValue(id++, 2.71828182));
    terminals.push_back(new TConstValue(id++, 10));
    terminals.push_back(new TConstValue(id++, 100));
    terminals.push_back(new TConstValue(id++, 1000));

    terminals.push_back(new TChoNumerator(id++));
    terminals.push_back(new TChoDenominator(id++));


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

    if(argc != 2) {
        cerr << "Usage: "<< argv[0] << " <dataset_file>" << endl;
        exit(1);
    }
    std::string dataset_filename = argv[1];

    // We set up a new-handler, because we might need a lot of memory,
    // and we don't know it's there.
    set_new_handler(newHandler);

    cout << "================= GPC++ Config ==============" << endl;

    // Init GP system.
    GPInit(0, -1);

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

    // Create the adf function/terminal set and print it out.
    GPAdfNodeSet adfNs;
    createNodeSet(adfNs);

    cout << adfNs << endl;
    fout << adfNs << endl;

    cout << "=============================================" << endl;

    // Set up the dataset and crawling resources
    DataArchive data_file;
    data_file.Init(dataset_filename);

    const int warm_up = 3; // Nuber of initial visits to get basic statistics of change
    const double resources = 0.05;
    const int num_folds = 5;
    const int max_top_gps = cfg.NumberOfGenerations * 3;
    EvaluationReport evaluation;


    for (int fold = 0; fold < num_folds; ++fold) {

        cout << "=============================================" << endl;
        cout << "Running fold " << fold << " out of " << num_folds << endl;
        fout << "Fold "<< fold << " of " << num_folds << endl << endl;
        tout << "\\section{Fold "<<fold<<" of "<<num_folds<<"}\n" << endl;


        Dataset test_set = data_file.dataset().testCV(num_folds, fold);
        Dataset train_set = data_file.dataset().trainCV(num_folds, fold);

        Dataset evolution_set  = train_set.trainCV(2, 0);
        Dataset validation_set = train_set.testCV(2, 0);

        GPSelector selector(max_top_gps);

        // Create a population with this configuration
        cout << "Creating initial population ..." << endl;
        MyPopulation* pop = new MyPopulation(cfg, adfNs, &evolution_set,
                                             resources, warm_up);
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
                                          resources, warm_up);
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

        MyGP* best_gp = selector.SelectGP(&validation_set, bests_file);

        cout << endl;
        cout << "Best GP of the last generation:" << endl;
        cout << *pop->NthGP(pop->bestOfPopulation) << endl;

        cout << "Best GP using validation method:" << endl;
        cout << *best_gp << endl;

        cout << "================= Test Phase ================" << endl;
        ostringstream fold_filename;
        fold_filename  << InfoFileName << ".fold" << fold << ends;
        ofstream fold_result_out(fold_filename.str().c_str());

        cout << "Testing best individual and baselines..." << endl;
        GPScorer gp_last((MyGP*)pop->NthGP(pop->bestOfPopulation), "gp_last");
        GPScorer gp_val(best_gp,"gp_val");

        std::vector<Scorer*> scorers;
        scorers.push_back(&gp_last);
        scorers.push_back(&gp_val);

        evaluation.Evaluate(scorers, &test_set, resources, warm_up, fold_result_out);

        fold_result_out.close();
        cout << "Results written into file: " << fold_filename.str() << endl;
    }

    ostringstream fold_mean_filename;
    fold_mean_filename  << InfoFileName << ".mean" << ends;
    ofstream fold_out(fold_mean_filename.str().c_str());

    std::vector<std::string> scorer_names;
    scorer_names.push_back("gp_last");
    scorer_names.push_back("gp_val");

    evaluation.Sumarize(scorer_names, fold_out);

    // TeX-file: end of document
    tout << endl << "\\end{document}"<< endl;
    tout.close ();

    cout << "\nResults are in "
         << InfoFileName << ".dat,"
         << InfoFileName << ".tex,"
         << InfoFileName << ".stc." << endl;

    return 0;
}
