
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include <llvm/CodeGen/SelectionDAGNodes.h>

#include "ECE2031InstrInfo.h"
#include "MCTargetDesc/ECE2031MCTargetDesc.h"

#define DEBUG_TYPE "lc2200-instr-info"

#define GET_INSTRINFO_CTOR_DTOR
#include "ECE2031GenInstrInfo.inc"

using namespace llvm;

ECE2031InstrInfo::ECE2031InstrInfo() : ECE2031GenInstrInfo(), RI() {}

void ECE2031InstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator I,
                                          unsigned SrcReg, bool IsKill, int FI,
                                          const TargetRegisterClass *RC,
                                          const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  unsigned Opcode;

  if (ECE2031::GRRegsRegClass.hasSubClassEq(RC))
    Opcode = ECE2031::SW;
  else
    llvm_unreachable("Can't store this register to stack slot");

  BuildMI(MBB, I, DL, get(Opcode))
      .addReg(SrcReg)
      .addFrameIndex(FI)
      .addImm(0);
}

void ECE2031InstrInfo::loadRegFromStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator I, unsigned DstReg,
    int FI, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  unsigned Opcode;

  if (ECE2031::GRRegsRegClass.hasSubClassEq(RC))
    Opcode = ECE2031::LW;
  else
    llvm_unreachable("Can't load this register from stack slot");

  BuildMI(MBB, I, DL, get(Opcode), DstReg).addFrameIndex(FI).addImm(0);
}

unsigned ECE2031InstrInfo::resolveComparison(MachineBasicBlock &MBB,
                                            MachineBasicBlock::iterator I,
                                            const DebugLoc &DL,
                                            ISD::CondCode ConditionCode,
                                            MachineOperand &a,
                                            MachineOperand &b) const {
  unsigned bytesAdded;
  // Recursive rewrite rules :^)))
  // a =  b  ==> !(a != b) ==> skpe a, b; jmp 1; jmp dst
  // a >  b  ==> b < a     ==> skplt b, a; jmp 1; jmp dst
  // a >= b  ==>               skplt a, b; jmp dst
  // a <  b  ==> !(a >= b) ==> skplt a, b; jmp 1; jmp dst
  // a <= b  ==> b >= a    ==>
  // a != b  ==> skpe a, b; jmp dst
  switch (ConditionCode) {
  case ISD::CondCode::SETEQ:
  case ISD::CondCode::SETUEQ:
    bytesAdded = resolveComparison(MBB, I, DL, ISD::CondCode::SETNE, a, b) + 1;
    BuildMI(MBB, I, DL, get(ECE2031::GOTO)).addImm(1);
    break;
  case ISD::CondCode::SETGT:
//  case ISD::CondCode::SETUGT: // using this for unordered might be mathematically incorrect
    bytesAdded = resolveComparison(MBB, I, DL, ISD::CondCode::SETLT, b, a);
    break;
//  case ISD::CondCode::SETUGE: // using this for unordered might be mathematically incorrect
  case ISD::CondCode::SETGE:
    BuildMI(MBB, I, DL, get(ECE2031::SKPLT)).addReg(a.getReg()).addReg(b.getReg());
    bytesAdded = 1;
    break;
  case ISD::CondCode::SETLT:
//  case ISD::CondCode::SETULT: // using this for unordered might be mathematically incorrect
    bytesAdded = resolveComparison(MBB, I, DL, ISD::CondCode::SETGE, a, b) + 1;
    BuildMI(MBB, I, DL, get(ECE2031::GOTO)).addImm(1);
    break;
  case ISD::CondCode::SETLE:
//  case ISD::CondCode::SETULE: // using this for unordered might be mathematically incorrect
    bytesAdded = resolveComparison(MBB, I, DL, ISD::CondCode::SETGE, b, a);
    break;
  case ISD::CondCode::SETNE:
  case ISD::CondCode::SETUNE:
    BuildMI(MBB, I, DL, get(ECE2031::SKPE)).addReg(a.getReg()).addReg(b.getReg());
    bytesAdded = 1;
    break;
  default:
    LLVM_DEBUG(dbgs() << "Got cond code: " << ConditionCode << "\n");
    llvm_unreachable("dude weed lmao: how did we get such a condition code in "
                     "this pseudo instruction?! are we fLoATiNg?");
  }
  return bytesAdded;
}

