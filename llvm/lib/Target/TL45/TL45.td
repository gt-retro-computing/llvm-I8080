//===-- TL45.td - Describe the TL45 Target Machine -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//===----------------------------------------------------------------------===//
//
// This is the top level entry point for the TL45 target.
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

include "TL45RegisterInfo.td"
include "TL45InstrInfo.td"
include "TL45CallingConv.td"

def TL45InstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// TL45 processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
    : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def TL45 : Target {
  let InstructionSet = TL45InstrInfo;
}