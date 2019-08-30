#include "LC2200.h"
#include "LC2200TargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Debug.h"
#include "LC2200InstrInfo.h"


class LC2200DAGToDAGISel : public SelectionDAGISel {
	const LC2200Subtarget &Subtarget;
 public:
	explicit LC2200DAGToDAGISel(LC2200TargetMachine &TM, CodeGenOpt::Level OptLevel) 
		: SelectionDAGISel(TM, OptLevel), Subtarget(*TM.getSubtargetImpl()) {}
	SDNode *Select(SDNode *N);
	bool SelectAddr(SDValue Addr, SDValue &Base, SDValue &Offset);
};



SDNode *LC2200DAGToDAGISel::Select(SDNode *N) {
return SelectCode(N);
}


bool LC2200DAGToDAGISel::SelectAddr(SDValue Addr, SDValue &Base, SDValue &Offset) {
	if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
		Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), getTargetLowering()->getPointerTy());
		Offset = CurDAG->getTargetConstant(0, MVT::i32);
		return true;
	}
	if (Addr.getOpcode() == ISD::TargetExternalSymbol ||
		Addr.getOpcode() == ISD::TargetGlobalAddress ||
		Addr.getOpcode() == ISD::TargetGlobalTLSAddress) {
		return false; // direct calls.
	}
	Base = Addr;
	Offset = CurDAG->getTargetConstant(0, MVT::i32);
	return true;
}


FunctionPass *llvm::createLC2200ISelDag(LC2200TargetMachine &TM, CodeGenOpt::Level OptLevel) {
	return new LC2200DAGToDAGISel(TM, OptLevel);
}