bool ECE2031InstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
  const DebugLoc DL = MI.getDebugLoc();
  MachineBasicBlock &MBB = *MI.getParent();
  switch (MI.getOpcode()) {
  default:
    return false;

  case ECE2031::CMP_SKIP: {
    auto ConditionCode = ISD::CondCode(MI.getOperand(0).getImm());
    resolveComparison(MBB, MI, DL, ConditionCode, MI.getOperand(1), MI.getOperand(2));
    break;
  }
  case ECE2031::CMP_JMP: {
    auto ConditionCode = ISD::CondCode(MI.getOperand(0).getImm());
    resolveComparison(MBB, MI, DL, ConditionCode, MI.getOperand(1), MI.getOperand(2));
    BuildMI(MBB, MI, DL, get(ECE2031::GOTO)).add(MI.getOperand(3));
    break;
  }
  case ECE2031::SELECT_MOVE: {
    auto Dst = MI.getOperand(0).getReg();
    auto TrueValue = MI.getOperand(1).getReg();
    auto FalseValue = MI.getOperand(2).getReg();
    BuildMI(MBB, MI, DL, get(ECE2031::GOTO)).addImm(2);
    BuildMI(MBB, MI, DL, get(ECE2031::ADD)).addReg(Dst).addReg(ECE2031::zero).addReg(FalseValue);
    BuildMI(MBB, MI, DL, get(ECE2031::GOTO)).addImm(1);
    BuildMI(MBB, MI, DL, get(ECE2031::ADD)).addReg(Dst).addReg(ECE2031::zero).addReg(TrueValue);
//    BuildMI(MBB, MI, DL, get(ECE2031::ADD)).addReg(Dest).addReg(ECE2031::zero).addReg(FalseValue);
//    BuildMI(MBB, MI, DL, get(ECE2031::ADD)).addReg(Dest).addReg(ECE2031::zero).addReg(TrueValue);
    break;
  }
  case ECE2031::CMP_SELECT_MOVE: {
    auto Dst = MI.getOperand(0).getReg();

    auto ConditionCode = ISD::CondCode(MI.getOperand(1).getImm());

    auto TrueValue = MI.getOperand(4).getReg();
    auto FalseValue = MI.getOperand(5).getReg();

    resolveComparison(MBB, MI, DL, ConditionCode, MI.getOperand(2), MI.getOperand(3));

    BuildMI(MBB, MI, DL, get(ECE2031::GOTO)).addImm(2);
    BuildMI(MBB, MI, DL, get(ECE2031::ADD)).addReg(Dst).addReg(ECE2031::zero).addReg(FalseValue);
    BuildMI(MBB, MI, DL, get(ECE2031::GOTO)).addImm(1);
    BuildMI(MBB, MI, DL, get(ECE2031::ADD)).addReg(Dst).addReg(ECE2031::zero).addReg(TrueValue);
//    BuildMI(MBB, MI, DL, get(ECE2031::ADD)).addReg(Dest).addReg(ECE2031::zero).addReg(FalseValue);
//    BuildMI(MBB, MI, DL, get(ECE2031::ADD)).addReg(Dest).addReg(ECE2031::zero).addReg(TrueValue);
    break;
  }

  case ECE2031::JMP: {
    MachineOperand &op = MI.getOperand(0);
    if (op.isMBB()) {
      MachineBasicBlock *dst = op.getMBB();
      BuildMI(MBB, MI, DL, get(ECE2031::GOTO)).addMBB(dst);
    } else {
      int64_t dst = op.getImm();
      BuildMI(MBB, MI, DL, get(ECE2031::GOTO)).addImm(dst);
    }
    break;
  }

  case ECE2031::PseudoCALL: {
    MachineOperand &op = MI.getOperand(0);
    BuildMI(MBB, MI, DL, get(ECE2031::ADDI)).addReg(ECE2031::sp).addReg(ECE2031::sp).addImm(-1);
    BuildMI(MBB, MI, DL, get(ECE2031::SW)).addReg(ECE2031::ra).addReg(ECE2031::sp).addImm(0);
    BuildMI(MBB, MI, DL, get(ECE2031::LEA)).addReg(ECE2031::at).add(op);
    BuildMI(MBB, MI, DL, get(ECE2031::JALR)).addReg(ECE2031::ra).addReg(ECE2031::at);
    BuildMI(MBB, MI, DL, get(ECE2031::LW)).addReg(ECE2031::ra).addReg(ECE2031::sp).addImm(0);
    BuildMI(MBB, MI, DL, get(ECE2031::ADDI)).addReg(ECE2031::sp).addReg(ECE2031::sp).addImm(1);
    break;
  }

  //case ECE2031::PseudoRET: {
  //  break;
  //}
  }

  MBB.erase(MI);
  return true;
}

