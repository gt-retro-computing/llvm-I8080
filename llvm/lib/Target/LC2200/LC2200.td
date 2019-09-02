//===-- LC2200.td - Describe the LC2200 Target Machine -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//===----------------------------------------------------------------------===//
//
// This is the top level entry point for the LC2200 target.
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

include "LC2200RegisterInfo.td"
include "LC2200InstrInfo.td"
include "LC2200CallingConv.td"

def LC2200InstInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// LC2200 processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
    : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def LC2200 : Target {
  let InstructionSet = LC2200InstInfo;
}