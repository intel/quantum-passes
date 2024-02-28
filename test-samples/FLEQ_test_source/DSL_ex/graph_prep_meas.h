//preparation
QExpr preparePoints(const DataList entry, const QList qubits, bool op);

QExpr prepareGraph(const DataList entry, const DataList sym_table, 
                   const QList qubits, bool op);

//measurement
QExpr measurePoints(const DataList entry, const QList qubits, bool op, 
                    double &outcome);

QExpr MeasXtoDouble(qbit &qubits, double &outcome);

QExpr MeasZtoDouble(qbit &qubits, double &outcome);

QExpr measureGraph(const DataList entry, const DataList sym_table, 
                   const QList qubits, bool op, double &outcome);

QExpr measureEdges(const DataList entry, const QList  qubits, bool op, 
                   double &outcome);

QExpr handlFor(const DataList after_for, const QList  qubits, bool op, 
               double &outcome);

QExpr handleSubset(const DataList after_for, const QList  qubits, 
                   double param, bool op, double &outcome);

QExpr measureEdge(const DataList edge, const DataList idx, 
                  const QList  qubits, bool op, double &outcome);

QExpr measXX(qbit &q1, qbit &q2, double &outcome);

QExpr measZZ(qbit &q1, qbit &q2, double &outcome);

QExpr prepareSymbol(const DataList symbol, const DataList sym_table, const QList qubits, bool op){
  auto lookup = sym_table.after_next("@" + symbol)(2/* #*/, "@");
  char type = lookup[0];
  return
        cIf(type == 'c'       , prepareSymbol(lookup>>2, sym_table, qubits, true),
        cIf(type == 'm'       , prepareSymbol(lookup>>2, sym_table, qubits, false),
        cIf(type == 'p'       , preparePoints(lookup>>2, qubits, op),
        cIf(type == 'g'       , prepareGraph(lookup>>2, sym_table, qubits, op),
        handleParseError()))));
}

QExpr preparePoints(const DataList entry, const QList qubits, bool op){
  auto subset = getQbits(entry, qubits);
  return cIf(op, map(_PrepX, qubits), map(_PrepZ, qubits));
}

QExpr prepareGraph(const DataList entry, const DataList sym_table, const QList qubits, bool op){
  auto pts = entry("(", ")") >> 1;
  auto lookup = sym_table.after_next(pts)(4/* #*/, "@");
  return preparePoints(lookup, qubits, op);
}

QExpr measureSymbol(const DataList symbol, const DataList sym_table, const QList qubits, bool op, double &outcome){
  auto lookup = sym_table.after_next("@" + symbol)(2/* #*/, "@");
  char type = lookup[0];
  return
        cIf(type == 'c'       , measureSymbol(lookup>>2, sym_table, qubits, false, outcome),
        cIf(type == 'm'       , measureSymbol(lookup>>2, sym_table, qubits, true, outcome),
        cIf(type == 'p'       , measurePoints(lookup>>2, qubits, op, outcome),
        cIf(type == 'g'       , measureGraph(lookup>>2, sym_table, qubits, op, outcome),
        handleParseError()))));
}

QExpr measurePoints(const DataList entry, const QList qubits, bool op, double &outcome){
  auto subset = getQbits(entry, qubits);
  return cIf(op, map<double&>(MeasXtoDouble, qubits, outcome), map<double&>(MeasZtoDouble, qubits, outcome));
}

PROTECT QExpr MeasXtoDouble(qbit &qubit, double &outcome){
  cbit loc;
  MeasX(qubit, loc);
  if(loc)
    outcome -= 1.;
  else
    outcome += 1.;

  return this_as_expr;
}

PROTECT QExpr MeasZtoDouble(qbit &qubit, double &outcome){
  cbit loc;
  MeasZ(qubit, loc);
  if(loc)
    outcome -= 1.;
  else
    outcome += 1.;

  return this_as_expr;
}

