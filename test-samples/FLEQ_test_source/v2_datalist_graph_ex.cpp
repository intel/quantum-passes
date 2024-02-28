#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>
#include <clang/Quantum/datalist.h>

using namespace qexpr;
using namespace qlist;

define_quantum_data(ring_p, "0-1, 1-2, 2-3, 3-4, 4-0, 1-3");
define_quantum_data(rnd, "0-3, 2-4, 4-1, 0-2");

const unsigned N = 5;
qbit listable(vertices, N);

QExpr ZZ(qbit &q1, qbit &q2, double ang){
  return join(_CNOT(q1, q2),
          join(_RZ(q2, ang),
          _CNOT(q1, q2)));
}

QExpr addEdge(DataList edge, QList Vs, double ang){
  const DataList numeric("0123456789");
  unsigned long frst_beg = edge.find_any(numeric);
  unsigned long frst_end = (edge >> frst_beg).find_not(numeric) + frst_beg;
  auto v1 = edge(frst_beg, frst_end);
  unsigned long sec_beg = (edge >> frst_end).find_any(numeric) + frst_end;
  unsigned long sec_end = (edge >> sec_beg).find_not(numeric) + sec_beg;
  auto v2 = edge(sec_beg, sec_end);
  return ZZ(Vs[_i(v1)], Vs[_i(v2)], ang);
}

QExpr parse_graph(QExpr in, DataList graph, QList Vs, double ang){
  auto edge = graph(0, ",");
  auto graph_fwd = graph.after_next(",");
  return cIf(graph.size() > 0, 
           parse_graph(addEdge(edge, Vs, ang) * in, graph_fwd, Vs, ang),
           in);
}

QExpr build_graph_circuit(DataList graph, QList Vs, double ang){
  return parse_graph(identity(), graph, Vs, ang); 
}


int main() {
  eval_hold(build_graph_circuit(ring_p, vertices, 0.1));
  eval_hold(build_graph_circuit(rnd, vertices, 0.1));
}