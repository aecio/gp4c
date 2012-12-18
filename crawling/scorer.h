#ifndef SCORER_H
#define SCORER_H

#include <cstdlib>

class Instance;
class MyGP;

class Scorer {
  public:
    virtual double Score(Instance& intance, int cycle) = 0;
};

class GPScorer : public Scorer {
public:
    GPScorer(MyGP* gp): gp_(gp) { }
    inline double Score(Instance& url, int cycle) {
        return gp_->NthMyGene(0)->evaluate(url, cycle);
    }
private:
    MyGP* gp_;
};

class RandomScorer : public Scorer {
public:
    inline double Score(Instance& url, int cycle) {
        return rand() / (double) RAND_MAX;;
    }
private:
};


class AgeScorer : public Scorer {
public:
    inline double Score(Instance& url, int cycle) {
        return url.GetAge(cycle);
    }
private:
};

class ChangeRateScorer : public Scorer {
public:
    inline double Score(Instance& url, int cycle) {
        return url.GetChangeRate();
    }
private:
};

class ChangeProbScorer : public Scorer {
public:
    inline double Score(Instance& url, int cycle) {
        return url.GetChangeProbability();
    }
private:
};

class ChangeProbAgeScorer : public Scorer {
public:
    inline double Score(Instance& url, int cycle) {
        return url.GetChangeProbabilityAge(cycle);
    }
private:
};



#endif // SCORER_H
