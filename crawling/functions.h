#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cmath>

#include "gp.h"
#include "crawling.h"

class Function : public GPNode {
public:
    Function(int id, const char* name, int n_args): GPNode(id, (char*) name, n_args) { }
    virtual void PrintMathStyle(MyGene* gene, std::ostream& os, int precedence) = 0;
    virtual void PrintTexStyle(MyGene* gene, std::ostream& os, int precedence) = 0;
    virtual double Value(MyGene* gene, URL &url, int cycle) = 0;
    virtual int Precedence() = 0;
protected:
    void PrintMathStyle1Arg(MyGene* gene, std::ostream& os, int precedence) {
        os << representation << "(";
        gene->NthMyChild(0)->printMathStyle (os, precedence);
        os << ")";
    }
    void PrintMathStyle2Args(MyGene* gene, std::ostream& os, int precedence) {
        gene->NthMyChild(0)->printMathStyle (os, precedence);
        os << representation;
        gene->NthMyChild(1)->printMathStyle (os, precedence);
    }
    void PrintTexStyle2Args(MyGene* gene, std::ostream& os, int precedence) {
        gene->NthMyChild(0)->printTeXStyle(os, precedence);
        os << representation;
        gene->NthMyChild(1)->printTeXStyle(os, precedence);
    }
};

class PlusFunction : public Function {
public:
    PlusFunction(int id): Function(id, "+", 2) { }
    double Value(MyGene* gene, URL &url, int cycle) {
        return gene->NthMyChild(0)->evaluate(url, cycle)
                + gene->NthMyChild(1)->evaluate (url, cycle);
    }
    void PrintMathStyle(MyGene* gene, std::ostream& os, int precedence) {
        PrintMathStyle2Args(gene, os, precedence);
    }
    void PrintTexStyle(MyGene* gene, std::ostream& os, int precedence) {
        PrintTexStyle2Args(gene, os, precedence);
    }
    int Precedence() {
        return 1;
    }
};

class MinusFunction : public Function {
public:
    MinusFunction(int id): Function(id, "-", 2) { }
    double Value(MyGene* gene, URL &url, int cycle) {
        return gene->NthMyChild(0)->evaluate(url, cycle)
                - gene->NthMyChild(1)->evaluate (url, cycle);
    }
    void PrintMathStyle(MyGene* gene, std::ostream& os, int precedence) {
        PrintMathStyle2Args(gene, os, precedence);
    }
    void PrintTexStyle(MyGene* gene, std::ostream& os, int precedence) {
        PrintTexStyle2Args(gene, os, precedence);
    }
    int Precedence() {
        return 1;
    }
};

class TimesFunction : public Function {
public:
    TimesFunction(int id): Function(id, "*", 2) { }
    double Value(MyGene* gene, URL &url, int cycle) {
        return gene->NthMyChild(0)->evaluate(url, cycle)
                * gene->NthMyChild(1)->evaluate (url, cycle);
    }
    void PrintMathStyle(MyGene* gene, std::ostream& os, int precedence) {
        PrintMathStyle2Args(gene, os, precedence);
    }
    void PrintTexStyle(MyGene* gene, std::ostream& os, int precedence) {
        gene->NthMyChild(0)->printTeXStyle(os, precedence);
        os << " \\times ";
        gene->NthMyChild(1)->printTeXStyle(os, precedence);
    }
    int Precedence() {
        return 2;
    }
};

class DivisionFunction : public Function {
public:
    DivisionFunction(int id): Function(id, "%", 2) { }
    double Value(MyGene* gene, URL &url, int cycle) {
        return divide(gene->NthMyChild(0)->evaluate(url, cycle),
                      gene->NthMyChild(1)->evaluate (url, cycle));
    }
    void PrintMathStyle(MyGene* gene, std::ostream& os, int precedence) {
        PrintMathStyle2Args(gene, os, precedence);
    }
    void PrintTexStyle(MyGene* gene, std::ostream& os, int precedence) {
        // As we use \frac, we start again with precedence 0
        os << "\\frac{";
        gene->NthMyChild(0)->printTeXStyle(os, 0);
        os << "}{";
        gene->NthMyChild(1)->printTeXStyle(os, 0);
        os << "}";
    }
    int Precedence() {
        return 2;
    }
private:
    // This function is the divide with closure. Basically if you divide
    // anything by zero you get an error so we have to stop this
    // process. We check for a very small denominator and return a very
    // high value.
    inline double divide(double x, double y) {
        if(fabs(y)<1e-6) {
            if (x*y<0.0)
                return -1e6;
            else
                return 1e6;
        } else {
            return x/y;
        }
    }
};

class LogFunction : public Function {
public:
    LogFunction(int id): Function(id, "log", 1) { }
    double Value(MyGene* gene, URL &url, int cycle) {
        return safe_log(gene->NthMyChild(0)->evaluate(url, cycle));
    }
    void PrintMathStyle(MyGene* gene, std::ostream& os, int precedence) {
        PrintMathStyle1Arg(gene, os, precedence);
    }
    void PrintTexStyle(MyGene* gene, std::ostream& os, int precedence) {
        os << "\\log(";
        gene->NthMyChild(0)->printTeXStyle(os, precedence);
        os << ")";
    }
    int Precedence() {
        return 1;
    }
private:
    inline double safe_log(double x) {
        double arg = fabs(x);
        if ( arg <= 1.0)
            return 0.0;
        else
            return log( arg );
    }
};

class ExpFunction : public Function {
public:
    ExpFunction(int id): Function(id, "exp", 1) { }
    double Value(MyGene* gene, URL &url, int cycle) {
        return exp(gene->NthMyChild(0)->evaluate(url, cycle));
    }
    void PrintMathStyle(MyGene* gene, std::ostream& os, int precedence) {
        PrintMathStyle1Arg(gene, os, precedence);
    }
    void PrintTexStyle(MyGene* gene, std::ostream& os, int precedence) {
        os << "e^{";
        gene->NthMyChild(0)->printTeXStyle(os, precedence);
        os << "}";
    }
    int Precedence() {
        return 1;
    }
};

#endif // FUNCTIONS_H
