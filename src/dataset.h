#ifndef DATASET_H
#define DATASET_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

class Dataset {
public:

    class Instance {
    public:
        Instance() { }
        Instance(const std::string& changes): changes_(changes) { }

        bool ChangedIn(int cycle) const {
            if(changes_[cycle-1] == '1')
                return true;
            else
                return false;
        }

        bool ChangedIn(int last_visit, int cycle) const {
            for(int i=last_visit; i < cycle; ++i) {
                if(changes_[i] == '1') {
                    return true;
                }
            }
            return false;
        }

        int NumCycles() const {
            return changes_.size();
        }

        const std::string& changes() const {
            return changes_;
        }

    private:
        std::string changes_;
    };

    Dataset(): idcg_ready_(false) { }

    const Instance& instance(int i) const {
        return instances_[i];
    }

    void ComputeIDCG();
    double IDCG(int cycle);

    int PagesChanged(int cycle) {
        return pages_changed_[cycle];
    }

    int NumInstances() const {
        return instances_.size();
    }

    int NumCycles() const {
        return instances_[0].NumCycles();
    }

    void Add(const Instance& instance) {
        instances_.push_back(instance);
    }

    void Randomize() {
        std::random_shuffle(instances_.begin(), instances_.end());
    }

    Dataset testCV(int num_folds, int current_fold) const;

    Dataset trainCV(int num_folds, int current_fold) const;

    Dataset timeSplit(int num_splits, int split_index) const;

    void Print() const {
        for (int i = 0; i < instances_.size(); ++i) {
            std::cout << instances_[i].changes() << std::endl;
        }
    }

    void Clear() {
        instances_.clear();
        idcg_.clear();
        idcg_ready_ = false;
    }

private:

    void CopyInstances(int from, Dataset& dest, int num) const;
    std::vector<Instance> instances_;
    std::vector<double> idcg_;
    std::vector<int> pages_changed_;
    bool idcg_ready_;
};

class DataArchive {
public:
    enum {PARTITION_URL, PARTITION_CYCLE};

    DataArchive() { }

    DataArchive(const std::string& filename) {
        Init(filename);
    }

    void Init(const std::string& filename);

    void ReadFile(const std::string& filename);

    const Dataset& dataset() const {
        return dataset_;
    }

private:
    Dataset dataset_;
};

#endif // DATASET_H
