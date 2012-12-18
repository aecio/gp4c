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
    enum {TRAIN, TEST, PARTITION_URL, PARTITION_CYCLE};

    WebArchiveDataset() {}
    WebArchiveDataset(const std::string& filename, int mode, int partition) {
        Init(filename, mode, partition);
    }

    void ReadFile(const std::string& filename) {
        std::string changes;
        int host_id, page_id;
        std::ifstream file(filename.c_str());
        file >> host_id >> page_id >> changes;
        while (file.good()) {
            assert(changes.size() > 0);
            dataset_.push_back(changes);
            file >> host_id >> page_id >> changes;
        }
        file.close();
    }

    void Init(const std::string& filename, int mode, int partition) {
        std::cout << "Loading UCLA WebArchive dataset..." << std::endl;
        dataset_.clear();
        dataset_.reserve(300000);

        ReadFile(filename);

        dataset_size = dataset_.size();
        dataset_cycles = dataset_[0].size();
//        dataset_size = 300000;
//        dataset_cycles = 40;

        std::cout << "Loaded " << dataset_.size() << " instances "
                  << "from file " << filename << std::endl;
        std::cout << "Using: " << std::endl
                  << dataset_size << " instances." << std::endl
                  << dataset_cycles << " cycles." << std::endl;

        SetMode(mode, partition);
    }

    void SetMode(int mode, int partition) {
        if(partition == PARTITION_URL) {
            std::cout << "Dataset partition scheme: URL" << std::endl;
            mode_.first_cycle = 0;
            mode_.n_cycles = dataset_cycles;
            mode_.n_instances = dataset_size / 2;
            if(mode == TRAIN) {
                mode_.first_instance = 0;
                std::cout << "Dataset in mode: TRAIN" << std::endl;
            } else if(mode == TEST) {
                mode_.first_instance = dataset_size / 2;
                std::cout << "Dataset in mode: TEST" << std::endl;
            } else {
                std::cerr << "You tried to set an unknown mode." << std::endl;
            }
        } else if(partition == PARTITION_CYCLE){
            std::cout << "Dataset partition scheme: CYCLE" << std::endl;
            mode_.first_instance = 0;
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
        } else {
            std::cerr << "You tried to set an unknown partition." << std::endl;
        }
        std::cout << "intances_range: ["
                  << mode_.first_instance << ", "
                  << mode_.first_instance+mode_.n_instances<< "]" << std::endl;
        std::cout << "cycles_range: ["
                  << mode_.first_cycle << ", "
                  << mode_.first_cycle+mode_.n_cycles<< "]" << std::endl;
    }

    bool ChangedIn(int instance, int cycle) {
        cycle += mode_.first_cycle;
        instance += mode_.first_instance;
        if(dataset_[instance][cycle-1] == '1')
            return true;
        else
            return false;
    }

    int NumCycles() {
        return mode_.n_cycles;
    }

    int NumInstances() {
        return mode_.n_instances;
    }

    std::vector<std::string> dataset_;
private:
    struct mode_t {
        int n_instances;
        int n_cycles;
        int first_cycle;
        int first_instance;
    };
    int dataset_size;
    int dataset_cycles;
    mode_t mode_;
};

#endif // DATASET_H
