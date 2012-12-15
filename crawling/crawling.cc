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

#include "scorer.h"
#include "crawling.h"
#include "crawl_simulation.h"

typedef std::ostringstream ostrstream;

// Defines the dataset
WebArchiveDataset dataset;
int resources; // Resources in functions of URL that can be crawled

// The TeX-file
ofstream tout;
int printTexStyle=0;

// Define configuration parameters and the neccessary array to
// read/write the configuration to a file. If you need more variables,
// just add them below and insert an entry in the configArray.
GPVariables cfg;
char *InfoFileName="data";
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
    int precedence;

    // Function or terminal?
    if (isFunction())
    {
        // Determine operator priority
        switch (node->value ()) {
        case '*':
        case '%':
            precedence=1;
            break;
        case '+':
        case '-':
        case 'l':
        case 'e':
            precedence=0;
            break;
        default:
            GPExitSystem ("MyGene::printMathStyle", "Undefined function value");
        }

        // Do we need brackets?
        if (lastPrecedence > precedence)
            os << "(";

        // Print out the operator and the parameters
        switch (node->value ())
        {
        case '*':
            NthMyChild(0)->printMathStyle (os, precedence);
            os << "*";
            NthMyChild(1)->printMathStyle (os, precedence);
            break;
        case '+':
            NthMyChild(0)->printMathStyle (os, precedence);
            os << "+";
            NthMyChild(1)->printMathStyle (os, precedence);
            break;
        case '-':
            NthMyChild(0)->printMathStyle (os, precedence);
            os << "-";
            NthMyChild(1)->printMathStyle (os, precedence);
            break;
        case '%':
            NthMyChild(0)->printMathStyle (os, precedence);
            os << "%";
            NthMyChild(1)->printMathStyle (os, precedence);
            break;
        case 'l':
            os << "log(";
            NthMyChild(0)->printMathStyle (os, precedence);
            os << ")";
            break;
        case 'e':
            os << "exp(";
            NthMyChild(0)->printMathStyle (os, precedence);
            os << ")";
            break;
        default:
            GPExitSystem ("MyGene::printMathStyle",
                          "Undefined function value");
        }

        // Do we need brackets?
        if (lastPrecedence > precedence)
            os << ")";
    }

    // Print the terminal
    if (isTerminal ())
        os << *node;
}



// Print out a gene in LaTeX-style. Don't be confused, I don't make a
// difference whether this gene is the main program or an ADF, I
// assume the internal structure is correct.
void MyGene::printTeXStyle (ostream& os, int lastPrecedence)
{
    int precedence=0;

    // Function or terminal?
    if (isFunction ())
    {
        // Determine operator priority
        switch (node->value())
        {
        case '*':
        case '%':
            precedence=2;
            break;
        case '+':
        case '-':
            precedence=1;
            break;
        case 'l':
        case 'e':
            precedence=0;
            break;
        default:
            GPExitSystem ("MyGene::printTeXStyle",
                          "Undefined function value 1");
        }

        // Do we need brackets?
        if (lastPrecedence>precedence)
            os << "\\left(";

        // Print out the operator and the parameters
        switch (node->value())
        {
        case '*':
            NthMyChild(0)->printTeXStyle (os, precedence);
            os << " ";
            NthMyChild(1)->printTeXStyle (os, precedence);
            break;
        case '+':
            NthMyChild(0)->printTeXStyle (os, precedence);
            os << "+";
            NthMyChild(1)->printTeXStyle (os, precedence);
            break;
        case '-':
            NthMyChild(0)->printTeXStyle (os, precedence);
            os << "-";
            NthMyChild(1)->printTeXStyle (os, precedence);
            break;
        case '%':
            // As we use \frac, we start again with precedence 0
            os << "\\frac{";
            NthMyChild(0)->printTeXStyle (os, 0);
            os << "}{";
            NthMyChild(1)->printTeXStyle (os, 0);
            os << "}";
            break;
        case 'l':
            os << "\log(";
            NthMyChild(0)->printTeXStyle (os, precedence);
            os << ")";
            break;
        case 'e':
            os << "e^{";
            NthMyChild(0)->printTeXStyle (os, precedence);
            os << "}";
            break;
        default:
            GPExitSystem ("MyGene::printTeXStyle",
                          "Undefined function value 2");
        }

        // Do we need brackets?
        if (lastPrecedence>precedence)
            os << "\\right)";
    }

    // We can't let the terminal print itself, because we want to modify
    // it a little bit
    if (isTerminal ())
        switch (node->value ())
        {
        case 'a':
            os << "age";
            break;
        case 'v':
            os << "visits";
            break;
        case 'c':
            os << "changes";
            break;
        case '1':
            os << "1";
            break;
        default:
            GPExitSystem ("MyGene::printTeXStyle",
                          "Undefined terminal value");
        }
}



