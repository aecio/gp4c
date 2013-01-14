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

        scorers_.push_back(new SADChangeRateScorer());
        scorers_.push_back(new NADChangeRateScorer());
        scorers_.push_back(new AADChangeRateScorer());
        scorers_.push_back(new GADChangeRateScorer());
        scorers_.push_back(new ChoChangeRateScorer());

        scorers_.push_back(new ChoChangeProbScorer());
        scorers_.push_back(new NADChangeProbScorer());
        scorers_.push_back(new SADChangeProbScorer());
        scorers_.push_back(new AADChangeProbScorer());
        scorers_.push_back(new GADChangeProbScorer());

    }

    ~EvaluationReport() {
        for (int i = 0; i < scorers_.size(); ++i) {
            delete scorers_[i];
        }
    }

    void Evaluate(std::vector<Scorer*>& gp_scorers, Dataset* dataset,
                  double resources, int warm_up, ostream& out) {

        int current_fold = results_.size();
        results_.resize(current_fold+1);

        std::vector<CrawlSimulation>& simulators = results_[current_fold];

        // Add GP scorers of this fold
        for (int i = 0; i < gp_scorers.size(); ++i) {
            scorers_.push_back(gp_scorers[i]);
        }
        simulators.resize(scorers_.size());

        // Run simulation
        for (int i = 0; i < scorers_.size(); ++i) {
            simulators[i].Run(scorers_[i], dataset, resources, warm_up);
        }

        // Print results of each cycle
        for (int j = 0; j < scorers_.size(); ++j) {
            out << scorers_[j]->Name() << ";";
        }
        out << endl;
        for(int i = 0; i < dataset->NumCycles()-warm_up; ++i) {
            for (int j = 0; j < scorers_.size(); ++j) {
                out << simulators[j].ErrorRates()[i] << ";";
            }
            out << endl;
        }

        // Print mean change ratio
        for (int j = 0; j < scorers_.size(); ++j) {
            out << scorers_[j]->Name() << ";";
        }
        out << endl;
        for (int j = 0; j < scorers_.size(); ++j) {
            out << simulators[j].AverageErrorRate() << ";";
        }
        out << endl;

        // Remove GP scorers of this fold
        for (int i = 0; i < gp_scorers.size(); ++i) {
            scorers_.pop_back();
        }
    }

    void Sumarize(std::vector<std::string>& names, std::ostream& out) {
        // Print results of each cycle
        for (int j = 0; j < scorers_.size(); ++j) {
            out << scorers_[j]->Name() << ";";
        }
        for (int i = 0; i < names.size(); ++i) {
            out << names[i] << ";";
        }
        out << endl;

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

                out << mean << ";";
            }
            out << endl;
        }

        // Print results of each cycle
        for (int j = 0; j < scorers_.size(); ++j) {
            out << scorers_[j]->Name() << ";";
        }
        for (int i = 0; i < names.size(); ++i) {
            out << names[i] << ";";
        }
        out << endl;


        int n_scorers = results_[0].size();
        for (int scorer = 0; scorer < n_scorers; ++scorer) {

            double sum = 0.0;
            for (int fold = 0; fold < results_.size(); ++fold) {
                sum += results_[fold][scorer].AverageErrorRate();
            }
            double mean = sum / results_.size();

            out << mean << ";";
        }
        out << endl;

    }

private:
    std::vector<Scorer*> scorers_;
    std::vector< std::vector<CrawlSimulation> > results_;
};

#endif // EVALUATION_H
