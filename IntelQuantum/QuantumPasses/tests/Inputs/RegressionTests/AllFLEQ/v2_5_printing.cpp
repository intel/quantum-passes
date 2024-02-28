#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;
using namespace datalist;

qbit listable(q ,3);

QExpr HAndPrint(QList qs){
  unsigned sz = qs.size();
  return cIf(sz > 0,
           printDataList(DataList("Adding H for qubit ") + DataList(sz - 1) + DataList("\n"),
           printQuantumLogic(
             _H(qs[sz -1]) * HAndPrint(qs << 1)
           )),
           identity()
         );
}

QExpr PrintWithBranch(bool b1, bool b2){
  return printDataList(DataList("\nThis is my quantum logic:\n\n"),
         printQuantumLogic(cIf(b1, 
                               _H(q[0]),
                           cIf(b2,
                               _H(q[1]),
                               _H(q[2])
                              ))
                          )
                      ); 
}

bool doWeBranch();

int main(){

  eval_hold(printDataList(DataList(
    "\nBuilding my QExpr step-by-step:\n\n"),
    HAndPrint(q)));
    
  eval_hold(PrintWithBranch(doWeBranch(), doWeBranch()));

}