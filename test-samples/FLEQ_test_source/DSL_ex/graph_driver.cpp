#include <vector>
#include <iostream>

#include "graph_parser.h"

define_quantum_data(src, (defpoints nodes 10 \n \n defgraph my_graph (nodes) \n for i all (0-i) \n (1-6) \nend \n\nmy_cost = build my_graph as cost \nmy_mixer = build nodes as mixer \n\nrun(my_cost) my_cost, my_mixer x 10\n));

QExpr oneShotDriver(const DataList source, double *arry, double &outcome){
  return coarseParserData(source, "", QList::empty_list(), arry, outcome);
}

const unsigned N = 1000;
double compute(double array[]){
  double outcome = 0;
  for(int i = 0; i < N; i++)
    eval_release(oneShotDriver(src, array, outcome));
  return outcome / (double)N;
}

//fake optimizer
template<class Func>
double optimize(Func, std::vector<double>&);

int main() {
  
  auto run_cmd = src(src.find("run ") + 4, src.size());
  DataList numeric("0123456789");
  auto reps = run_cmd(run_cmd.find_any(numeric), run_cmd.find_any_last(numeric));
  unsigned arry_sz = (run_cmd.count(",") + 1) * _i(reps);
  std::vector<double> params(arry_sz, 1.);

  double outcome = optimize(compute, params);

  //std::cout<<"Your minimum value is "<<outcome<<"\n";

  return 0;

}