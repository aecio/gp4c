#ifndef TERMINALS_H
#define TERMINALS_H

#include <iostream>

#include "gp.h"

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
        return url.changes();
    }
};

class VisitsTerminal : public Terminal {
public:
    VisitsTerminal(int id): Terminal(id, "visits") { }
    double Value(URL &url, int cycle) {
        return url.visits();
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

class NADChangeRateTerminal : public Terminal {
public:
    NADChangeRateTerminal(int id): Terminal(id, "nad_change_rate") { }
    double Value(URL &url, int cycle) {
        return url.GetNADChangeRate(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "nad\\_change\\_rate";
    }
};

class SADChangeRateTerminal : public Terminal {
public:
    SADChangeRateTerminal(int id): Terminal(id, "sad_change_rate") { }
    double Value(URL &url, int cycle) {
        return url.GetSADChangeRate(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "sad\\_change\\_rate";
    }
};

class AADChangeRateTerminal : public Terminal {
public:
    AADChangeRateTerminal(int id): Terminal(id, "aad_change_rate") { }
    double Value(URL &url, int cycle) {
        return url.GetAADChangeRate(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "aad\\_change\\_rate";
    }
};

class GADChangeRateTerminal : public Terminal {
public:
    GADChangeRateTerminal(int id): Terminal(id, "gad_change_rate") { }
    double Value(URL &url, int cycle) {
        return url.GetGADChangeRate(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "gad\\_change\\_rate";
    }
};

#endif // TERMINALS_H
