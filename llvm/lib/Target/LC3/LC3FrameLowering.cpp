//
// Created by codetector on 4/29/19.
//

#include "LC3FrameLowering.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

using namespace llvm;

LC3FrameLowering::LC3FrameLowering() : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 4, 0) {
}

bool LC3FrameLowering::hasFP(const MachineFunction &MF) const {
    return true;
}

void LC3FrameLowering::emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const {
    const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
    MachineBasicBlock::iterator MBBI = MBB.begin();
    DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
    uint64_t stackSize = computeStackSize(MF);
    if (!stackSize) {
        return;
    }

    // Adj SP
    unsigned stackReg = LC3::
}

void LC3FrameLowering::emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const {

}

MachineBasicBlock::iterator LC3FrameLowering::eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                                                            MachineBasicBlock::iterator I) const {
    return TargetFrameLowering::eliminateCallFramePseudoInstr(MF, MBB, I);
}

uint64_t LC3FrameLowering::computeStackSize(MachineFunction &MF) const {
    return 0;
}

