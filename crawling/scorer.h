#ifndef SCORER_H
#define SCORER_H

#include <cstdlib>

#include "url.h"
#include "crawling.h"

class Scorer {
  public:
    virtual double Score(URL& intance, int cycle) = 0;
    virtual const std::string Name() = 0;
};

class GPScorer : public Scorer {
public:
    GPScorer(MyGP* gp): gp_(gp) { }
    inline double Score(URL& url, int cycle) {
        return gp_->NthMyGene(0)->evaluate(url, cycle);
    }
    const std::string Name() {
        return "best_gp";
    }

private:
    MyGP* gp_;
};

class RandomScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return rand() / (double) RAND_MAX;;
    }
    const std::string Name() {
        return "random";
    }
private:
};


class AgeScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetAge(cycle);
    }
    const std::string Name() {
        return "age";
    }
private:
};

class ChangeRateScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetChangeRate();
    }
    const std::string Name() {
        return "change_rate";
    }
private:
};

class ChangeProbScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetChangeProbability(cycle);
    }
    const std::string Name() {
        return "change_prob";
    }
private:
};

class NADChangeRateScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetNADChangeRate(cycle);
    }
    const std::string Name() {
        return "nad";
    }
private:
};

class SADChangeRateScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetSADChangeRate(cycle);
    }
    const std::string Name() {
        return "sad";
    }
private:
};

class AADChangeRateScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetAADChangeRate(cycle);
    }
    const std::string Name() {
        return "aad";
    }
private:
};

class GADChangeRateScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetGADChangeRate(cycle);
    }
    const std::string Name() {
        return "gad";
    }
private:
};

#endif // SCORER_H
