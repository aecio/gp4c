#ifndef TERMINALS_H
#define TERMINALS_H

#include <iostream>

#include "gp.h"
#include "crawling.h"

class Terminal : public GPNode {
public:
    Terminal(int id, const char* name): GPNode(id, (char*) name) { }
    virtual void PrintMathStyle(std::ostream& os) {
        os << representation;
    }
    virtual void PrintTexStyle(std::ostream& os) {
        os << representation;
    }
    virtual double Value(URL& url, int cycle) = 0;
};

class AgeTerminal : public Terminal {
public:
    AgeTerminal(int id): Terminal(id, "age") { }
    double Value(URL &url, int cycle) {
        return url.GetAge(cycle);
    }
};

class ChangesTerminal : public Terminal {
public:
    ChangesTerminal(int id): Terminal(id, "changes") { }
    double Value(URL &url, int cycle) {
        return url.changes;
    }
};

class VisitsTerminal : public Terminal {
public:
    VisitsTerminal(int id): Terminal(id, "visits") { }
    double Value(URL &url, int cycle) {
        return url.visits;
    }
};

class OneTerminal : public Terminal {
public:
    OneTerminal(int id): Terminal(id, "1") { }
    double Value(URL &url, int cycle) {
        return 1.0;
    }
};

class ChangeRateTerminal : public Terminal {
public:
    ChangeRateTerminal(int id): Terminal(id, "change_rate") { }
    double Value(URL &url, int cycle) {
        return url.GetChangeRate();
    }
    void PrintTexStyle(std::ostream& os) {
        os << "change\\_rate";
    }
};

class ChangeProbabilityTerminal : public Terminal {
public:
    ChangeProbabilityTerminal(int id): Terminal(id, "change_prob") { }
    double Value(URL &url, int cycle) {
        return url.GetChangeProbability();
    }
    void PrintTexStyle(std::ostream& os) {
        os << "change\\_prob";
    }
};

class ChangeProbabilityAgeTerminal : public Terminal {
public:
    ChangeProbabilityAgeTerminal(int id): Terminal(id, "change_prob_age") { }
    double Value(URL &url, int cycle) {
        return url.GetChangeProbabilityAge(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "change\\_prob\\_age";
    }
};


#endif // TERMINALS_H
