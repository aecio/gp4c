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

    GPSelector(int max_size):
        max_size_(max_size), best_gp_sum_(NULL), best_gp_avg_(NULL) { }

    ~GPSelector() {
        if(best_gp_sum_) {
            delete best_gp_sum_;
            best_gp_sum_ = NULL;
        }

        if(best_gp_avg_) {
            delete best_gp_avg_;
            best_gp_avg_ = NULL;
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
//                cout << "fitness=" << top_gp->getFitness()
//                     << " lenght=" << top_gp->length() << endl;
                if(gp->getFitness() < top_gp->getFitness()) {
//                    cout << "new gp fitness=" << gp->getFitness() << endl;
                    ids_.erase(id);
                    top_gps_.pop();
                    delete top_gp;

                    top_gps_.push((MyGP*) &gp->duplicate());
                    ids_.insert(id);
                }
            }
        }
    }

    double SumScore(MyGP* gp) {
        return gp->fitness_e + gp->fitness_v - gp->cycles_std_dev;
    }

    double AvgScore(MyGP* gp) {
        return ((gp->fitness_e + gp->fitness_v)/2) - gp->cycles_std_dev;
    }

    void Validate(Dataset* validation_set, ostream& top_out) {


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

        assert(gps.size() > 0);

        best_gp_sum_ = (MyGP*) &gps[0]->duplicate();
        best_sum_score_ = SumScore(best_gp_sum_);

        best_gp_avg_ = (MyGP*) &gps[0]->duplicate();
        best_avg_score_ = AvgScore(best_gp_avg_);

        std::cout << "Num best gps: " << gps.size() << endl;
        for(int i = 0; i < gps.size(); ++i) {
            MyGP* gp = gps[i];

            double sum_score = SumScore(gp);
            double avg_score = AvgScore(gp);

            top_out << "evolution: " << gp->fitness_e <<
                       " validation: " << gp->fitness_v <<
                       " cycles_std_dev: " << gp->cycles_std_dev <<
                       " fitness_std_dev: "<< gp->fitness_std_dev <<
                       " length: " << gp->length() <<
                       " sum_score: " << sum_score <<
                       " avg_score: " << avg_score <<
                       " " << *gp;
//            cout << "evolution:" << gp->fitness_e <<
//                       " validation:" << gp->fitness_v <<
//                       " cycles_std_dev:" << gp->cycles_std_dev <<
//                       " fitness_std_dev: "<< gp->fitness_std_dev <<
//                       " sum_score:" << sum_score <<
//                       " avg_score:" << avg_score << endl;

//            std::cout << "best_sum_score=" << best_sum_score
//                      << " sum_score=" << sum_score
//                      << std::endl;

            if(sum_score > best_sum_score_ ||
               (sum_score == best_sum_score_ && gp->length() < best_gp_sum_->length()) ) {

                std::cout << "best_sum_score=" << best_sum_score_
                          << " new_sum_score=" << sum_score
                          << std::endl;
                cout << "old:" << *best_gp_sum_;
                cout << "new:" << *gp << endl;

                best_sum_score_ = sum_score;
                delete best_gp_sum_;
                best_gp_sum_ = (MyGP*) &gp->duplicate();
            }
            if(avg_score > best_avg_score_ ||
               (avg_score == best_avg_score_ && gp->length() < best_gp_avg_->length())) {

                std::cout << "best_avg_score: " << best_avg_score_
                          << " new_avg_score: " << avg_score
                          << std::endl;
                cout << "old:" << *best_gp_sum_;
                cout << "new:" << *gp << endl;

                best_avg_score_ = avg_score;
                delete best_gp_avg_;
                best_gp_avg_ = (MyGP*) &gp->duplicate();
            }

            delete gp;
        }

        assert(best_gp_avg_ != NULL);
        assert(best_gp_sum_ != NULL);
    }

    MyGP* BestGPSum() {
        return best_gp_sum_;
    }

    MyGP* BestGPAvg() {
        return best_gp_avg_;
    }

    double best_avg_score() const {
        return best_avg_score_;
    }

    double best_sum_score() const {
        return best_sum_score_;
    }

private:
    MyGPQueue top_gps_;
    std::set<std::string> ids_;
    int max_size_;
    MyGP* best_gp_sum_;
    double best_sum_score_;
    MyGP* best_gp_avg_;
    double best_avg_score_;
};

#endif // BEST_GP_H
