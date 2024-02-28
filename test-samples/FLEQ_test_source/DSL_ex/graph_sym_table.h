using namespace qexpr;
using namespace qlist;

QExpr addPointsToSymbols(const DataList name, const DataList range, const DataList remaining, 
                         const DataList sym_table, const QList qubits, double * params, double &outcome);


QExpr addPointsAsQubits(const DataList line, const DataList remaining, const DataList sym_table, const QList qubits, double * params, double &outcome) {
  //create qubits by parsing the input line
  DataList numeric("01234556789");
  auto num_pos = line.find_any(numeric);
  auto pts_rough = line(0, num_pos);
  auto pts_name = trim(pts_rough);
  auto wdth = line(line.find_any(numeric), line.find_any_last(numeric) + 1);
  QList pts_qbits(IQC_alloca<qbit>(pts_name, _i(wdth)));
  auto range = DataList(qubits.size())+ "t" + wdth;
  //return by adding the points to the symbol table
  return addPointsToSymbols(pts_name, range, remaining, sym_table, qubits + pts_qbits, params, outcome);
}

QExpr addPointsToSymbols(const DataList name, const DataList range, const DataList remaining, const DataList sym_table, const QList qubits, double * params, double &outcome){
  return coarseParserData(remaining, sym_table + "@" + name + " " + "#p:"+ range, qubits, params, outcome);
}

QExpr addGraphToSymbols(const DataList line, const DataList remaining, const DataList sym_table, const QList qubits, double * params, double &outcome){
  auto gr_name = trim(line(0, "("));
  auto pts = line("(", line.find(")")  + 1);
  auto gr_def_end = remaining.find("end");
  auto rm_gr_def = (remaining>>gr_def_end).after_next("\n");
  return coarseParserData(rm_gr_def, sym_table + "@" + gr_name + " " + "#g:"+ pts +"\n" + remaining(0, gr_def_end), qubits, params, outcome);  
}

QExpr addAssignToSymbols(const DataList line, const DataList remaining, const DataList sym_table, const QList qubits, double * params, double &outcome){
  auto name_rough = line(0, "=");
  auto name = trim(name_rough);
  //only possiblity we allow is build <symbol> as <cost or mixer>
  auto as_pos = line.find(" as "); 
  auto sym = trim(line(line.find("build") + 5, as_pos));
  auto bl_key = line >> as_pos;
  return  cIf(bl_key.contains("cost") , coarseParserData(remaining, sym_table + "@" + name + " " + "#c:"+ sym, qubits, params, outcome),
          cIf(bl_key.contains("mixer") , coarseParserData(remaining, sym_table + "@" + name + " " + "#m:"+ sym, qubits, params, outcome),
          handleParseError()));
}