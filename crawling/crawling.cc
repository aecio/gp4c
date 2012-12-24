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

#include "crawling.h"
#include "crawl_simulation.h"
#include "dataset.h"
#include "functions.h"
#include "scorer.h"
#include "terminals.h"
#include "url.h"
#include "evaluation.h"

typedef std::ostringstream ostrstream;

// Defines the dataset
int partition_scheme;
WebArchiveDataset data_file;
Dataset train_set;
Dataset test_set;

int resources; // Resources in functions of URL that can be crawled
int warm_up; // Nuber of initial revisits to get basic statistics of change

// The TeX-file
ofstream texout;
int printTexStyle=0;

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


// Print out a gene in typical math style. Don't be confused, I don't
// make a difference whether this gene is the main program or an ADF,
// I assume the internal structure is correct.
void MyGene::printMathStyle (ostream& os, int lastPrecedence) {

    // Function or terminal?
    if (isFunction()) {
        // Determine operator priority
        int precedence = static_cast<Function*>(node)->Precedence();

        // Do we need brackets?
        if (lastPrecedence > precedence) {
            os << "(";
        }
        // Print out the operator and the parameters
        static_cast<Function*>(node)->PrintMathStyle(this, os, precedence);

        // Do we need brackets?
        if (lastPrecedence > precedence) {
            os << ")";
        }
    }

    // Print the terminal
    if (isTerminal()) {
        static_cast<Terminal*>(node)->PrintMathStyle(os);
    }
}


// Print out a gene in LaTeX-style. Don't be confused, I don't make a
// difference whether this gene is the main program or an ADF, I
// assume the internal structure is correct.
void MyGene::printTeXStyle(ostream& os, int lastPrecedence) {

    // Function or terminal?
    if (isFunction()) {
        // Determine operator priority
        int precedence = static_cast<Function*>(node)->Precedence();

        // Do we need brackets?
        if(lastPrecedence > precedence) {
            os << "\\left(";
        }

        // Print out the operator and the parameters
        static_cast<Function*>(node)->PrintTexStyle(this, os, precedence);

        // Do we need brackets?
        if (lastPrecedence > precedence) {
            os << "\\right)";
        }
    }

    // We can't let the terminal print itself, because we want to modify
    // it a little bit
    if(isTerminal()) {
        static_cast<Terminal*>(node)->PrintTexStyle(os);
        //GPExitSystem ("MyGene::printTeXStyle", "Undefined terminal value");
    }
}

// Print a Gene.
void MyGene::printOn(ostream& os) {
    if (printTexStyle)
        printTeXStyle (os);
    else
        printMathStyle (os);
}


// We have the freedom to define this function in any way we like.
double MyGene::evaluate(URL& url, int cycle) {
    if(isFunction()) {
        return static_cast<Function*>(node)->Value(this, url, cycle);
    }
    if(isTerminal()) {
        return static_cast<Terminal*>(node)->Value(url, cycle);
    }
}



// Print a GP. If we want a LaTeX-output, we must provide for the
// equation environment, otherwise we simply call the print function
// of our base class.
void MyGP::printOn(ostream& os) {
    // If we use LaTeX-style, we provide here for the right equation
    // overhead used for LaTeX.
    if (printTexStyle) {
        texout << "\\begin{eqnarray}" << endl;

        // Print all ADF's, if any
        GPGene* current;
        for (int n=0; n<containerSize(); n++)
        {
            if (n!=0)
                os << "\\\\" << endl;
            os << "f_" << n+1 << " & = & ";
            if ((current=NthGene(n)))
                os << *current;
            else
                os << " NONE";
            os << "\\nonumber ";
        }
        texout << endl << "\\end{eqnarray}" << endl << endl;

    }
    else
        // Just call the print function of our base class to do the
        // standard job.
        GP::printOn (os);
}


// Evaluate the fitness of a GP and save it into the GP class variable
// stdFitness.
void MyGP::evaluate() {

    GPScorer scorer(this);

    CrawlSimulation simulator;
    simulator.Run(&scorer, &train_set, resources, warm_up);
    double avg_error_rate = simulator.AverageErrorRate();

    // add a slight penalization for long functions
    stdFitness = avg_error_rate /* + ( length()*0.0001 ) */;
}


// Create function and terminal set
void createNodeSet (GPAdfNodeSet& adfNs) {
    // Reserve space for the node sets
    adfNs.reserveSpace(1);

    // Now define the function and terminal set for each ADF and place
    // function/terminal sets into overall ADF container
    GPNodeSet& ns0 = *new GPNodeSet(17);
    adfNs.put(0, ns0);

    // Define functions/terminals and place them into the appropriate
    // sets. Terminals take two arguments, functions three (the third
    // parameter is the number of arguments the function has)
    ns0.putNode(*new PlusFunction('+'));
    ns0.putNode(*new MinusFunction('-'));
    ns0.putNode(*new TimesFunction('*'));
    ns0.putNode(*new DivisionFunction('%'));
    ns0.putNode(*new LogFunction('l'));
    ns0.putNode(*new ExpFunction('e'));
    ns0.putNode(*new PowFunction('p'));

    ns0.putNode(*new AgeTerminal(1));
    ns0.putNode(*new ChangesTerminal(2));
    ns0.putNode(*new VisitsTerminal(3));
    ns0.putNode(*new OneTerminal(4));
    ns0.putNode(*new ChangeRateTerminal(5));
    ns0.putNode(*new ChangeProbabilityTerminal(6));
//    ns0.putNode(*new ChangeProbabilityAgeTerminal(7));

    ns0.putNode(*new NADChangeRateTerminal(8));
    ns0.putNode(*new SADChangeRateTerminal(9));
    ns0.putNode(*new AADChangeRateTerminal(10));
    ns0.putNode(*new GADChangeRateTerminal(11));
}


