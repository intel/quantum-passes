#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>
#include <clang/Quantum/datalist.h>

using namespace qexpr;
using namespace qlist;

#include "graph_utils.h"

//main parsing function
QExpr coarseParserData(const DataList  src, const DataList sym_table, 
                       const QList qubits, double * params, double &outcome);


//next level parsing functions
QExpr addPointsAsQubits(const DataList line, const DataList remaining, 
                        const DataList sym_table, const QList qubits, 
               double *params, double & outcome);

QExpr addGraphToSymbols(const DataList line, const DataList remaining, 
                        const DataList sym_table, const QList qubits, 
               double *params, double & outcome);

QExpr addAssignToSymbols(const DataList line, const DataList remaining, 
                         const DataList sym_table, const QList qubits, 
               double *params, double & outcome);

QExpr buildRun(const DataList line, const DataList remaining, 
               const DataList sym_table, const QList qubits, 
               double *params, double &outcome);

//This is a dummy to be expanded later
QExpr handleParseError(){
  return identity();
}
QExpr handleParseError0(){
  return identity();
}
QExpr handleParseError1(){
  return identity();
}

QExpr coarseParserData(const DataList src, const DataList sym_table, const QList qubits, double * params, double &outcome) {
  //get the line to parse
  auto line = src(0, "\n");
  auto remaining = src.after_next("\n");

  //check for keywords
  return
      //cIf(line.empty(), handleParseError0(),  
      cIf(line.contains("defpoints ") , addPointsAsQubits(line.after_next("defpoints "), remaining, sym_table, qubits, params, outcome),
      cIf(line.contains("defgraph")   , addGraphToSymbols(line.after_next("defgraph"), remaining, sym_table, qubits, params, outcome),
      cIf(line.contains("=")          , addAssignToSymbols(line, remaining, sym_table, qubits, params, outcome),
      cIf(line.contains("run (")      , buildRun(line.after_next("run "), remaining, sym_table,qubits, params, outcome),
      cIf(line.contains("run(")       , buildRun(line.after_next("run"), remaining, sym_table,qubits, params, outcome),     
      cIf(line.find_not(" ") 
             == line.size()       , coarseParserData(remaining, sym_table, qubits, params, outcome),
      handleParseError1())))))); //default

}

#include "graph_sym_table.h"
#include "graph_build.h"