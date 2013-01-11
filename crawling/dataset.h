#ifndef DATASET_H
#define DATASET_H

#include <algorithm>
#include <cmath>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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

    const Instance* instance(int i) const {
        return instances_[i];
    }

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

    Dataset testCV(int num_folds, int current_fold) {

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
        return test;
    }

    Dataset trainCV(int num_folds, int current_fold) {

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
        return train;
    }

private:
    void CopyInstances(int from, Dataset& dest, int num) {
      for (int i = 0; i < num; i++) {
        dest.Add(instance(from + i));
      }
    }

    std::vector<const Instance*> instances_;
};

class WebArchiveDataset {
public:
    enum {PARTITION_URL, PARTITION_CYCLE};

    WebArchiveDataset(): data_(NULL) { }
    WebArchiveDataset(const std::string& filename): data_(NULL) {
        Init(filename);
    }

    ~WebArchiveDataset() {
        if(data_) {
            delete[] data_;
        }
    }

    void Init(const std::string& filename) {
        std::cout << "Loading UCLA WebArchive dataset..." << std::endl;
        ReadFile(filename);

        std::cout << "Loaded " << dataset_.NumInstances()
                  << " instances and "
                  << dataset_.NumCycles() << " cycles "
                  << "from file " << filename << std::endl;
    }

    void ReadFile(const std::string& filename) {

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
//                    if(tmp_data.size() >= 1000) break;       // max instances
//                    changes.resize(20);                        // max cycles

                    tmp_data.push_back(changes);
                }
                file.close();
            }

            if(tmp_data.size() > 0) {
                data_ = new Dataset::Instance[tmp_data.size()];
                for (int i = 0; i < tmp_data.size(); ++i) {
                    data_[i] = Dataset::Instance(tmp_data[i]);
                    dataset_.Add(&data_[i]);
                }
            }
        }

    Dataset& dataset() {
        return dataset_;
    }

private:
    Dataset dataset_;
    Dataset::Instance* data_;
};

#endif // DATASET_H
