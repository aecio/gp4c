#include <fstream>
#include <sstream>
#include <ostream>

#include "genetic.h"
#include "functions.h"
#include "terminals.h"
#include "scorer.h"


/*========================= MyGene =========================
  ----------------------------------------------------------*/

// Print out a gene in typical math style. Don't be confused, I don't
// make a difference whether this gene is the main program or an ADF,
// I assume the internal structure is correct.
void MyGene::printMathStyle(ostream& os, int lastPrecedence) {

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
    printMathStyle(os);
}

// We have the freedom to define this function in any way we like.
double MyGene::evaluate(URL& url, int cycle) {
    if(isFunction()) {
        return static_cast<Function*>(node)->Value(this, url, cycle);
    } else {
//    if(isTerminal())
        return static_cast<Terminal*>(node)->Value(url, cycle);
    }
}

/*========================== MyGP ==========================
  ----------------------------------------------------------*/

int MyGP::fitness_function = MyGP::CHANGE_RATE;

// Print a GP. If we want a LaTeX-output, we must provide for the
// equation environment.
void MyGP::printTeXStyle(ostream& os) {
    os << "\\begin{eqnarray}" << endl;

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

    os << endl << "\\end{eqnarray}" << endl << endl;
}

void MyGP::RunValidation() {

    CrawlSimulation sim_validation;
    GPScorer scorer(this);
    sim_validation.Run(&scorer, validation_set_, resources_, warm_up_);

    if(sim_validation.ChangeRates().size() == 0) {
        GPExitSystem("MyGP::Validate", "Error rates of validation is empty.");
    }
    if(sim_evolution_.ChangeRates().size() == 0) {
        GPExitSystem("MyGP::Validate", "Error rates of evolutino is empty.");
    }

    int total_rates = 0;
    double sum_v = 0.0;

    const std::vector<double>* rates;

    // Calc mean for validation
    rates = &sim_validation.ChangeRates();
    for (int i = 0; i < rates->size(); ++i) {
        sum_v += (*rates)[i];
    }
    fitness_v = sum_v / rates->size();
    total_rates += rates->size();


    // Calc mean for evolution
    double sum_e = 0.0;
    rates = &sim_evolution_.ChangeRates();
    for (int i = 0; i < rates->size(); ++i) {
        sum_e += (*rates)[i];
    }
    fitness_e = sum_e / rates->size();
    total_rates += rates->size();

//    assert(fitness_e == stdFitness);

    // Calc std deviation for validation
    double mean = (sum_e + sum_v) / total_rates;
    double std_dev = 0.0;
    rates = &sim_validation.ChangeRates();
    for (int i = 0; i < rates->size(); ++i) {
        std_dev += pow((*rates)[i] - mean, 2);
    }

    // Calc std deviation for evolution
    rates = &sim_evolution_.ChangeRates();
    for (int i = 0; i < rates->size(); ++i) {
        std_dev += pow((*rates)[i] - mean, 2);
    }

    std_dev = sqrt(std_dev);

    cycles_std_dev = std_dev;
    fitness_std_dev = sqrt(pow(fitness_v-mean, 2) + pow(fitness_e-mean, 2));
}

// Evaluate the fitness of a GP and save it into the GP class variable
// stdFitness.
void MyGP::evaluate() {
    GPScorer scorer(this);
    sim_evolution_.Run(&scorer, evolution_set_, resources_, warm_up_);
    if(fitness_function == CHANGE_RATE)
        stdFitness = 1 - sim_evolution_.AverageChangeRate();
    else
        stdFitness = 1 - sim_evolution_.AverageNDCG();
}
