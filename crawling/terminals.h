#ifndef TERMINALS_H
#define TERMINALS_H

#include <iostream>
#include <sstream>

#include "gp.h"
#include "url.h"

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

class TAge : public Terminal {
public:
    TAge(int id): Terminal(id, "age") { }
    double Value(URL &url, int cycle) {
        return url.GetAge(cycle);
    }
};

class TChanges : public Terminal {
public:
    TChanges(int id): Terminal(id, "changes") { }
    double Value(URL &url, int cycle) {
        return url.changes();
    }
};

class TVisits : public Terminal {
public:
    TVisits(int id): Terminal(id, "visits") { }
    double Value(URL &url, int cycle) {
        return url.visits();
    }
};

class TConstValue : public Terminal {
public:
    TConstValue(int id, double value): Terminal(id, ""), value_(value) {
        std::stringstream ss;
        ss << value;
        representation = copyString((char*) ss.str().c_str());
    }
    double Value(URL &url, int cycle) {
        return value_;
    }
private:
    int value_;
};


class TChangeProbabilityCho : public Terminal {
public:
    TChangeProbabilityCho(int id): Terminal(id, "cp_cho") { }
    double Value(URL &url, int cycle) {
        return url.GetChangeProbabilityCho(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "cp\\_cho";
    }
};


class TChangeProbabilityNAD : public Terminal {
public:
    TChangeProbabilityNAD(int id): Terminal(id, "cp_nad") { }
    double Value(URL &url, int cycle) {
        return url.GetChangeProbabilityNAD(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "cp\\_nad";
    }
};

class TChangeProbabilitySAD : public Terminal {
public:
    TChangeProbabilitySAD(int id): Terminal(id, "cp_sad") { }
    double Value(URL &url, int cycle) {
        return url.GetChangeProbabilitySAD(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "cp\\_sad";
    }
};

class TChangeProbabilityAAD : public Terminal {
public:
    TChangeProbabilityAAD(int id): Terminal(id, "cp_aad") { }
    double Value(URL &url, int cycle) {
        return url.GetChangeProbabilityAAD(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "cp\\_'aad'";
    }
};

class TChangeProbabilityGAD : public Terminal {
public:
    TChangeProbabilityGAD(int id): Terminal(id, "cp_gad") { }
    double Value(URL &url, int cycle) {
        return url.GetChangeProbabilityGAD(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "cp\\_gad";
    }
};

class TChoChangeRate : public Terminal {
public:
    TChoChangeRate(int id): Terminal(id, "cho") { }
    double Value(URL &url, int cycle) {
        return url.GetChoChangeRate();
    }
    void PrintTexStyle(std::ostream& os) {
        os << "cho";
    }
};

class TNADChangeRate : public Terminal {
public:
    TNADChangeRate(int id): Terminal(id, "nad") { }
    double Value(URL &url, int cycle) {
        return url.GetNADChangeRate(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "nad";
    }
};

class TSADChangeRate : public Terminal {
public:
    TSADChangeRate(int id): Terminal(id, "sad") { }
    double Value(URL &url, int cycle) {
        return url.GetSADChangeRate(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "sad";
    }
};

class TAADChangeRate : public Terminal {
public:
    TAADChangeRate(int id): Terminal(id, "aad") { }
    double Value(URL &url, int cycle) {
        return url.GetAADChangeRate(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "aad";
    }
};

class TGADChangeRate : public Terminal {
public:
    TGADChangeRate(int id): Terminal(id, "gad") { }
    double Value(URL &url, int cycle) {
        return url.GetGADChangeRate(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "gad";
    }
};

class TChoNumerator : public Terminal {
public:
    TChoNumerator(int id): Terminal(id, "cho_num") { }
    double Value(URL &url, int cycle) {
        return url.visits() - url.changes() + 0.5;
    }
    void PrintTexStyle(std::ostream& os) {
        os << "cho_num";
    }
};

class TChoDenominator : public Terminal {
public:
    TChoDenominator(int id): Terminal(id, "cho_den") { }
    double Value(URL &url, int cycle) {
        return url.visits() + 0.5;
    }
    void PrintTexStyle(std::ostream& os) {
        os << "cho_den";
    }
};

#endif // TERMINALS_H
