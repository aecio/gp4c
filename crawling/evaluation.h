#ifndef EVALUATION_H
#define EVALUATION_H

#include <ostream>

#include "crawl_simulation.h"
#include "dataset.h"
#include "scorer.h"

class EvaluationReport {
public:

    EvaluationReport() {
        scorers_.push_back(new RandomScorer());
        scorers_.push_back(new AgeScorer());
        scorers_.push_back(new ChangeRateScorer());
        scorers_.push_back(new ChangeProbScorer());
        scorers_.push_back(new NADChangeRateScorer());
        scorers_.push_back(new SADChangeRateScorer());
        scorers_.push_back(new AADChangeRateScorer());
        scorers_.push_back(new GADChangeRateScorer());
    }

    ~EvaluationReport() {
        for (int i = 0; i < scorers_.size(); ++i) {
            delete scorers_[i];
        }
    }

    void Evaluate(Scorer* scorer, Dataset* dataset,
                  double resources, int warm_up, ostream& out) {

        int current = results_.size();
        results_.resize(current+1);
        std::vector<CrawlSimulation>& simulators = results_[current];

        scorers_.push_back(scorer);
        simulators.resize(scorers_.size());

        // Run simulation
        for (int i = 0; i < scorers_.size(); ++i) {
            simulators[i].Run(scorers_[i], dataset, resources, warm_up);
        }

        // Print results of each cycle
        for (int j = 0; j < scorers_.size(); ++j) {
            out << scorers_[j]->Name() << "; ";
        }
        out << endl;
        for(int i = 0; i < dataset->NumCycles()-warm_up; ++i) {
            for (int j = 0; j < scorers_.size(); ++j) {
                out << simulators[j].ErrorRates()[i] << "; ";
            }
            out << endl;
        }

        // Print mean change ratio
        for (int j = 0; j < scorers_.size(); ++j) {
            out << scorers_[j]->Name() << "; ";
        }
        out << endl;
        for (int j = 0; j < scorers_.size(); ++j) {
            out << simulators[j].AverageErrorRate() << "; ";
        }
        out << endl;

        scorers_.pop_back();
    }

    void Sumarize(std::ostream& out) {
        // Print results of each cycle
        for (int j = 0; j < scorers_.size(); ++j) {
            out << scorers_[j]->Name() << "; ";
        }
        out << "best_gp;" << endl;

        std::vector< std::vector<double> > means;
        means.resize(scorers_.size());

        int n_cycles = results_[0][0].ErrorRates().size();
        for (int cycle = 0; cycle < n_cycles; ++cycle) {
            int n_scorers = results_[0].size();
            for (int scorer = 0; scorer < n_scorers; ++scorer) {

                double sum = 0.0;
                for (int fold = 0; fold < results_.size(); ++fold) {
                    sum += results_[fold][scorer].ErrorRates()[cycle];
                }
                double mean = sum / results_.size();

                out << mean << "; ";
            }
            out << endl;
        }

        // Print results of each cycle
        for (int j = 0; j < scorers_.size(); ++j) {
            out << scorers_[j]->Name() << "; ";
        }
        out << "best_gp;" << endl;


            int n_scorers = results_[0].size();
            for (int scorer = 0; scorer < n_scorers; ++scorer) {

                double sum = 0.0;
                for (int fold = 0; fold < results_.size(); ++fold) {
                    sum += results_[fold][scorer].AverageErrorRate();
                }
                double mean = sum / results_.size();

                out << mean << "; ";
            }
            out << endl;

    }

private:
    std::vector<Scorer*> scorers_;
    std::vector< std::vector<CrawlSimulation> > results_;
};

#endif // EVALUATION_H
