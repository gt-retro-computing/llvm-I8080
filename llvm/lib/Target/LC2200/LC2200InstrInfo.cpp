
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include <llvm/CodeGen/SelectionDAGNodes.h>

#include "LC2200InstrInfo.h"
#include "MCTargetDesc/LC2200MCTargetDesc.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "LC2200GenInstrInfo.inc"

using namespace llvm;

LC2200InstrInfo::LC2200InstrInfo() : LC2200GenInstrInfo(), RI() {}

void LC2200InstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator I,
                                          unsigned SrcReg, bool IsKill, int FI,
                                          const TargetRegisterClass *RC,
                                          const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  unsigned Opcode;

  if (LC2200::GRRegsRegClass.hasSubClassEq(RC))
    Opcode = LC2200::SW;
  else
    llvm_unreachable("Can't store this register to stack slot");

  BuildMI(MBB, I, DL, get(Opcode))
      .addReg(SrcReg)
      .addFrameIndex(FI)
      .addImm(0);
}

void LC2200InstrInfo::loadRegFromStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator I, unsigned DstReg,
    int FI, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  unsigned Opcode;

  if (LC2200::GRRegsRegClass.hasSubClassEq(RC))
    Opcode = LC2200::LW;
  else
    llvm_unreachable("Can't load this register from stack slot");

  BuildMI(MBB, I, DL, get(Opcode), DstReg).addFrameIndex(FI).addImm(0);
}

unsigned LC2200InstrInfo::resolveComparison(MachineBasicBlock *MBB,
                                            const DebugLoc &DL,
                                            ISD::CondCode ConditionCode,
                                            MachineOperand &a,
                                            MachineOperand &b) const {
  unsigned bytesAdded;
  // Recursive rewrite rules :^)))
  // a =  b  ==> !(a != b)
  // a >  b  ==> b < a
  // a >= b  ==> skplt a, b; jmp dst
  // a <  b  ==> !(a >= b)
  // a <= b  ==> b >= a
  // a != b  ==> skpe a, b; jmp dst
  switch (ConditionCode) {
  case ISD::CondCode::SETEQ:
    bytesAdded = resolveComparison(MBB, DL, ISD::CondCode::SETNE, a, b) + 1;
    BuildMI(MBB, DL, get(LC2200::GOTO)).addImm(1);
    break;
  case ISD::CondCode::SETGT:
    bytesAdded = resolveComparison(MBB, DL, ISD::CondCode::SETLT, b, a);
    break;
  case ISD::CondCode::SETGE:
    BuildMI(MBB, DL, get(LC2200::SKPLT)).addReg(a.getReg()).addReg(b.getReg());
    bytesAdded = 1;
    break;
  case ISD::CondCode::SETLT:
    bytesAdded = resolveComparison(MBB, DL, ISD::CondCode::SETGE, a, b) + 1;
    BuildMI(MBB, DL, get(LC2200::GOTO)).addImm(1);
    break;
  case ISD::CondCode::SETLE:
    bytesAdded = resolveComparison(MBB, DL, ISD::CondCode::SETGE, b, a);
    break;
  case ISD::CondCode::SETNE:
    BuildMI(MBB, DL, get(LC2200::SKPE)).addReg(a.getReg()).addReg(b.getReg());
    bytesAdded = 1;
    break;
  default:
    llvm_unreachable("dude weed lmao: how did we get such a condition code in "
                     "this pseudo instruction?! are we fLoATiNg?");
  }
  return bytesAdded;
}