void newHandler () {
    cerr << "\nFatal error: Out of memory." << endl;
    exit (1);
}


int main(int argc, char** argv) {

    if(argc != 2) {
        cerr << "Usage: "<< argv[0] << " <dataset_file>" << endl;
        exit(1);
    }
    // We set up a new-handler, because we might need a lot of memory,
    // and we don't know it's there.
    set_new_handler (newHandler);


    std::string dataset_filename = argv[1];

    // Set up the dataset and crawling resources
    data_file.Init(dataset_filename);
    data_file.dataset().Randomize();


    cout << "========= GPC++ Config ==========" << endl;

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
    ostrstream strOutFile, strStatFile, strTeXFile;
    strOutFile  << InfoFileName << ".dat" << ends;
    strStatFile << InfoFileName << ".stc" << ends;
    strTeXFile  << InfoFileName << ".tex" << ends;
    ofstream fout (strOutFile.str().c_str());
    ofstream bout (strStatFile.str().c_str());
    texout.open (strTeXFile.str().c_str(), ios::out);
    texout << endl
         << "\\documentstyle[a4]{article}" << endl
         << "\\begin{document}" << endl;

    // Print the configuration to the files just opened
    fout << cfg << endl;
    cout << cfg << endl;
    texout << "\\begin{verbatim}\n" << cfg << "\\end{verbatim}\n" << endl;

    // Create the adf function/terminal set and print it out.
    GPAdfNodeSet adfNs;
    createNodeSet(adfNs);
    cout << adfNs << endl;
    fout << adfNs << endl;

    EvaluationReport evaluation;

    const int num_folds = 5;
    for (int fold = 0; fold < num_folds; ++fold) {

        cout << "Running fold " << fold << " out of " << num_folds << endl;
        fout << "Fold "<< fold << " of " << num_folds << endl << endl;
        texout << "\\section{Fold "<<fold<<" of "<<num_folds<<"}\n" << endl;

        ostrstream fold_file;
        fold_file  << InfoFileName << ".fold" << fold << ends;
        ofstream fold_result_out(fold_file.str().c_str());

        test_set = data_file.dataset().testCV(num_folds, fold);
        train_set = data_file.dataset().trainCV(num_folds, fold);

        double resources_percent = 0.05;
        resources = train_set.NumInstances()*resources_percent;
        warm_up = 3;

        cout << "Crawling resources used in training: " << resources
             << " ("<<resources_percent*100<<"%)" << endl;
        cout << "Warm-up initial visits: "  << warm_up << endl;


        // Create a population with this configuration
        cout << "Creating initial population ..." << endl;
        MyPopulation* pop = new MyPopulation(cfg, adfNs);
        pop->create();
        cout << "Ok." << endl;
        pop->createGenerationReport(1, 0, fout, bout);

        // Print the best of generation to the LaTeX-file.
        printTexStyle=1;
        texout << *pop->NthGP(pop->bestOfPopulation);
        printTexStyle=0;

        // This next for statement is the actual genetic programming system
        // which is in essence just repeated reproduction and crossover loop
        // through all the generations .....
        MyPopulation* newPop = NULL;
        for (int gen=1; gen <= cfg.NumberOfGenerations; gen++) {

            // Create a new generation from the old one by applying the
            // genetic operators
            if (!cfg.SteadyState) {
                newPop=new MyPopulation (cfg, adfNs);
            }
            pop->generate(*newPop);

            // Delete the old generation and make the new the old one
            if (!cfg.SteadyState) {
                delete pop;
                pop=newPop;
            }

            // Print the best of generation to the LaTeX-file.
            printTexStyle=1;
            texout << "Generation " << gen << ", fitness "
                 << pop->NthGP(pop->bestOfPopulation)->getFitness() << endl;
            texout << *pop->NthGP(pop->bestOfPopulation);
            printTexStyle=0;

            // Create a report of this generation and how well it is doing
            pop->createGenerationReport (0, gen, fout, bout);
        }

        cout << "============== Best Individual ===============" << endl;
        cout << endl << *pop->NthGP(pop->bestOfPopulation) << endl;

        cout << "============ Baselines Comparison ============" << endl;
        resources = test_set.NumInstances()*resources_percent;
        cout << "Crawling resources used in training: " << resources
             << " ("<<resources_percent*100<<"%)" << endl;
        cout << "Warm-up initial visits: "  << warm_up << endl;
        cout << "=============================================" << endl;

        Scorer* gp = new GPScorer((MyGP*)pop->NthGP(pop->bestOfPopulation));
        evaluation.Evaluate(gp, &test_set, resources, warm_up, fold_result_out);
        delete gp;

        fold_result_out.close();
    }

    // TeX-file: end of document
    texout << endl << "\\end{document}"<< endl;
    texout.close ();

    cout << "\nResults are in "
         << InfoFileName << ".dat,"
         << InfoFileName << ".tex,"
         << InfoFileName << ".stc." << endl;

    ostrstream fold_mean_file;
    fold_mean_file  << InfoFileName << ".fold.mean" << ends;
    ofstream fold_result_out(fold_mean_file.str().c_str());
    evaluation.Sumarize(fold_result_out);

    return 0;
}
