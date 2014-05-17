#ifndef SCORER_H
#define SCORER_H

#include <cstdlib>

#include "url.h"
#include "genetic.h"

class Scorer {
  public:
    virtual double Score(URL& intance, int cycle) = 0;
    virtual const std::string Name() = 0;
};

class GPScorer : public Scorer {
public:
    GPScorer(MyGP* gp): gp_(gp), name_("best_gp") { }
    GPScorer(MyGP* gp, const char* name): gp_(gp), name_(name) { }

    inline double Score(URL& url, int cycle) {
        return gp_->NthMyGene(0)->evaluate(url, cycle);
    }
    const std::string Name() {
        return name_;
    }
private:
    MyGP* gp_;
    std::string name_;
};

// Basic scorers

class RandomScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return rand() / (double) RAND_MAX;;
    }
    const std::string Name() {
        return "rand";
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

// Change rate scorers

class ChoChangeRateScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetChoChangeRate();
    }
    const std::string Name() {
        return "cho";
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

// Change probability scorers

class ChoChangeProbScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetChangeProbabilityCho(cycle);
    }
    const std::string Name() {
        return "cho_prob";
    }
private:
};

class NADChangeProbScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetChangeProbabilityNAD(cycle);
    }
    const std::string Name() {
        return "nad_prob";
    }
private:
};

class SADChangeProbScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetChangeProbabilitySAD(cycle);
    }
    const std::string Name() {
        return "sad_prob";
    }
private:
};

class AADChangeProbScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetChangeProbabilityAAD(cycle);
    }
    const std::string Name() {
        return "aad_prob";
    }
private:
};

class GADChangeProbScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetChangeProbabilityGAD(cycle);
    }
    const std::string Name() {
        return "gad_prob";
    }
private:
};

//  Windowed change prob scorers

class WNADChangeProbScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetChangeProbabilityWNAD(cycle);
    }
    const std::string Name() {
        return "wnad_prob";
    }
private:
};

class WAADChangeProbScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetChangeProbabilityAAD(cycle);
    }
    const std::string Name() {
        return "waad_prob";
    }
private:
};

class WGADChangeProbScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetChangeProbabilityWGAD(cycle);
    }
    const std::string Name() {
        return "wgad_prob";
    }
private:
};

// Windowed change rate scorers

class WNADChangeRateScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetWindowedNADChangeRate(cycle);
    }
    const std::string Name() {
        return "wnad";
    }
private:
};

class WAADChangeRateScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetWindowedAADChangeRate(cycle);
    }
    const std::string Name() {
        return "waad";
    }
private:
};

class WGADChangeRateScorer : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        return url.GetWindowedGADChangeRate(cycle);
    }
    const std::string Name() {
        return "wgad";
    }
private:
};

class BestChrateScorer1 : public Scorer {
public:
    inline double Score(URL& url, int cycle) {

        //  n          X         t
        //visits_ - changes_   url.last_visit();
        //((cho*(cho*t))+(gad+0.01))
        return ((url.GetChoChangeRate()*(url.GetChoChangeRate()*url.last_visit()))+(url.GetGADChangeRate(cycle)+0.01));
    }
    const std::string Name() {
        return "bestgp_chrate1";
    }
private:
};

class BestChrateScorer2 : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
        //  n          X         t
        //visits_ - changes_   url.last_visit();
	return (log(((exp(url.GetChoChangeRate()))*(url.GetChoChangeRate()*url.last_visit())))-((10*10)-((((10+10))/((url.changes()+url.visits())))*((url.changes()+pow(0.5,0.01))))));
    }
    const std::string Name() {
        return "bestgp_chrate2";
    }
private:
};

class BestChrateScorer3 : public Scorer {
public:
    inline double Score(URL& url, int cycle) {

        //  n          X         t
        //visits_ - changes_   url.last_visit();
	return pow((url.last_visit()*url.GetChoChangeRate()),(((2.71828*url.GetGADChangeRate(cycle))+pow(((1+url.GetGADChangeRate(cycle))),(exp(1))))));
    }
    const std::string Name() {
        return "bestgp_chrate3";
    }
private:
};

class BestChrateScorer4 : public Scorer {
public:
    inline double Score(URL& url, int cycle) {

        //  n          X         t
        //visits_ - changes_   url.last_visit();
	return  (url.GetGADChangeRate(cycle)-((10+0.01)-(pow(url.GetChoChangeRate(),2.71828)*url.last_visit())));
    }
    const std::string Name() {
        return "bestgp_chrate4";
    }
private:
};

class BestChrateScorer5 : public Scorer {
public:
    inline double Score(URL& url, int cycle) {

        //  n          X         t
        //visits_ - changes_   url.last_visit();
	return  pow(((url.last_visit()+url.GetChoChangeRate())),((url.GetChoChangeRate()+0.001)));
    }
    const std::string Name() {
        return "bestgp_chrate5";
    }
private:
};

class BestNDCGScorer1 : public Scorer {
public:
    inline double Score(URL& url, int cycle) {

        //(0.001+(cho%(n%(cho*t))))
        return (0.001 + (url.GetChoChangeRate()/(url.visits()/(url.GetChoChangeRate()*url.last_visit()))));
    }
    const std::string Name() {
        return "bestgp_ndcg1";
    }
private:
};

class BestNDCGScorer2 : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
	return log((url.GetChoChangeRate()*((exp(pow(url.changes(),0.1))+url.last_visit()))));
    }
    const std::string Name() {
        return "bestgp_ndcg2";
    }
private:
};

class BestNDCGScorer3 : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
	return  pow((exp((url.GetGADChangeRate(cycle)*url.last_visit()))),pow(((2.71828+log(url.changes()))),url.last_visit()));
    }
    const std::string Name() {
        return "bestgp_ndcg3";
    }
private:
};

class BestNDCGScorer4 : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
	return  log(exp((url.GetChoChangeRate()-pow((log(2.71828)),(1*url.last_visit())))));
    }
    const std::string Name() {
        return "bestgp_ndcg4";
    }
private:
};

class BestNDCGScorer5 : public Scorer {
public:
    inline double Score(URL& url, int cycle) {
	return  ((((((url.changes()*url.GetChoChangeRate())*url.changes())/100)+url.GetChoChangeRate()))*url.last_visit());
    }
    const std::string Name() {
        return "bestgp_ndcg5";
    }
private:
};


#endif // SCORER_H
