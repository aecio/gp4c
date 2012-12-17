#ifndef DATASET_H
#define DATASET_H

#include <cmath>
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>

class Instance {
public:
    Instance(): id(0), last_visit(0), visits(0), changes(0), score(0) { }
    int id;
    int last_visit;
    int visits;
    int changes;
    double score;

    bool operator<(const Instance &other) const {
        if (this->score > other.score)
            return true;
        else
            return false;
    }

    static bool ComparePtr(Instance* a, Instance* b) {
        if (a->score > b->score)
            return true;
        else
            return false;
    }

    double GetAge(int cycle) {
        return cycle-last_visit;
    }

    double GetChangeRate() {
        return -log((visits - changes+0.5)/(visits+0.5));
    }

    double GetChangeProbability() {
        return 1.0 - exp( - GetChangeRate());
    }

    double GetChangeProbabilityAge(int cycle) {
        return 1.0 - exp( - GetChangeRate() * GetAge(cycle));
    }
};

class WebArchiveDataset {
public:
    enum {TRAIN, TEST};

    WebArchiveDataset() {}
    WebArchiveDataset(const std::string& filename) {
        Init(filename);
    }

    void Init(const std::string& filename) {
        std::cout << "Loading UCLA WebArchive dataset..." << std::endl;
        dataset_.clear();
        dataset_.reserve(300000);
        std::string changes;
        int url_id, page_id;
        std::ifstream file(filename.c_str());
        file >> url_id >> page_id >> changes;
        while (file.good()) {
            assert(changes.size() > 0);
            if(changes.size() > 0)
                dataset_.push_back(changes);
            file >> url_id >> page_id >> changes;
        }
        dataset_size = dataset_.size();
        dataset_cycles = dataset_[0].size();
//        dataset_size = 50000;
//        dataset_cycles = 60;
        std::cout << "Loaded " << dataset_.size() << " instances "
                  << "from file " << filename << std::endl;
        SetMode(TRAIN);
    }

    void SetMode(int mode) {
        mode_.n_instances = dataset_size;
        mode_.n_cycles = dataset_cycles / 2;
        if(mode == TRAIN) {
            mode_.first_cycle = 0;
            std::cout << "Dataset in mode: TRAIN" << std::endl;
        } else if(mode == TEST) {
            mode_.first_cycle = dataset_cycles / 2;
            std::cout << "Dataset in mode: TEST" << std::endl;
        } else {
            std::cerr << "You tried to set an unknown mode." << std::endl;
        }
        std::cout << "cycles:      " << mode_.n_cycles << std::endl;
        std::cout << "intances:    "   << mode_.n_instances << std::endl;
        std::cout << "cycles range: [" << mode_.first_cycle << ", "
                  << mode_.first_cycle+mode_.n_instances << "]" << std::endl;
    }

    bool ChangedIn(int url, int cycle) {
        cycle += mode_.first_cycle;
        if(dataset_[url][cycle-1] == '1')
            return true;
        else
            return false;
    }

    int NumCycles() {
        return mode_.n_cycles;
//        return dataset_[0].size();
//        return 30;
    }

    int NumInstances() {
        return mode_.n_instances;
//        return 60000;
//        return dataset_.size();
    }

    std::vector<std::string> dataset_;
private:
    struct mode_t {
        int n_instances;
        int n_cycles;
        int first_cycle;
    };
    int dataset_size;
    int dataset_cycles;
    mode_t mode_;
};

#endif // DATASET_H
