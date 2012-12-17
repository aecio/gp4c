#ifndef CRAWLING_H
#define CRAWLING_H

#include <vector>
#include <iostream>
#include <string>
#include <cassert>
#include <cmath>

class MyGP;
class Instance;


class MyGene : public GPGene {
public:
  MyGene (GPNode& gpo) : GPGene (gpo) {}

  MyGene (const MyGene& gpo) : GPGene (gpo) { }
  virtual GPObject& duplicate () { return *(new MyGene(*this)); }
  virtual GPGene* createChild (GPNode& gpo) {
    return new MyGene (gpo); }

  virtual void printOn (ostream& os);
  void printMathStyle (ostream& os, int lastPrecedence=0);
  void printTeXStyle (ostream& os, int lastPrecedence=0);

  MyGene* NthMyChild (int n) {
    return (MyGene*) GPContainer::Nth (n); }

  double evaluate(Instance& url, int cycle);
};


class MyGP : public GP {
public:
  MyGP (int genes) : GP (genes) {}

  MyGP (MyGP& gpo) : GP (gpo) { }
  virtual GPObject& duplicate () { return *(new MyGP(*this)); }
  virtual GPGene* createGene (GPNode& gpo) {
    return new MyGene (gpo); }

  virtual void printOn (ostream& os);

  MyGene* NthMyGene (int n) {
    return (MyGene*) GPContainer::Nth (n); }
  virtual void evaluate ();
};


class MyPopulation : public GPPopulation
{
public:
  MyPopulation (GPVariables& GPVar_, GPAdfNodeSet& adfNs_) :
    GPPopulation (GPVar_, adfNs_) {}

  MyPopulation (MyPopulation& gpo) : GPPopulation(gpo) {}
  virtual GPObject& duplicate () { return *(new MyPopulation(*this)); }

  // Don't check for ultimate diversity as it takes a long time.
  // Accept every created GP
  virtual int checkForValidCreation (GP&) { return 1; }

  virtual GP* createGP (int numOfGenes) { return new MyGP (numOfGenes); }
};


#endif CRAWLING_H
