#ifndef BEST_GP_H
#define BEST_GP_H

#include <queue>
#include <set>
#include <limits>

#include <unistd.h>
#include "boost/threadpool.hpp"

#include "genetic.h"
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

    MyGP* SelectGP(Dataset* validation_set, ostream& top_out) {

        best_gp_ = NULL;
        double min_score = std::numeric_limits<double>::max();

        std::vector<MyGP*> gps;
        gps.reserve(top_gps_.size());

        // start one thread per processor
        boost::threadpool::pool tp(sysconf(_SC_NPROCESSORS_ONLN));

        // loop through all GPs and start their validation in parallel
        while(!top_gps_.empty()) {
            MyGP* current = top_gps_.top();
            gps.push_back(current);
            top_gps_.pop();

            current->set_validation_set(validation_set);
            tp.schedule( boost::bind(&MyGP::RunValidation, current) );
        }
        tp.wait();



        for(int i =0; i < gps.size(); ++i) {
//        while(!top_gps_.empty()) {

            MyGP* gp = gps[i];

//            gp->RunValidation(validation_set);

            double score = gp->fitness_e + gp->fitness_v + gp->cycles_std_dev;
//            double score = gp->fitness_e + gp->fitness_v + gp->fitness_std_dev;

            top_out << "evolution:" << gp->fitness_e <<
                       " validation:" << gp->fitness_v <<
                       " cycles_std_dev:" << gp->cycles_std_dev <<
                       " fitness_std_dev: "<< gp->fitness_std_dev <<
                       " score:" << score <<
                       " " << *gp;

            if(score < min_score) {
                min_score = score;
                best_gp_ = (MyGP*) &gp->duplicate();
            }

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
