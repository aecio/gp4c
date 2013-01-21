#ifndef EVALUATION_H
#define EVALUATION_H

#include <ostream>
#include <sstream>

#include "crawl_simulation.h"
#include "dataset.h"
#include "scorer.h"


class EvaluationReport {
public:

    EvaluationReport(const std::string info_file_name):
        info_file_name_(info_file_name) { }

    void Evaluate(std::vector<Scorer*>& gp_scorers, Dataset* dataset,
                  double resources, int warm_up, int fold) {

        ostringstream changerate_filename;
        changerate_filename << info_file_name_ << ".changerate.fold" << fold << ends;
        ofstream crate_out(changerate_filename.str().c_str());

        ostringstream ndcg_filename;
        ndcg_filename << info_file_name_ << ".ndcg.fold" << fold << ends;
        ofstream ndcg_out(ndcg_filename.str().c_str());

        ostringstream ncg_filename;
        ncg_filename << info_file_name_ << ".ncg.fold" << fold << ends;
        ofstream ncg_out(ncg_filename.str().c_str());

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
            crate_out << scorers_[j]->Name() << ";";
            ndcg_out << scorers_[j]->Name() << ";";
            ncg_out << scorers_[j]->Name() << ";";
        }
        crate_out << endl;
        ndcg_out << endl;
        ncg_out << endl;
        for(int i = 0; i < dataset->NumCycles()-warm_up; ++i) {
            for (int j = 0; j < scorers_.size(); ++j) {
                crate_out << simulators[j].ChangeRates()[i] << ";";
                ndcg_out << simulators[j].NDCGs()[i] << ";";
                ncg_out << simulators[j].NDCGs()[i] << ";";
            }
            crate_out << endl;
            ndcg_out << endl;
            ncg_out << endl;
        }

        // Print mean change ratio
        for (int j = 0; j < scorers_.size(); ++j) {
            crate_out << scorers_[j]->Name() << ";";
            ndcg_out << scorers_[j]->Name() << ";";
            ncg_out << scorers_[j]->Name() << ";";
        }
        crate_out << endl;
        ndcg_out << endl;
        ncg_out << endl;
        for (int j = 0; j < scorers_.size(); ++j) {
            crate_out << simulators[j].AverageChangeRate() << ";";
            ndcg_out << simulators[j].AverageNDCG() << ";";
            ncg_out << simulators[j].AverageNCG() << ";";
        }
        crate_out << endl;
        ndcg_out << endl;
        ncg_out << endl;

        // Remove GP scorers of this fold
        for (int i = 0; i < gp_scorers.size(); ++i) {
            scorers_.pop_back();
        }

        crate_out.close();
        ndcg_out.close();
        ncg_out.close();
    }

    void Sumarize(std::vector<std::string>& names) {

        ostringstream changerate_filename;
        changerate_filename << info_file_name_ << ".changerate.mean" << ends;
        ofstream crate_out(changerate_filename.str().c_str());

        ostringstream ndcg_filename;
        ndcg_filename << info_file_name_ << ".ndcg.mean" << ends;
        ofstream ndcg_out(ndcg_filename.str().c_str());

        ostringstream ncg_filename;
        ncg_filename << info_file_name_ << ".ncg.mean" << ends;
        ofstream ncg_out(ncg_filename.str().c_str());

        // Print results of each cycle
        for (int j = 0; j < scorers_.size(); ++j) {
            crate_out << scorers_[j]->Name() << ";";
            ndcg_out << scorers_[j]->Name() << ";";
            ncg_out << scorers_[j]->Name() << ";";
        }
        for (int i = 0; i < names.size(); ++i) {
            crate_out << names[i] << ";";
            ndcg_out << names[i] << ";";
            ncg_out << names[i] << ";";
        }
        crate_out << endl;
        ndcg_out << endl;
        ncg_out << endl;

        std::vector< std::vector<double> > means;
        means.resize(scorers_.size());

        int n_cycles = results_[0][0].ChangeRates().size();
        for (int cycle = 0; cycle < n_cycles; ++cycle) {
            int n_scorers = results_[0].size();
            for (int scorer = 0; scorer < n_scorers; ++scorer) {
                double sum;
                double mean;

                sum = 0.0;
                for (int fold = 0; fold < results_.size(); ++fold) {
                    sum += results_[fold][scorer].ChangeRates()[cycle];
                }
                mean = sum / results_.size();
                crate_out << mean << ";";

                sum = 0.0;
                for (int fold = 0; fold < results_.size(); ++fold) {
                    sum += results_[fold][scorer].NDCGs()[cycle];
                }
                mean = sum / results_.size();
                ndcg_out << mean << ";";

                sum = 0.0;
                for (int fold = 0; fold < results_.size(); ++fold) {
                    sum += results_[fold][scorer].NCGs()[cycle];
                }
                mean = sum / results_.size();
                ncg_out << mean << ";";
            }
            crate_out << endl;
            ndcg_out << endl;
            ncg_out << endl;
        }

        // Print results of each cycle
        for (int j = 0; j < scorers_.size(); ++j) {
            crate_out << scorers_[j]->Name() << ";";
            ndcg_out << scorers_[j]->Name() << ";";
            ncg_out << scorers_[j]->Name() << ";";
        }
        for (int i = 0; i < names.size(); ++i) {
            crate_out << names[i] << ";";
            ndcg_out << names[i] << ";";
            ncg_out << names[i] << ";";
        }
        crate_out << endl;
        ndcg_out << endl;
        ncg_out << endl;


        int n_scorers = results_[0].size();
        for (int scorer = 0; scorer < n_scorers; ++scorer) {
            double sum;
            double mean;

            sum = 0.0;
            for (int fold = 0; fold < results_.size(); ++fold) {
                sum += results_[fold][scorer].AverageChangeRate();
            }
            mean = sum / results_.size();
            crate_out << mean << ";";


            sum = 0.0;
            for (int fold = 0; fold < results_.size(); ++fold) {
                sum += results_[fold][scorer].AverageNDCG();
            }
            mean = sum / results_.size();
            ndcg_out << mean << ";";


            sum = 0.0;
            for (int fold = 0; fold < results_.size(); ++fold) {
                sum += results_[fold][scorer].AverageNCG();
            }
            mean = sum / results_.size();
            ncg_out << mean << ";";
        }
        crate_out << endl;
        ndcg_out << endl;
        ncg_out << endl;

        crate_out.close();
        ndcg_out.close();
        ncg_out.close();
    }

private:
    std::vector<Scorer*> scorers_;
    std::vector< std::vector<CrawlSimulation> > results_;
    std::string info_file_name_;
};

#endif // EVALUATION_H
