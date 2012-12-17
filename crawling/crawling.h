#ifndef CRAWLING_H
#define CRAWLING_H

#include <vector>
#include <iostream>
#include <string>
#include <cassert>
#include <cmath>

class MyGP;

class Instance {
public:
    Instance(): id(0), last_visit(0), visits(0), changes(0), score(0) { }
    int id;
    int last_visit;
    int visits;
    int changes;
    double score;

    bool operator<(const Instance &other) const {
        if (this->score > other.score)
            return true;
        else
            return false;
    }

    static bool ComparePtr(Instance* a, Instance* b) {
        if (a->score > b->score)
            return true;
        else
            return false;
    }

    double GetAge(int cycle) {
        return cycle-last_visit;
    }

    double GetChangeRate() {
        return -log((visits - changes+0.5)/(visits+0.5));
    }

    double GetChangeProbability() {
        return 1.0 - exp( - GetChangeRate());
    }

    double GetChangeProbabilityAge(int cycle) {
        return 1.0 - exp( - GetChangeRate() * GetAge(cycle));
    }
};

class WebArchiveDataset {
public:
    WebArchiveDataset() {}
    WebArchiveDataset(const std::string& filename) {
        Init(filename);
    }

    void Init(const std::string& filename) {
        std::cout << "Loading UCLA WebArchive dataset..." << std::endl;
        dataset_.clear();
        dataset_.reserve(300000);
        std::string changes;
        int url_id, page_id;
        std::ifstream file(filename.c_str());
        file >> url_id >> page_id >> changes;
        while (file.good()) {
            assert(changes.size() > 0);
            if(changes.size() > 0)
                dataset_.push_back(changes);
            file >> url_id >> page_id >> changes;
        }
        std::cout << "Loaded " << dataset_.size() << " instances "
                  << "from file " << filename << std::endl;
    }

    bool ChangedIn(int url, int cycle) {
        if(dataset_[url][cycle-1] == '1')
            return true;
        else
            return false;
    }

    int NumCycles() {
//        return dataset_[0].size();
        return 30;
    }

    int NumInstances() {
        return 30000;
//        return dataset_.size();
    }

    std::vector<std::string> dataset_;
};


class MyGene : public GPGene
{
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



class MyGP : public GP
{
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
