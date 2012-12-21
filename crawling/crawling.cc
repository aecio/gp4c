#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

#include <stdlib.h>
#include <new>    // For the new-handler
#include <math.h>   // fabs()
#include <string.h>

// Include header file of genetic programming system.
#include "gp.h" 
#include "gpconfig.h"

#include "crawling.h"
#include "crawl_simulation.h"
#include "dataset.h"
#include "scorer.h"
#include "terminals.h"
#include "functions.h"

typedef std::ostringstream ostrstream;

// Defines the dataset
int partition_scheme;
WebArchiveDataset dataset;
Dataset train_set;
Dataset test_set;

int resources; // Resources in functions of URL that can be crawled
int warm_up; // Nuber of initial revisits to get basic statistics of change

// The TeX-file
ofstream tout;
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
        tout << "\\begin{eqnarray}" << endl;

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
        tout << endl << "\\end{eqnarray}" << endl << endl;

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

    CrawlSimulation simulator(&train_set);
    simulator.Run(&scorer, resources, warm_up);

    double avg_error_rate = simulator.AverageErrorRate();

    // add a slight penalization for long functions
    stdFitness = avg_error_rate + ( length()*0.0001 );
}


// Create function and terminal set
void createNodeSet (GPAdfNodeSet& adfNs) {
    // Reserve space for the node sets
    adfNs.reserveSpace(1);

    // Now define the function and terminal set for each ADF and place
    // function/terminal sets into overall ADF container
    GPNodeSet& ns0 = *new GPNodeSet(14);
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

    ns0.putNode(*new AgeTerminal('1'));
    ns0.putNode(*new ChangesTerminal('2'));
    ns0.putNode(*new VisitsTerminal('3'));
    ns0.putNode(*new OneTerminal('4'));
    ns0.putNode(*new ChangeRateTerminal('5'));
    ns0.putNode(*new ChangeProbabilityTerminal('6'));
    ns0.putNode(*new ChangeProbabilityAgeTerminal('7'));
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
    std::string filename = argv[1];

    // We set up a new-handler, because we might need a lot of memory,
    // and we don't know it's there.
    set_new_handler (newHandler);


    // Set up the dataset and crawling resources
    dataset.Init(filename);

    test_set = dataset.dataset_.testCV(3, 0);
    train_set = dataset.dataset_.trainCV(3, 0);

    double resources_percent = 0.05;
    resources = test_set.NumInstances()*resources_percent;
    warm_up = 3;

    cout << "Crawling resources used: " << resources
         << " ("<<resources_percent*100<<"%)" << endl;
    cout << "Warm-up initial visits: "  << warm_up << endl;


    cout << "========= GPC++ Config ==========" << endl;

    // Init GP system.
    GPInit (0, -1);

    // Declare the GP Variables, set defaults and read configuration
    // file.  The defaults will be overwritten by the configuration file
    // when read.  If it doesn't exist, the defaults will be written to
    // the file.
    GPConfiguration config (cout, "crawling.ini", configArray);

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
    tout.open (strTeXFile.str().c_str(), ios::out);
    tout << endl
         << "\\documentstyle[a4]{article}" << endl
         << "\\begin{document}" << endl;

    // Print the configuration to the files just opened
    fout << cfg << endl;
    cout << cfg << endl;
    tout << "\\begin{verbatim}\n" << cfg << "\\end{verbatim}\n" << endl;

    // Create the adf function/terminal set and print it out.
    GPAdfNodeSet adfNs;
    createNodeSet (adfNs);
    cout << adfNs << endl;
    fout << adfNs << endl;

    // Create a population with this configuration
    cout << "Creating initial population ..." << endl;
    MyPopulation* pop=new MyPopulation (cfg, adfNs);
    pop->create ();
    cout << "Ok." << endl;
    pop->createGenerationReport (1, 0, fout, bout);

    // Print the best of generation to the LaTeX-file.
    printTexStyle=1;
    tout << *pop->NthGP (pop->bestOfPopulation);
    printTexStyle=0;

    // This next for statement is the actual genetic programming system
    // which is in essence just repeated reproduction and crossover loop
    // through all the generations .....
    MyPopulation* newPop=NULL;
    for (int gen=1; gen<=cfg.NumberOfGenerations; gen++) {

        // Create a new generation from the old one by applying the
        // genetic operators
        if (!cfg.SteadyState) {
            newPop=new MyPopulation (cfg, adfNs);
        }
        pop->generate (*newPop);

        // Delete the old generation and make the new the old one
        if (!cfg.SteadyState) {
            delete pop;
            pop=newPop;
        }

        // Print the best of generation to the LaTeX-file.
        printTexStyle=1;
        tout << "Generation " << gen << ", fitness "
             << pop->NthGP (pop->bestOfPopulation)->getFitness()
             << endl;
        tout << *pop->NthGP (pop->bestOfPopulation);
        printTexStyle=0;

        // Create a report of this generation and how well it is doing
        pop->createGenerationReport (0, gen, fout, bout);
    }

    // TeX-file: end of document
    tout << endl
         << "\\end{document}"
         << endl;
    tout.close ();

    cout << "\nResults are in "
         << InfoFileName << ".dat,"
         << InfoFileName << ".tex,"
         << InfoFileName << ".stc." << endl;

    cout << endl << *pop->NthGP(pop->bestOfPopulation) << endl;

    cout << "============== Starting Test ===============" << endl;

    RandomScorer s_random;
    AgeScorer s_age;
    ChangeRateScorer s_change_rate;
    ChangeProbScorer s_change_prob;
    ChangeProbAgeScorer s_change_prob_age;
    GPScorer s_gp((MyGP*)pop->NthGP(pop->bestOfPopulation));

    CrawlSimulation simulator(&test_set);

    cout << "============ Average Error Rate =============" << endl;

    simulator.Run(&s_random, resources, warm_up);
    std::vector<double> error_random = simulator.ErrorRates();
    cout << "random:          " << simulator.AverageErrorRate() << endl;

    simulator.Run(&s_age, resources, warm_up);
    std::vector<double> error_age = simulator.ErrorRates();
    cout << "age:             " << simulator.AverageErrorRate() << endl;

    simulator.Run(&s_change_rate, resources, warm_up);
    std::vector<double> error_change_rate = simulator.ErrorRates();
    cout << "change_rate:     " << simulator.AverageErrorRate() << endl;

    simulator.Run(&s_change_prob, resources, warm_up);
    std::vector<double> error_change_prob = simulator.ErrorRates();
    cout << "change_prob:     " << simulator.AverageErrorRate() << endl;

    simulator.Run(&s_change_prob_age, resources, warm_up);
    std::vector<double> error_change_prob_age = simulator.ErrorRates();
    cout << "change_prob_age: " << simulator.AverageErrorRate() << endl;

    simulator.Run(&s_gp, resources, warm_up);
    std::vector<double> error_gp = simulator.ErrorRates();
    cout << "best_gp:         " << simulator.AverageErrorRate() << endl;

    cout << "============= Error Rate per Cycle =============" << endl;

    cout << "random; age; change_rate; change_prob; change_prob_age; best_gp;" << endl;

    for(int i=0; i < test_set.NumCycles()-warm_up; ++i) {
        cout << error_random[i] << "; ";
        cout << error_age[i] << "; ";
        cout << error_change_rate[i] << "; ";
        cout << error_change_prob[i] << "; ";
        cout << error_change_prob_age[i] << "; ";
        cout << error_gp[i] << "; ";
        cout << endl;
    }

    return 0;
}
