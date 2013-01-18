#ifndef GENETIC_H
#define GENETIC_H

#include <vector>
#include <iostream>
#include <string>
#include <cassert>
#include <cmath>

#include "gp.h"
#include "url.h"
#include "crawl_simulation.h"

class Dataset;

class MyGene : public GPGene {
public:

    MyGene (GPNode& gpo) : GPGene (gpo) { }
    MyGene (const MyGene& gpo) : GPGene (gpo) { }

    virtual GPObject& duplicate () {
        return *(new MyGene(*this));
    }

    virtual GPGene* createChild (GPNode& gpo) {
        return new MyGene (gpo);
    }

    virtual void printOn (ostream& os);
    void printMathStyle (ostream& os, int lastPrecedence=0);
    void printTeXStyle (ostream& os, int lastPrecedence=0);

    MyGene* NthMyChild (int n) {
        return (MyGene*) GPContainer::Nth(n);
    }

    double evaluate(URL& url, int cycle);

};


class MyGP : public GP {
public:

    enum Fitness {CHANGE_RATE = 1, NDCG = 2};

    class Comparator {
    public:
        bool operator() (const MyGP* lhs, const MyGP* rhs) const {
            if(lhs->stdFitness > rhs->stdFitness)
                return true;
            else if(lhs->stdFitness == rhs->stdFitness &&
                    lhs->length() < rhs->length() )
                return true;
            else
                return false;
        }
    };

    MyGP (int genes, Dataset* train_set, double resources, int warm_up):
        GP(genes),
        evolution_set_(train_set),
        resources_(resources),
        warm_up_(warm_up) { }

    MyGP (MyGP& gpo) :
        GP(gpo),
        evolution_set_(gpo.evolution_set_),
        resources_(gpo.resources_),
        warm_up_(gpo.warm_up_),
        sim_evolution_(gpo.sim_evolution_){ }

    virtual GPObject& duplicate() {
        return *(new MyGP(*this));
    }

    virtual GPGene* createGene(GPNode& gpo) {
        return new MyGene (gpo);
    }

    void printTeXStyle(ostream& os);

//    virtual void printOn(ostream& os);

    MyGene* NthMyGene (int n) {
        return (MyGene*) GPContainer::Nth(n);
    }

    void set_dataset(Dataset* dataset) {
        evolution_set_ =  dataset;
    }

    virtual void evaluate();

    double GetFitness(CrawlSimulation *simulator, Dataset* dataset);

    void RunValidation();

    void set_validation_set(Dataset* validation_set) {
        validation_set_ = validation_set;
    }

    double fitness_e;
    double fitness_v;
    double cycles_std_dev;
    double fitness_std_dev;

    static int fitness_function;

private:
    Dataset* evolution_set_;
    Dataset* validation_set_;
    double resources_;
    int warm_up_;
    CrawlSimulation sim_evolution_;
};


class MyPopulation : public GPPopulation {
public:
    MyPopulation(GPVariables& GPVar_, GPAdfNodeSet& adfNs_,
                 Dataset* train_set, double resources, int warm_up) :
        GPPopulation(GPVar_, adfNs_),
        train_set_(train_set),
        resources_(resources),
        warm_up_(warm_up) { }

    MyPopulation(MyPopulation& gpo):
        GPPopulation(gpo),
        train_set_(gpo.train_set_),
        resources_(gpo.resources_),
        warm_up_(gpo.warm_up_) {
    }

    virtual GPObject& duplicate() {
        return *(new MyPopulation(*this));
    }

    // Don't check for ultimate diversity as it takes a long time.
    // Accept every created GP
    virtual int checkForValidCreation(GP&) {
        return 1;
    }

    virtual GP* createGP(int numOfGenes) {
        return new MyGP(numOfGenes, train_set_, resources_, warm_up_);
    }

private:
    Dataset* train_set_;
    double resources_;
    int warm_up_;
};


#endif // GENETIC_H
