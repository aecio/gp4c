#ifndef CRAWLING_H
#define CRAWLING_H

#include <vector>
#include <iostream>
#include <string>
#include <cassert>
#include <cmath>

class MyGP;
class URL;
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
    MyGP (int genes, Dataset* train_set, int resources, int warm_up):
        GP(genes),
        train_set_(train_set),
        resources_(resources),
        warm_up_(warm_up) { }

    MyGP (MyGP& gpo) :
        GP(gpo),
        train_set_(gpo.train_set_),
        resources_(gpo.resources_),
        warm_up_(gpo.warm_up_) { }

    virtual GPObject& duplicate() {
        return *(new MyGP(*this));
    }

    virtual GPGene* createGene(GPNode& gpo) {
        return new MyGene (gpo);
    }

    virtual void printOn(ostream& os);

    MyGene* NthMyGene (int n) {
        return (MyGene*) GPContainer::Nth(n);
    }

    virtual void evaluate();

private:
    Dataset* train_set_;
    int resources_, warm_up_;
};


class MyPopulation : public GPPopulation {
public:
    MyPopulation(GPVariables& GPVar_, GPAdfNodeSet& adfNs_,
                 Dataset* train_set, int resources, int warm_up) :
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
    int resources_, warm_up_;
};


#endif // CRAWLING_H
