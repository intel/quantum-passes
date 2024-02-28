using namespace qexpr;
using namespace qlist;

//preparation
QExpr prepareSymbol(const DataList symbol, const DataList sym_table, 
                    const QList qubits, bool op);

//measurement
QExpr measureSymbol(const DataList symbol, const DataList sym_table, 
                    const QList qubits, bool op, double &outcome);

//build
QExpr buildFromSymbolsMulti(const DataList symbols, const DataList sym_table, const QList  qubits, double *params, int power);

//main
QExpr buildRun(const DataList line, const DataList remaining, const DataList sym_table, const QList qubits, double *params, double & outcome){
  //only support (<symbol>) <symbol1>, <symbol2> ... x <number>
  //get the argument symbol
  auto arg = line(line.find("(") + 1, line.find(")"));
  auto x_pos = line.find(" x");
  auto symbols = line(line.find(")") + 1, x_pos);
  auto power_temp = line>>x_pos;
  auto power = power_temp(2, power_temp.find_any_last("0123456789") + 1);
  return measureSymbol(arg, sym_table, qubits, false, outcome)
         * buildFromSymbolsMulti(symbols, sym_table, qubits, params, _i(power))
         * prepareSymbol(arg, sym_table, qubits, false);
}

QExpr buildFromSymbols(const DataList symbols, const DataList sym_table, 
                       const QList  qubits, double *params);

QExpr buildFromSymbolsMulti(const DataList symbols, const DataList sym_table, const QList  qubits, double *params, int power){
  //get number of symbols
  unsigned num_sym = symbols.count(",") + 1;

  return cIf(power > 0,
               buildFromSymbols(symbols, sym_table, qubits, params) 
               + buildFromSymbolsMulti(symbols, sym_table, qubits, params + num_sym, power - 1),
              identity()
            );
}

QExpr buildSymbol(const DataList entry, const DataList sym_table, 
                  const QList  qubits, double param, bool op);

QExpr buildFromSymbols(const DataList symbols, const DataList sym_table, const QList  qubits, double *params) {
  auto symbol = trim(symbols(0, ","));
  //get lookup
  auto lookup = sym_table.after_next(symbol)(2/* #*/, "@");
  char type = lookup[0];
  
  return cIf(lookup.size() > 0, 
               buildFromSymbols(symbols.after_next(","), sym_table, qubits, params + 1)
               * cIf(type == 'c', buildSymbol(lookup>>2, sym_table, qubits, params[0], false),
                 cIf(type == 'm', buildSymbol(lookup>>2, sym_table, qubits, params[0], true),
                 handleParseError0())),
               identity()
            );

}

QExpr buildPoints(const DataList entry, const QList qubits, double param, bool op);

QExpr buildGraph(const DataList entry, const DataList  sym_table,const QList qubits, 
                 double param, bool op);

QExpr buildSymbol(const DataList entry, const DataList sym_table, const QList  qubits, double param, bool op){
  auto lookup = sym_table.after_next(entry)(2/* #*/, "@");
  char type = lookup[0];

  return 
        cIf(type == 'p', buildPoints(lookup>>2, qubits, param, op),
        cIf(type == 'g', buildGraph(lookup>>2, sym_table, qubits, param, op),
        handleParseError0()));

}

QExpr RXOnAll(const QList  qubits, double param){
  return cIf(qubits.size() > 0, 
           RXOnAll(++qubits, param)
           * _RX(qubits[0], param),
           identity()
         );
}

QExpr RZOnAll(const QList  qubits, double param){
  return cIf(qubits.size() > 0, 
           RZOnAll(++qubits, param)
           * _RZ(qubits[0], param),
           identity()
         );
}

QExpr buildPoints(const DataList entry, const QList qubits, double param, bool op){
  auto subset = getQbits(entry, qubits);
  return cIf(op, RXOnAll(subset, param), RZOnAll(subset, param));
}

QExpr buildEdges(const DataList entry, const QList  qubits, double param, bool op);

