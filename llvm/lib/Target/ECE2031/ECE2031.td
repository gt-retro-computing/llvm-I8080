//===-- ECE2031.td - Describe the ECE2031 Target Machine -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//===----------------------------------------------------------------------===//
//
// This is the top level entry point for the ECE2031 target.
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

include "ECE2031RegisterInfo.td"
include "ECE2031InstrInfo.td"
include "ECE2031CallingConv.td"

def ECE2031InstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// ECE2031 processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
    : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def ECE2031 : Target {
  let InstructionSet = ECE2031InstrInfo;
}