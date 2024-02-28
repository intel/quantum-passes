#include <clang/Quantum/quintrinsics.h>

int main(){

	qbit q[4];

	CNOTGate(q[0], q[1]);

	CNOTGate(q[1], q[2]);

	RXGate(q[1], 0.35);

	CNOTGate(q[2], q[3]);

	TdagGate(q[3]);

	return 0;
}

