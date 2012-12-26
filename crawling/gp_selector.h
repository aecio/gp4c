#ifndef BEST_GP_H
#define BEST_GP_H

#include <queue>
#include <set>
#include <limits>

#include "crawling.h"
#include "crawl_simulation.h"

typedef std::priority_queue<MyGP*, vector<MyGP*>, MyGP::Comparator> MyGPQueue;

class GPSelector {
public:

    GPSelector(int max_size): max_size_(max_size), best_gp_(NULL) { }

    ~GPSelector() {
        if(best_gp_) {
            delete best_gp_;
            best_gp_ = NULL;
        }
    }

    void Push(MyGP* gp) {
        ostringstream os;
        os << *gp;
        std::string id = os.str();
        if( ids_.find(id) == ids_.end() ) {
            // add
            if(top_gps_.size() < max_size_) {
                top_gps_.push((MyGP*) &gp->duplicate());
            } else {
                MyGP* top_gp = top_gps_.top();
                if(gp->getFitness() < top_gp->getFitness()) {
                    ids_.erase(id);
                    top_gps_.pop();
                    delete top_gp;

                    top_gps_.push((MyGP*) &gp->duplicate());
                    ids_.insert(id);
                }
            }
        }
    }

    MyGP* SelectGP(Dataset* validation_set, double resources, int warm_up,
                   ostream& top_out) {

        best_gp_ = NULL;
        double min_score = std::numeric_limits<double>::max();

        while(!top_gps_.empty()) {

            MyGP* gp = top_gps_.top();

            double fitness_e = gp->getFitness();
            double fitness_v = CrawlSimulation::Run(gp, validation_set, resources, warm_up);
            double mean = (fitness_e + fitness_v)/2;
            double std_dev = sqrt(pow(fitness_v-mean, 2) + pow(fitness_e-mean, 2));

            double score = fitness_e + fitness_v + std_dev;

            top_out << "evolution:" << fitness_e <<
                       " validation:" << fitness_v <<
                       " std_dev:" << std_dev <<
                       " score:" << score <<
                       " " << *gp;

            if(score < min_score) {
                min_score = score;
                best_gp_ = (MyGP*) &gp->duplicate();
            }

            top_gps_.pop();
            delete gp;
        }

        return best_gp_;
    }

private:
    MyGPQueue top_gps_;
    std::set<std::string> ids_;
    int max_size_;
    MyGP* best_gp_;
};

#endif // BEST_GP_H