unsigned ECE2031InstrInfo::loadImmediate(unsigned FrameReg, int64_t Imm,
                                        MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator II,
                                        const DebugLoc &DL,
                                        unsigned &NewImm) const {

  llvm_unreachable("not yet implemented loadImmediate()");
}

bool ECE2031InstrInfo::validImmediate(unsigned Opcode, unsigned Reg,
                                     int64_t Amount) {
  return isInt<20>(Amount);
}

bool ECE2031InstrInfo::isAsCheapAsAMove(const MachineInstr &MI) const {
  const unsigned Opcode = MI.getOpcode();
  switch (Opcode) {
  default:
    break;
  case ECE2031::ADDI:
    return (MI.getOperand(1).isReg() &&
            MI.getOperand(1).getReg() == ECE2031::zero);
  }
  return MI.isAsCheapAsAMove();
}

void ECE2031InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator MI,
                                  const DebugLoc &DL, unsigned DestReg,
                                  unsigned SrcReg, bool KillSrc) const {
  BuildMI(MBB, MI, DL, get(ECE2031::ADD))
      .addReg(DestReg)
      .addReg(SrcReg)
      .addReg(ECE2031::zero);
}

unsigned ECE2031InstrInfo::insertBranch(
    MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB,
    ArrayRef<MachineOperand> Cond, const DebugLoc &DL, int *BytesAdded) const {
  if (BytesAdded)
    *BytesAdded = 0;

  if (Cond.empty()) {
    assert(!FBB && "Unconditional branch with multiple successors!");
    auto &MI = *BuildMI(&MBB, DL, get(ECE2031::GOTO)).addMBB(TBB);
    if (BytesAdded)
      *BytesAdded += 4;
    return 1;
  }

  // Conditional branch, do the skips
  auto ConditionCode = (ISD::CondCode)Cond[0].getImm();
  auto a = Cond[1];
  auto b = Cond[2];
  // TODO Might not work
  unsigned Count = resolveComparison(MBB, MBB.end(), DL, ConditionCode, a, b);
  if (BytesAdded)
    *BytesAdded += (int)Count * 4;

  // True branch instruction
  BuildMI(MBB, MBB.end(), DL, get(ECE2031::GOTO)).addMBB(TBB);
  if (BytesAdded)
    *BytesAdded += 4;
  Count++;

  if (FBB) {
    // Two-way Conditional branch. Insert the second branch.
    BuildMI(MBB, MBB.end(), DL, get(ECE2031::GOTO)).addMBB(FBB);
    if (BytesAdded)
      *BytesAdded += 4;
    Count++;
  }

  return Count;
}

static bool getAnalyzableBrOpc(unsigned Opc) {
  return Opc == ECE2031::JMP || Opc == ECE2031::CMP_JMP || Opc == ECE2031::GOTO;
}

static void AnalyzeCondBr(const MachineInstr *Inst, unsigned Opc,
                                  MachineBasicBlock *&BB,
                                  SmallVectorImpl<MachineOperand> &Cond) {
  assert(getAnalyzableBrOpc(Opc) && "Not an analyzable branch");
  int NumOp = Inst->getNumExplicitOperands();

  // for both int and fp branches, the last explicit operand is the
  // MBB.
  BB = Inst->getOperand(NumOp-1).getMBB();
  // Cond.push_back(MachineOperand::CreateImm(Opc));

  for (int i = 0; i < NumOp-1; i++)
    Cond.push_back(Inst->getOperand(i));
}

