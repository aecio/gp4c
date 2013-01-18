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
        max_size_(max_size), best_gp_sum(NULL), best_gp_avg(NULL) { }

    ~GPSelector() {
        if(best_gp_sum) {
            delete best_gp_sum;
            best_gp_sum = NULL;
        }

        if(best_gp_avg) {
            delete best_gp_avg;
            best_gp_avg = NULL;
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
                if(gp->getFitness() > top_gp->getFitness()) {
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

        best_gp_sum = (MyGP*) &gps[0]->duplicate();
        double best_sum_score = SumScore(best_gp_sum);

        best_gp_avg = (MyGP*) &gps[0]->duplicate();
        double best_avg_score = AvgScore(best_gp_avg);

        std::cout << "Num best gps: " << gps.size() << endl;
        for(int i = 0; i < gps.size(); ++i) {
            MyGP* gp = gps[i];

            double sum_score = SumScore(gp);
            double avg_score = AvgScore(gp);

            top_out << "evolution:" << gp->fitness_e <<
                       " validation:" << gp->fitness_v <<
                       " cycles_std_dev:" << gp->cycles_std_dev <<
                       " fitness_std_dev: "<< gp->fitness_std_dev <<
                       " sum_score:" << sum_score <<
                       " avg_score:" << avg_score <<
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

            if(sum_score > best_sum_score ||
               (sum_score == best_sum_score && gp->length() < best_gp_sum->length()) ) {

                std::cout << "best_sum_score=" << best_sum_score
                          << " new_sum_score=" << sum_score
                          << std::endl;
                cout << "old:" << *best_gp_sum;
                cout << "new:" << *gp << endl;

                best_sum_score = sum_score;
                delete best_gp_sum;
                best_gp_sum = (MyGP*) &gp->duplicate();
            }
            if(avg_score > best_avg_score ||
               (avg_score == best_avg_score && gp->length() < best_gp_avg->length())) {

                std::cout << "best_avg_score: " << best_avg_score
                          << " new_avg_score: " << avg_score
                          << std::endl;
                cout << "old:" << *best_gp_sum;
                cout << "new:" << *gp << endl;

                best_avg_score = avg_score;
                delete best_gp_avg;
                best_gp_avg = (MyGP*) &gp->duplicate();
            }

            delete gp;
        }

        assert(best_gp_avg != NULL);
        assert(best_gp_sum != NULL);
    }

    MyGP* BestGPSum() {
        return best_gp_sum;
    }

    MyGP* BestGPAvg() {
        return best_gp_avg;
    }

private:
    MyGPQueue top_gps_;
    std::set<std::string> ids_;
    int max_size_;
    MyGP* best_gp_sum;
    MyGP* best_gp_avg;
};

#endif // BEST_GP_H