QExpr measureGraph(const DataList entry, const DataList sym_table, const QList qubits, bool op, double &outcome){
  auto pts = entry("(", ")") >> 1;
  auto pt_lookup = sym_table.after_next(pts)(4/* #n:*/, "@");
  auto pt_qubits = getQbits(pt_lookup, qubits);
  auto edge_def = entry.after_next("\n");
  return measureEdges(edge_def, pt_qubits, op, outcome);
}

QExpr handleFor(const DataList after_for, const QList  qubits, bool op, double &outcome);

QExpr measureEdges(const DataList entry, const QList  qubits, bool op, double &outcome){
  auto line = entry(0, "\n");
  auto after_for = line.after_next("for ");
  return cIf(line.contains("(") && line.contains(")") && line.contains("-"),
           cIf(!after_for.empty(),
             handleFor(after_for, qubits, op, outcome),
             measureEdge(line, "", qubits, op, outcome)
           ),
           identity()
         )      
         * cIf(entry.size() > 0,
             measureEdges(entry.after_next("\n"), qubits, op, outcome),
             identity()
         );
}

QExpr handleSubset(const DataList after_for, const QList  qubits, bool op, double &outcome);

QExpr handleFor(const DataList after_for, const QList  qubits, bool op, double &outcome){
  auto idx_name = after_for.next_not(" ")(0, " ");
  bool has_all = after_for.contains("all");

  return cIf(has_all,
           measureEdge(after_for.next("("), idx_name, qubits, op, outcome),
           handleSubset(after_for, qubits, op, outcome)            
         );
}

QExpr handleSubset(const DataList after_for, const QList  qubits, bool op, double &outcome){
  auto idx_name = after_for.next_not(" ")(0, " ");
  unsigned  edge_start = after_for.find("(");
  auto upto_edge = after_for(0, edge_start);
  DataList numeric("0123456789");
  auto idx_edge = upto_edge(after_for.find_any(numeric), after_for.find_any_last(numeric)); 
  
  return cIf(after_for.contains("<"),
           measureEdge(after_for.next("("), idx_name, qubits(0, _i(idx_edge)), op, outcome),
           measureEdge(after_for.next("("), idx_name, qubits(_i(idx_edge), qubits.size()), op, outcome)
         );
}

QExpr measureEdge(const DataList edge, const DataList idx, const QList  qubits, bool op, double &outcome){
  auto pt1 = trim(edge(edge.find("(") + 1, edge.find("-")));
  auto pt2 = trim(edge(edge.find("-") + 1, edge.find(")")));
  bool run1 = pt1.contains(idx) && !idx.empty();
  bool run2 = pt2.contains(idx) && !idx.empty();

  return  cIf(run1,
            cIf(op, map<qbit&, double&>(measXX, qubits, qubits[_i(pt2)], outcome), map<qbit&, double&>(measZZ, qubits, qubits[_i(pt2)], outcome)),
            cIf(run2,
              cIf(op, map<qbit&, double&>(measXX, qubits, qubits[_i(pt1)], outcome), map<qbit&, double&>(measZZ, qubits, qubits[_i(pt1)], outcome)),
              cIf(op, measXX(qubits[_i(pt1)], qubits[_i(pt2)], outcome), measZZ(qubits[_i(pt1)], qubits[_i(pt2)], outcome))  
            )
          );
}

QExpr measXX(qbit &q1, qbit &q2, double &outcome){
  return cIf(qbits_equal(q1, q2),
           identity(), 
           _CNOT(q1, q2)
           * MeasZtoDouble(q1, outcome)
           * _CNOT(q1, q2)
         );
}

QExpr measZZ(qbit &q1, qbit &q2, double &outcome){
  return cIf(qbits_equal(q1, q2),
           identity(), 
           _CNOT(q1, q2)
           * MeasZtoDouble(q2, outcome)
           * _CNOT(q1, q2)
         );
}