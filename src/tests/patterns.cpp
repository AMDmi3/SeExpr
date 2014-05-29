/*
* Copyright Disney Enterprises, Inc.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License
* and the following modification to it: Section 6 Trademarks.
* deleted and replaced with:
*
* 6. Trademarks. This License does not grant permission to use the
* trade names, trademarks, service marks, or product names of the
* Licensor and its affiliates, except as required for reproducing
* the content of the NOTICE file.
*
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*/
#include <SeExpression.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "SeExprWalker.h"
#include "SeExprPatterns.h"
#include "SeExprFunc.h"
#include "SeControlSpec.h"


/**
   @file assignmentPatterns.cpp
*/
//! Simple expression class to list out variable uses
class PatternExpr : public SeExpression
{
public:
    struct DummyFuncX:SeExprFuncX
    {
        DummyFuncX()
            : SeExprFuncX(false)
        {};

        SeExprType prep(SeExprFuncNode* node, SeExprType wanted, SeExprVarEnv & env)
        {
            bool valid=true;
            for(int i=0;i<node->numChildren();i++){
                if(!node->isStrArg(i))
                    valid&=node->child(i)->prep(false, env).isValid();
            }
            return wanted;
        }

        virtual bool       isScalar() const { return true;                          };
        virtual SeExprType retType () const { return SeExprType().FP(1).Varying(); };

        void eval(const SeExprFuncNode* node,SeVec3d& result) const
        {result=SeVec3d();}
    } dummyFuncX;
    mutable SeExprFunc dummyFunc;

    //! Constructor that takes the expression to parse
    PatternExpr(const std::string& expr) :
        SeExpression(expr), dummyFunc(dummyFuncX,0,16),
        _examiner(),
        _walker(&_examiner)
    {};

    //! Empty constructor
    PatternExpr() :
        SeExpression(), dummyFunc(dummyFuncX,0,16),
        _examiner(),
        _walker(&_examiner)
    {};
    
    inline void walk () { _walker.walk (_parseTree); };
    void specs() { if(isValid()) { walk(); printSpecs(_examiner); }; };

private:
    SeExpr::SpecExaminer _examiner;
    SeExpr::ConstWalker  _walker;
    
    template<typename Examiner>
    void printSpecs(Examiner examiner) {
        if(isValid()) {
            for(int i = 0; i < examiner.length(); ++i)
                std::cout << examiner.spec(i)->toString()
                          << std::endl;
        };
    };

    //! resolve function that only supports one external variable 'x'
    SeExprVarRef* resolveVar(const std::string& name) const {
	return 0;
    };

    SeExprFunc* resolveFunc(const std::string& name) const
    {
        return &dummyFunc;
    }
};


void quit(const std::string & str) {
    if(str == "quit"
       || str == "q")
	exit(0);
};


int main(int argc,char *argv[])
{
    PatternExpr expr;
    std::string str;

    std::cout << "SeExpr Basic Pattern Matcher:";

    while(true) {
	std::cout << std::endl << "> ";
	//std::cin >> str;
	getline(std::cin, str);

	if(std::cin.eof()) {
	    std::cout << std::endl;
	    str = "q";
	};

	quit(str);
        expr.setExpr(str);

	if(!expr.isValid()) {
	    std::cerr << "Expression failed: " << expr.parseError() << std::endl;
	} else {
            std::cout << "Expression value:"
                      << std::endl;
	    std::cout << "   " << expr.evaluate() << std::endl;
            std::cout << "Expression patterns:"
                      << std::endl;
            expr.specs();
	};
    };

    return 0;
}
