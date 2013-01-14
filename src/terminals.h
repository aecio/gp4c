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
    TChangeProbabilityCho(int id): Terminal(id, "cho_prob") { }
    double Value(URL &url, int cycle) {
        return url.GetChangeProbabilityCho(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "cho\\_prob";
    }
};

class TChangeProbabilityNAD : public Terminal {
public:
    TChangeProbabilityNAD(int id): Terminal(id, "nad_prob") { }
    double Value(URL &url, int cycle) {
        return url.GetChangeProbabilityNAD(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "nad\\_prob";
    }
};

class TChangeProbabilitySAD : public Terminal {
public:
    TChangeProbabilitySAD(int id): Terminal(id, "sad_prob") { }
    double Value(URL &url, int cycle) {
        return url.GetChangeProbabilitySAD(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "sad\\_prob";
    }
};

class TChangeProbabilityAAD : public Terminal {
public:
    TChangeProbabilityAAD(int id): Terminal(id, "aad_prob") { }
    double Value(URL &url, int cycle) {
        return url.GetChangeProbabilityAAD(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "aad\\_prob";
    }
};

class TChangeProbabilityGAD : public Terminal {
public:
    TChangeProbabilityGAD(int id): Terminal(id, "gad_prob") { }
    double Value(URL &url, int cycle) {
        return url.GetChangeProbabilityGAD(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "gad\\_prob";
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

//
// Non-windowed Tan's Adaptive Change Rates
//

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

// Windowed Change Rates

class TWindowedNADChangeRate : public Terminal {
public:
    TWindowedNADChangeRate(int id): Terminal(id, "wnad") { }
    double Value(URL &url, int cycle) {
        return url.GetWindowedNADChangeRate(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "wnad";
    }
};

class TWindowedAADChangeRate : public Terminal {
public:
    TWindowedAADChangeRate(int id): Terminal(id, "waad") { }
    double Value(URL &url, int cycle) {
        return url.GetWindoedAADChangeRate(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "waad";
    }
};

class TWindowedGADChangeRate : public Terminal {
public:
    TWindowedGADChangeRate(int id): Terminal(id, "wgad") { }
    double Value(URL &url, int cycle) {
        return url.GetWindowedGADChangeRate(cycle);
    }
    void PrintTexStyle(std::ostream& os) {
        os << "wgad";
    }
};

// Other terminals

class TChoNumerator : public Terminal {
public:
    TChoNumerator(int id): Terminal(id, "cho_num") { }
    double Value(URL &url, int cycle) {
        return url.visits() - url.changes() + 0.5;
    }
    void PrintTexStyle(std::ostream& os) {
        os << "cho\\_num";
    }
};

class TChoDenominator : public Terminal {
public:
    TChoDenominator(int id): Terminal(id, "cho_den") { }
    double Value(URL &url, int cycle) {
        return url.visits() + 0.5;
    }
    void PrintTexStyle(std::ostream& os) {
        os << "cho\\_den";
    }
};

#endif // TERMINALS_H
