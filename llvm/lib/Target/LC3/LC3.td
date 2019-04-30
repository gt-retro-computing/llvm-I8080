include "llvm/Target/Target.td"

include "LC3RegisterInfo.td"
include "LC3CallingConv.td"
include "LC3InstrInfo.td"

def LC3InstrInfo: InstrInfo {}

def LC3 : Target {
  let InstructionSet = LC3InstrInfo;
}