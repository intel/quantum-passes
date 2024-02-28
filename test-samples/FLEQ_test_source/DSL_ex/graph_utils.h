//space trimming utility
const DataList WRAP_ATTR trim(const DataList &name){
  return name(name.find_not(" "), name.find_not_last(" ") + 1);
}

void dummy(const char *);
void dummy(char);
//void dummy(unsigned);

const QList WRAP_ATTR getQbits(const DataList &entry, const QList &qubits){
  DataList numeric("0123456789");
  unsigned start = _i(entry(entry.find_any(numeric), "t"));
  unsigned wdth = _i(entry >> (entry.find("t") + 1));
  return qubits(start, start + wdth);
}

template<typename... Args>
QExpr map(QExpr(*qk)(qbit&, Args...), QList qs, Args... args){
  return cIf(size(qs) > 0, 
           qk(qs[0], args...) * map<Args...>(qk, qs + 1, args...),
           identity()
  );        
}