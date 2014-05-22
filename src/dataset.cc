#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

#include "dataset.h"

using namespace std;

double Dataset::IDCG(int cycle) {
    #ifndef NDEBUG
    if(!idcg_ready_) {
        abort();
    }
    #endif
    return idcg_[cycle];
}

int comparator_cycle;
bool CycleComparator(const Dataset::Instance& a, const Dataset::Instance& b) {
    return a.ChangedIn(comparator_cycle) > b.ChangedIn(comparator_cycle);
}

void Dataset::ComputeIDCG() {
    std::cout << "Computing IDCG... ";
    std::vector<Instance> data(this->instances_);
    idcg_.resize(this->NumCycles()+1);
    pages_changed_.resize(this->NumCycles()+1);
    for(int cycle = 1; cycle <= this->NumCycles(); cycle++) {
        comparator_cycle = cycle;
        std::sort(data.begin(), data.end(), CycleComparator);
        idcg_[cycle] = 0;

        for (int i = 1; i <= data.size() && data[i-1].ChangedIn(cycle); ++i) {
            pages_changed_[cycle]++;
            if (i == 1) {
                idcg_[cycle] = pow(2, data[i-1].ChangedIn(cycle)) - 1;
            } else {
                idcg_[cycle] += (pow(2, data[i-1].ChangedIn(cycle)) - 1) / (log(i + 1));
            }
//            std::cout << "i="<< i
//                      << " changed=" << data[i-1]->ChangedIn(cycle)
//                      << " idcg[i]=" << idcg_[cycle] << std::endl;
        }
    }

    idcg_ready_ = true;
    std::cout <<"done." << std::endl;
}

Dataset Dataset::testCV(int num_folds, int current_fold) const {

    int offset;
    int fold_size = this->NumInstances() / num_folds;
    if (current_fold < this->NumInstances() % num_folds){
        fold_size++;
        offset = current_fold;
    } else {
        offset = this->NumInstances() % num_folds;
    }

    Dataset test;
    int first = current_fold * (this->NumInstances() / num_folds) + offset;
    CopyInstances(first, test, fold_size);
    cout << "Created cross-validation test set " << current_fold
         << " of " << num_folds << " with "
         << test.NumInstances() << " instances." << endl;
    test.ComputeIDCG();
    return test;
}

Dataset Dataset::trainCV(int num_folds, int current_fold) const {

    int offset;
    int fold_size = this->NumInstances() / num_folds;
    if (current_fold < this->NumInstances() % num_folds) {
      fold_size++;
      offset = current_fold;
    } else {
      offset = this->NumInstances() % num_folds;
    }
    Dataset train;
    int first = current_fold * (this->NumInstances() / num_folds) + offset;

    CopyInstances(0, train, first);
    CopyInstances(first + fold_size,
                  train,
                  this->NumInstances() - first - fold_size);
    cout << "Created cross-validation train set " << current_fold
         << " of " << num_folds << " with "
         << train.NumInstances() << " instances." << endl;
    train.ComputeIDCG();
    return train;
}

Dataset Dataset::timeSplit(int num_folds, int current_fold) const {
    int days_per_split = NumCycles() / num_folds;

    int offset;
    int num_cycles = this->NumCycles();
    int fold_size = num_cycles / num_folds;
    if (current_fold < num_cycles % num_folds) {
      fold_size++;
      offset = current_fold;
    } else {
      offset = num_cycles % num_folds;
    }

    int first_day = current_fold * (num_cycles / num_folds) + offset;
    int last_day = first_day+fold_size-1;

    std::cout << "num_cycles: " << num_cycles
              << " days_per_split: " << days_per_split
              << " fold_size: " << fold_size
              << " first_day: " << first_day
              << " last_day: " << last_day
              << std::endl;
    Dataset new_dataset;
    for (int i = 0; i < instances_.size(); i++) {
        const Instance& instance = instances_[i];
        new_dataset.Add(instance.changes().substr(first_day, fold_size));
    }
    new_dataset.ComputeIDCG();
    return new_dataset;
}

void Dataset::CopyInstances(int from, Dataset& dest, int num) const {
  for (int i = 0; i < num; i++) {
    dest.Add(instance(from + i));
  }
}

void DataArchive::Init(const std::string& filename) {
    std::cout << "Loading data archive..." << std::endl;
    ReadFile(filename);
    dataset_.ComputeIDCG();
    std::cout << "Loaded " << dataset_.NumInstances()
              << " instances and "
              << dataset_.NumCycles() << " cycles "
              << "from file " << filename << std::endl;
}

void DataArchive::ReadFile(const std::string& filename) {

    std::ifstream file(filename.c_str());

    std::vector<std::string> tmp_data;
    if(file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            std::istringstream iss(line);

            //                int host_id, page_id;
            //                iss >> host_id;
            //                iss >> page_id;

            std::string changes;
            iss >> changes;

            assert(changes.size() > 0);


            // Max values
            //if(tmp_data.size() >= 1000) break;       // max instances
            //changes.resize(30);                        // max cycles

            tmp_data.push_back(changes);
        }
        file.close();
    }

    if(tmp_data.size() > 0) {
        dataset_.Clear();
        for (int i = 0; i < tmp_data.size(); ++i) {
            dataset_.Add(tmp_data[i]);
        }
    }
}