bool LC2200InstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
  DebugLoc DL = MI.getDebugLoc();
  MachineBasicBlock *MBB = MI.getParent();
  switch (MI.getOpcode()) {
  default:
    return false;

  case LC2200::CMP_SKIP: {
    auto ConditionCode = ISD::CondCode(MI.getOperand(0).getImm());
    resolveComparison(MBB, DL, ConditionCode, MI.getOperand(1),
                      MI.getOperand(2));
    break;
  }

  case LC2200::SELECT_MOVE: {
    auto TrueValue = MI.getOperand(0).getReg();
    auto FalseValue = MI.getOperand(1).getReg();
    BuildMI(MBB, DL, get(LC2200::COPY)).addReg(FalseValue);
    BuildMI(MBB, DL, get(LC2200::COPY)).addReg(TrueValue);
    break;
  }

  case LC2200::JMP: {
    MachineOperand &op = MI.getOperand(0);
    if (op.isMBB()) {
      MachineBasicBlock *dst = op.getMBB();
      BuildMI(MBB, DL, get(LC2200::GOTO)).addMBB(dst);
    } else {
      int64_t dst = op.getImm();
      BuildMI(MBB, DL, get(LC2200::GOTO)).addImm(dst);
    }
    break;
  }

  case LC2200::PseudoCALL: {
    MachineOperand &op = MI.getOperand(0);
    if (!op.isGlobal())
      llvm_unreachable("dude weed lmao");
    BuildMI(MBB, DL, get(LC2200::ADDI)).addReg(LC2200::at).addReg(LC2200::zero).addGlobalAddress(op.getGlobal());
//    BuildMI(MBB, DL, get(LC2200::COPY)).addReg(LC2200::at).addGlobalAddress(op.getGlobal());
    BuildMI(MBB, DL, get(LC2200::JALR)).addReg(LC2200::ra).addReg(LC2200::at);
    break;
  }

  //case LC2200::PseudoRET: {
  //  break;
  //}
  }

  MBB->erase(MI);
  return true;
}

unsigned LC2200InstrInfo::loadImmediate(unsigned FrameReg, int64_t Imm,
                                        MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator II,
                                        const DebugLoc &DL,
                                        unsigned &NewImm) const {

  llvm_unreachable("not yet implemented loadImmediate()");
}

bool LC2200InstrInfo::validImmediate(unsigned Opcode, unsigned Reg,
                                     int64_t Amount) {
  return isInt<20>(Amount);
}

bool LC2200InstrInfo::isAsCheapAsAMove(const MachineInstr &MI) const {
  const unsigned Opcode = MI.getOpcode();
  switch (Opcode) {
  default:
    break;
  case LC2200::ADDI:
    return (MI.getOperand(1).isReg() &&
            MI.getOperand(1).getReg() == LC2200::zero);
  }
  return MI.isAsCheapAsAMove();
}

void LC2200InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator MI,
                                  const DebugLoc &DL, unsigned DestReg,
                                  unsigned SrcReg, bool KillSrc) const {
  BuildMI(MBB, MI, DL, get(LC2200::ADD))
      .addReg(DestReg)
      .addReg(SrcReg)
      .addReg(LC2200::zero);
}

unsigned LC2200InstrInfo::insertBranch(
    MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB,
    ArrayRef<MachineOperand> Cond, const DebugLoc &DL, int *BytesAdded) const {
  if (BytesAdded)
    *BytesAdded = 0;

  if (Cond.empty()) {
    assert(!FBB && "Unconditional branch with multiple successors!");
    auto &MI = *BuildMI(&MBB, DL, get(LC2200::GOTO)).addMBB(TBB);
    if (BytesAdded)
      *BytesAdded += 4;
    return 1;
  }

  // Conditional branch, do the skips
  auto ConditionCode = (ISD::CondCode)Cond[0].getImm();
  auto a = Cond[1];
  auto b = Cond[2];
  unsigned Count = resolveComparison(&MBB, DL, ConditionCode, a, b);
  if (BytesAdded)
    *BytesAdded += (int)Count * 4;

  // True branch instruction
  BuildMI(&MBB, DL, get(LC2200::GOTO)).addMBB(TBB);
  if (BytesAdded)
    *BytesAdded += 4;
  Count++;

  if (FBB) {
    // Two-way Conditional branch. Insert the second branch.
    BuildMI(&MBB, DL, get(LC2200::GOTO)).addMBB(FBB);
    if (BytesAdded)
      *BytesAdded += 4;
    Count++;
  }

  return Count;
}