// Print a Gene.
void MyGene::printOn (ostream& os)
{
    if (printTexStyle)
        printTeXStyle (os);
    else
        printMathStyle (os);
}



// Print a GP. If we want a LaTeX-output, we must provide for the
// equation environment, otherwise we simply call the print function
// of our base class.
void MyGP::printOn (ostream& os)
{
    // If we use LaTeX-style, we provide here for the right equation
    // overhead used for LaTeX.
    if (printTexStyle)
    {
        tout << "\\begin{eqnarray}" << endl;

        // Print all ADF's, if any
        GPGene* current;
        for (int n=0; n<containerSize(); n++)
        {
            if (n!=0)
                os << "\\\\" << endl;
            os << "f_" << n+1 << " & = & ";
            if ((current=NthGene (n)))
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



// This function is the divide with closure. Basically if you divide
// anything by zero you get an error so we have to stop this
// process. We check for a very small denominator and return a very
// high value.
inline double divide (double x, double y) {
    if (fabs (y)<1e-6)
    {
        if (x*y<0.0)
            return -1e6;
        else
            return 1e6;
    }
    else
        return x/y;
}

inline double f_rlog (double x) {
    double arg = 0.0;
    arg = fabs ( x );
    if ( arg <= 1.0)
        return 0.0;
    else
        return log ( arg );
}

// We have the freedom to define this function in any way we like. In
// this case, it takes the parameter x that represents the terminal X,
// and returns the value of the expression. It's recursive of course.
double MyGene::evaluate(Instance& url, int cycle, MyGP& gp)
{
    double ret;

    if (isFunction ())
        switch (node->value ())
        {
        case '*':
            ret=NthMyChild(0)->evaluate (url, cycle, gp)
                    * NthMyChild(1)->evaluate (url, cycle, gp);
            break;
        case '+':
            ret=NthMyChild(0)->evaluate (url, cycle, gp)
                    + NthMyChild(1)->evaluate (url, cycle, gp);
            break;
        case '-':
            ret = NthMyChild(0)->evaluate (url, cycle, gp)
                    - NthMyChild(1)->evaluate (url, cycle, gp);
            break;
        case '%':
            // We use the function divide rather than "/" to ensure the
            // closure property
            ret = divide(NthMyChild(0)->evaluate (url, cycle, gp),
                         NthMyChild(1)->evaluate (url, cycle, gp));
            break;
        case 'l':
            ret = f_rlog(NthMyChild(0)->evaluate (url, cycle, gp));
            break;
        case 'e':
            ret = exp(NthMyChild(0)->evaluate (url, cycle, gp));
            break;
        default:
            std::cout << "Value: " << node->value() << std::endl;
            GPExitSystem ("MyGene::evaluate", "Undefined function value");
        }
    if(isTerminal()) {
        switch (node->value()) {
        case 'a':
            ret=url.GetAge(cycle);
            break;
        case 'v':
            ret=url.visits;
            break;
        case 'c':
            ret=url.changes;
            break;
        case '1':
            ret=1.0;
            break;
        default:
            GPExitSystem ("MyGene::evaluate", "Undefined terminal value");
        }
    }
    return ret;
}

// Evaluate the fitness of a GP and save it into the GP class variable
// stdFitness.
void MyGP::evaluate () {

    GPScorer scorer(this);

    CrawlSimulation simulator(&dataset);
    simulator.Run(&scorer, resources);

    double avg_error_rate = simulator.AverageErrorRate();

    // add a slight penalization for long functions
    stdFitness = avg_error_rate + ( length()*0.0001 );
}



// Create function and terminal set
void createNodeSet (GPAdfNodeSet& adfNs)
{
    // Reserve space for the node sets
    adfNs.reserveSpace (1);

    // Now define the function and terminal set for each ADF and place
    // function/terminal sets into overall ADF container
    GPNodeSet& ns0=*new GPNodeSet (10);
    adfNs.put (0, ns0);

    // Define functions/terminals and place them into the appropriate
    // sets. Terminals take two arguments, functions three (the third
    // parameter is the number of arguments the function has)
    ns0.putNode (*new GPNode ('+', "+", 2));
    ns0.putNode (*new GPNode ('-', "-", 2));
    ns0.putNode (*new GPNode ('*', "*", 2));
    ns0.putNode (*new GPNode ('%', "%", 2));
    ns0.putNode (*new GPNode ('l', "log", 1));
    ns0.putNode (*new GPNode ('e', "exp", 1));

    ns0.putNode (*new GPNode ('a', "age"));
    ns0.putNode (*new GPNode ('c', "changes"));
    ns0.putNode (*new GPNode ('v', "visits"));
    ns0.putNode (*new GPNode ('1', "1"));
}



void newHandler () {
    cerr << "\nFatal error: Out of memory." << endl;
    exit (1);
}



int main () {
    // We set up a new-handler, because we might need a lot of memory,
    // and we don't know it's there.
    set_new_handler (newHandler);

    // Set up the dataset and crawling resources
    dataset.Init("datasets/synthetic.all.norm");
    resources = dataset.NumInstances()*0.05;
    cout << "Number URLS dataset: " << dataset.NumInstances() << endl;
    cout << "Crawling resources : " << resources << endl;

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

    cout << "\nResults are in "
         << InfoFileName << ".dat,"
         << InfoFileName << ".tex,"
         << InfoFileName << ".stc." << endl;

    RandomScorer s_random;
    AgeScorer s_age;
    ChangeProbScorer s_change_prob;
    ChangeProbAgeScorer s_change_prob_age;
    GPScorer s_gp((MyGP*)pop->NthGP(pop->bestOfPopulation));

    CrawlSimulation simulator(&dataset);

    cout << "========= Baseline results =========" << endl;

    simulator.Run(&s_random, resources);
    std::vector<double> error_random = simulator.ErrorRates();
    cout << "random:          " << simulator.AverageErrorRate() << endl;

    simulator.Run(&s_age, resources);
    std::vector<double> error_age = simulator.ErrorRates();
    cout << "age:             " << simulator.AverageErrorRate() << endl;

    simulator.Run(&s_change_prob, resources);
    std::vector<double> error_change_prob = simulator.ErrorRates();
    cout << "change_prob:     " << simulator.AverageErrorRate() << endl;

    simulator.Run(&s_change_prob_age, resources);
    std::vector<double> error_change_prob_age = simulator.ErrorRates();
    cout << "change_prob_age: " << simulator.AverageErrorRate() << endl;

    simulator.Run(&s_gp, resources);
    std::vector<double> error_gp = simulator.ErrorRates();
    cout << "best_gp:         " << simulator.AverageErrorRate() << endl;

    cout << "====================================" << endl;

    for(int i=0; i<dataset.NumCycles(); ++i) {
        cout << error_random[i] << "; ";
        cout << error_age[i] << "; ";
        cout << error_change_prob[i] << "; ";
        cout << error_change_prob_age[i] << "; ";
        cout << error_gp[i] << "; ";
        cout << endl;
    }

    return 0;
}
