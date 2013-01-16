#ifndef DATASET_H
#define DATASET_H

#include <algorithm>
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

        int NumCycles() const {
            return changes_.size();
        }

    private:
        std::string changes_;
    };

    Dataset(): idcg_ready_(false) { }

    const Instance* instance(int i) const {
        return instances_[i];
    }

    void ComputeIDCG();
    double IDCG(int cycle);

    int NumInstances() const {
        return instances_.size();
    }

    int NumCycles() const {
        return instances_[0]->NumCycles();
    }

    void Add(const Instance* instance) {
        instances_.push_back(instance);
    }

    void Randomize() {
        std::random_shuffle(instances_.begin(), instances_.end());
    }

    Dataset testCV(int num_folds, int current_fold);

    Dataset trainCV(int num_folds, int current_fold);

private:

    void CopyInstances(int from, Dataset& dest, int num);
    std::vector<const Instance*> instances_;
    std::vector<double> idcg_;
    std::vector<int> pages_changed;
    bool idcg_ready_;
};

class DataArchive {
public:
    enum {PARTITION_URL, PARTITION_CYCLE};

    DataArchive(): data_(NULL) { }

    DataArchive(const std::string& filename): data_(NULL) {
        Init(filename);
    }

    ~DataArchive() {
        if(data_) {
            delete[] data_;
        }
    }

    void Init(const std::string& filename);

    void ReadFile(const std::string& filename);

    Dataset& dataset() {
        return dataset_;
    }

private:
    Dataset dataset_;
    Dataset::Instance* data_;
};

#endif // DATASET_H
