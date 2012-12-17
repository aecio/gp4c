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
    virtual double Value(Instance& url, int cycle) = 0;
};

class AgeTerminal : public Terminal {
public:
    AgeTerminal(int id): Terminal(id, "age") { }
    double Value(Instance &url, int cycle) {
        return url.GetAge(cycle);
    }
};

class ChangesTerminal : public Terminal {
public:
    ChangesTerminal(int id): Terminal(id, "changes") { }
    double Value(Instance &url, int cycle) {
        return url.changes;
    }
};

class VisitsTerminal : public Terminal {
public:
    VisitsTerminal(int id): Terminal(id, "visits") { }
    double Value(Instance &url, int cycle) {
        return url.visits;
    }
};

class OneTerminal : public Terminal {
public:
    OneTerminal(int id): Terminal(id, "1") { }
    double Value(Instance &url, int cycle) {
        return 1.0;
    }
};


#endif // TERMINALS_H
