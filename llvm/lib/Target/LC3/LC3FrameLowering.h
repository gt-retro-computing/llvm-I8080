//
// Created by codetector on 4/29/19.
//

#ifndef LLVM_LC3FRAMELOWERING_H
#define LLVM_LC3FRAMELOWERING_H

#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
    class LC3Subtarget;

    class LC3FrameLowering : public TargetFrameLowering {
    public:
        LC3FrameLowering();
        // emit Prolog/emitEpilog - These methods insert prolog and epilog code into
        // the function.
        void emitPrologue(MachineFunction &MF,
                          MachineBasicBlock &MBB) const override;
        void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
        MachineBasicBlock::iterator eliminateCallFramePseudoInstr(MachineFunction &MF,
                                                                  MachineBasicBlock &MBB,
                                                                  MachineBasicBlock::iterator I)
        const override;
        bool hasFP(const MachineFunction &MF) const;

        //! Stack slot size (4 bytes)
        static int stackSlotSize() { return 4; }

    private:
        uint64_t computeStackSize(MachineFunction &MF) const;
    };
}

#endif //LLVM_LC3FRAMELOWERING_H