/// Copied From TargetInstrInfo.h:
///
/// Analyze the branching code at the end of MBB, returning
/// true if it cannot be understood (e.g. it's a switch dispatch or isn't
/// implemented for a target).  Upon success, this returns false and returns
/// with the following information in various cases:
///
/// 1. If this block ends with no branches (it just falls through to its succ)
///    just return false, leaving TBB/FBB null.
/// 2. If this block ends with only an unconditional branch, it sets TBB to be
///    the destination block.
/// 3. If this block ends with a conditional branch and it falls through to a
///    successor block, it sets TBB to be the branch destination block and a
///    list of operands that evaluate the condition. These operands can be
///    passed to other TargetInstrInfo methods to create new branches.
/// 4. If this block ends with a conditional branch followed by an
///    unconditional branch, it returns the 'true' destination in TBB, the
///    'false' destination in FBB, and a list of operands that evaluate the
///    condition.  These operands can be passed to other TargetInstrInfo
///    methods to create new branches.
///
/// Note that removeBranch and insertBranch must be implemented to support
/// cases where this method returns success.
///
/// If AllowModify is true, then this routine is allowed to modify the basic
/// block (e.g. delete instructions after the unconditional branch).
///
/// The CFG information in MBB.Predecessors and MBB.Successors must be valid
/// before calling this function.
bool ECE2031InstrInfo::
analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB, MachineBasicBlock *&FBB,
                                    SmallVectorImpl<MachineOperand> &Cond, bool AllowModify) const {
  MachineBasicBlock::reverse_iterator I = MBB.rbegin(), REnd = MBB.rend();

  // Skip all the debug instructions.
  while (I != REnd && I->isDebugInstr())
    ++I;

  if (I == REnd || !isUnpredicatedTerminator(*I)) {
    // This block ends with no branches (it just falls through to its succ).
    // Leave TBB/FBB null.
    TBB = FBB = nullptr;
    return false;
  }

  SmallVector<MachineInstr *, 4> BranchInstrs;

  MachineInstr *LastInst = &*I;
  unsigned LastOpc = LastInst->getOpcode();
  BranchInstrs.push_back(LastInst);


  // Not an analyzable branch (e.g., indirect jump).
  if (!getAnalyzableBrOpc(LastOpc))
    return true;

  // Get the second to last instruction in the block.
  unsigned SecondLastOpc = 0;
  MachineInstr *SecondLastInst = nullptr;

  // Skip past any debug instruction to see if the second last actual
  // is a branch.
  ++I;
  while (I != REnd && I->isDebugInstr())
    ++I;

  if (I != REnd) {
    SecondLastInst = &*I;
    SecondLastOpc = SecondLastInst->getOpcode();

    // Not an analyzable branch (must be an indirect jump).
    if (isUnpredicatedTerminator(*SecondLastInst) && !getAnalyzableBrOpc(SecondLastOpc))
      return true;
  }

  // If there is only one terminator instruction, process it.
  if (!SecondLastOpc || (SecondLastOpc != ECE2031::JMP && SecondLastOpc != ECE2031::CMP_JMP && SecondLastOpc != ECE2031::GOTO)) {
    // Unconditional branch.
    if (LastInst->isUnconditionalBranch()) {
      TBB = LastInst->getOperand(0).getMBB();
      return false;
    }

    // Conditional branch
    AnalyzeCondBr(LastInst, LastOpc, TBB, Cond);
    return false;
  }

  // If we reached here, there are two branches.
  // If there are three terminators, we don't know what sort of block this is.
  if (++I != REnd && isUnpredicatedTerminator(*I))
    return true;

  BranchInstrs.insert(BranchInstrs.begin(), SecondLastInst);

  // If second to last instruction is an unconditional branch,
  // analyze it and remove the last instruction.
  if (SecondLastInst->isUnconditionalBranch()) {
    if (!AllowModify) {
      return true;
    }

    TBB = SecondLastInst->getOperand(0).getMBB();
    LastInst->eraseFromParent();
    BranchInstrs.pop_back();
    return false;
  }

  // Conditional branch followed by an unconditional branch.
  // The last one must be unconditional.
  if (!LastInst->isUnconditionalBranch())
    return true;

  AnalyzeCondBr(SecondLastInst, SecondLastOpc, TBB, Cond);
  FBB = LastInst->getOperand(0).getMBB();

  return false;
}

unsigned ECE2031InstrInfo::removeBranch(MachineBasicBlock &MBB,
                                     int *BytesRemoved) const {
  assert(!BytesRemoved && "code size not handled");

  MachineBasicBlock::reverse_iterator I = MBB.rbegin(), REnd = MBB.rend();
  unsigned removed = 0;

  // Up to 2 branches are removed.
  // Note that indirect branches are not removed.
  while (I != REnd && removed < 2) {
    // Skip past debug instructions.
    if (I->isDebugInstr()) {
      ++I;
      continue;
    }
    if (!getAnalyzableBrOpc(I->getOpcode()))
      break;
    // Remove the branch.
    I->eraseFromParent();
    I = MBB.rbegin();
    ++removed;
  }

  return removed;
}