QExpr buildGraph(const DataList entry, const DataList  sym_table,const QList qubits, double param, bool op){
  auto pts = entry("(", ")") >> 1;
  auto pt_lookup = sym_table.after_next(pts)(4/* #n:*/, "@");
  auto pt_qubits = getQbits(pt_lookup, qubits);
  auto edge_def = entry.after_next("\n");
  return buildEdges(edge_def, pt_qubits, param, op);
}

QExpr handleForBuild(const DataList after_for, const QList  qubits, double param, 
               bool op);

QExpr buildEdge(const DataList edge, const DataList idx, const QList  qubits, 
                double param, bool op);

QExpr buildEdges(const DataList entry, const QList  qubits, double param, bool op){

  auto line = entry(0, "\n");
  auto after_for = line.after_next("for ");
  return cIf(line.contains("(") && line.contains(")") && line.contains("-"),
           cIf(!after_for.empty(),
             handleForBuild(after_for, qubits, param, op),
             buildEdge(line, "", qubits, param, op)
           ),
           identity()
         )      
         * cIf(entry.size() > 0,
             buildEdges(entry.after_next("\n"), qubits, param, op),
             identity()
         );
}

QExpr handleSubsetBuild(const DataList after_for, const QList  qubits, 
                   double param, bool op);

QExpr handleForBuild(const DataList after_for, const QList  qubits, double param, bool op){

  auto idx_name = after_for.next_not(" ")(0, " ");
  bool has_all = after_for.contains("all");

  return cIf(has_all,
           buildEdge(after_for.next("("), idx_name, qubits, param, op),
           handleSubsetBuild(after_for, qubits, param, op)            
         );
}

QExpr handleSubsetBuild(const DataList after_for, const QList  qubits, double param, bool op){
  auto idx_name = after_for.next_not(" ")(0, " ");
  unsigned  edge_start = after_for.find("(");
  auto upto_edge = after_for(0, edge_start);
  DataList numeric("0123456789");
  auto idx_edge = upto_edge(after_for.find_any(numeric), after_for.find_any_last(numeric)); 
  
  return cIf(after_for.contains("<"),
           buildEdge(after_for.next("("), idx_name, qubits(0, _i(idx_edge)), param, op),
           buildEdge(after_for.next("("), idx_name, qubits(_i(idx_edge), qubits.size()), param, op)
         );
}

QExpr XX(qbit &q1, qbit &q2, double param){
  return cIf(qbits_equal(q1, q2),
           identity(),
           _CNOT(q1, q2)
           * _RX(q1, param)
           * _CNOT(q1, q2)
         );
}

QExpr ZZ(qbit &q1, qbit &q2, double param){
  return cIf(qbits_equal(q1, q2),
           identity(),
           _CNOT(q1, q2)
           * _RZ(q2, param)
           * _CNOT(q1, q2)
         );
}

QExpr buildEdge(const DataList edge, const DataList idx, const QList  qubits, double param, bool op){
  auto pt1 = trim(edge(edge.find("(") + 1, edge.find("-")));
  auto pt2 = trim(edge(edge.find("-") + 1, edge.find(")")));
  bool run1 = pt1.contains(idx) && !idx.empty();
  bool run2 = pt2.contains(idx) && !idx.empty();

  return  cIf(run1,
            cIf(op, map<qbit&, double>(XX, qubits, qubits[_i(pt2)], param), map<qbit&, double>(ZZ, qubits, qubits[_i(pt2)], param)),
            cIf(run2,
              cIf(op, map<qbit&, double>(XX, qubits, qubits[_i(pt1)], param), map<qbit&, double>(ZZ, qubits, qubits[_i(pt1)], param)),
              cIf(op, XX(qubits[_i(pt1)], qubits[_i(pt2)], param), ZZ(qubits[_i(pt1)], qubits[_i(pt2)], param))  
            )
          );

}

#include "graph_prep_meas.h"
