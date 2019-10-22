
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include <llvm/CodeGen/SelectionDAGNodes.h>

#include "TL45InstrInfo.h"
#include "MCTargetDesc/TL45MCTargetDesc.h"

#define DEBUG_TYPE "tl45-instr-info"

#define GET_INSTRINFO_CTOR_DTOR
#include "TL45GenInstrInfo.inc"

using namespace llvm;

TL45InstrInfo::TL45InstrInfo() : TL45GenInstrInfo(TL45::ADJCALLSTACKDOWN, TL45::ADJCALLSTACKUP, -1, TL45::RET), RI() {}

void TL45InstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator I,
                                          unsigned SrcReg, bool IsKill, int FI,
                                          const TargetRegisterClass *RC,
                                          const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  unsigned Opcode;

  if (TL45::GRRegsRegClass.hasSubClassEq(RC))
    Opcode = TL45::SW;
  else
    llvm_unreachable("Can't store this register to stack slot");

  BuildMI(MBB, I, DL, get(Opcode))
      .addReg(SrcReg)
      .addFrameIndex(FI)
      .addImm(0);
}

void TL45InstrInfo::loadRegFromStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator I, unsigned DstReg,
    int FI, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  unsigned Opcode;

  if (TL45::GRRegsRegClass.hasSubClassEq(RC))
    Opcode = TL45::LW;
  else
    llvm_unreachable("Can't load this register from stack slot");

  BuildMI(MBB, I, DL, get(Opcode), DstReg).addFrameIndex(FI).addImm(0);
}

unsigned TL45InstrInfo::resolveComparison(MachineBasicBlock &MBB,
                                            MachineBasicBlock::iterator I,
                                            const DebugLoc &DL,
                                            ISD::CondCode ConditionCode,
                                            MachineOperand &a,
                                            MachineOperand &b,
                                            unsigned int &JmpOpcode) const {
  unsigned bytesAdded;
  // Recursive rewrite rules :^)))
  // a =  b  ==> !(a != b) ==> skpe a, b; jmp 1; jmp dst
  // a >  b  ==> b < a     ==> skplt b, a; jmp 1; jmp dst
  // a >= b  ==>               skplt a, b; jmp dst
  // a <  b  ==> !(a >= b) ==> skplt a, b; jmp 1; jmp dst
  // a <= b  ==> b >= a    ==>
  // a != b  ==> skpe a, b; jmp dst

  BuildMI(MBB, I, DL, get(TL45::CMP)).addReg(a.getReg()).addReg(b.getReg());
  bytesAdded += 1;

  switch (ConditionCode) {
  case ISD::CondCode::SETEQ:
  case ISD::CondCode::SETUEQ:
    JmpOpcode = TL45::JEI;
    break;
  case ISD::CondCode::SETGT:
    JmpOpcode = TL45::JGI;
    break;
  case ISD::CondCode::SETUGT:
    JmpOpcode = TL45::JAI;
    break;
  case ISD::CondCode::SETUGE:
    JmpOpcode = TL45::JNBI;
    break;
  case ISD::CondCode::SETGE:
    JmpOpcode = TL45::JGEI;
    break;
  case ISD::CondCode::SETLT:
    JmpOpcode = TL45::JLI;
    break;
  case ISD::CondCode::SETULT:
    JmpOpcode = TL45::JBI;
    break;
  case ISD::CondCode::SETLE:
    JmpOpcode = TL45::JLEI;
    break;
  case ISD::CondCode::SETULE:
    JmpOpcode = TL45::JBEI;
    break;
  case ISD::CondCode::SETNE:
  case ISD::CondCode::SETUNE:
    JmpOpcode = TL45::JNEI;
    break;
  default:
    LLVM_DEBUG(dbgs() << "Got cond code: " << ConditionCode << "\n");
    llvm_unreachable("dude weed lmao: how did we get such a condition code in "
                     "this pseudo instruction?! are we fLoATiNg?");
  }
  return bytesAdded;
}

bool TL45InstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
  const DebugLoc DL = MI.getDebugLoc();
  MachineBasicBlock &MBB = *MI.getParent();
  switch (MI.getOpcode()) {
  default:
    return false;

  case TL45::CMP_JMP: {
    auto ConditionCode = ISD::CondCode(MI.getOperand(0).getImm());
    unsigned int JmpOpcode;
    resolveComparison(MBB, MI, DL, ConditionCode, MI.getOperand(1), MI.getOperand(2), JmpOpcode);
    BuildMI(MBB, MI, DL, get(JmpOpcode)).add(MI.getOperand(3));
    break;
  }

//  case TL45::JMP: {
//    MachineOperand &op = MI.getOperand(0);
//    if (op.isMBB()) {
//      MachineBasicBlock *dst = op.getMBB();
//      BuildMI(MBB, MI, DL, get(TL45::GOTO)).addMBB(dst);
//    } else {
//      int64_t dst = op.getImm();
//      BuildMI(MBB, MI, DL, get(TL45::GOTO)).addImm(dst);
//    }
//    break;
//  }

//  case TL45::PseudoCALL: {
//    MachineOperand &op = MI.getOperand(0);
//
//    BuildMI(MBB, MI, DL, get(TL45::LEA)).addReg(TL45::at).add(op);
//    BuildMI(MBB, MI, DL, get(TL45::JALR)).addReg(TL45::ra).addReg(TL45::at);
//    break;
//  }

  //case TL45::PseudoRET: {
  //  break;
  //}
  }

  MBB.erase(MI);
  return true;
}

unsigned TL45InstrInfo::loadImmediate(unsigned FrameReg, int64_t Imm,
                                        MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator II,
                                        const DebugLoc &DL,
                                        unsigned &NewImm) const {

  llvm_unreachable("not yet implemented loadImmediate()");
}

bool TL45InstrInfo::validImmediate(unsigned Opcode, unsigned Reg,
                                     int64_t Amount) {
  return isInt<20>(Amount);
}

bool TL45InstrInfo::isAsCheapAsAMove(const MachineInstr &MI) const {
  const unsigned Opcode = MI.getOpcode();
  switch (Opcode) {
  default:
    break;
//  case TL45::ADD:
//    return (MI.getOperand(1).isReg() &&
//            MI.getOperand(1).getReg() == TL45::zero);
  }
  return MI.isAsCheapAsAMove();
}

void TL45InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator MI,
                                  const DebugLoc &DL, unsigned DestReg,
                                  unsigned SrcReg, bool KillSrc) const {
  BuildMI(MBB, MI, DL, get(TL45::ADD))
      .addReg(DestReg)
      .addReg(SrcReg)
      .addReg(TL45::r0);
}

unsigned TL45InstrInfo::insertBranch(
    MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB,
    ArrayRef<MachineOperand> Cond, const DebugLoc &DL, int *BytesAdded) const {
  if (BytesAdded)
    *BytesAdded = 0;

  if (Cond.empty()) {
    assert(!FBB && "Unconditional branch with multiple successors!");
    auto &MI = *BuildMI(&MBB, DL, get(TL45::JMP)).addMBB(TBB);
    if (BytesAdded)
      *BytesAdded += 4;
    return 1;
  }

  // Conditional branch, do the skips
  auto ConditionCode = (ISD::CondCode)Cond[0].getImm();
  auto a = Cond[1];
  auto b = Cond[2];
  // TODO Might not work

  unsigned FirstJmpOpcode;

  unsigned Count = resolveComparison(MBB, MBB.end(), DL, ConditionCode, a, b, FirstJmpOpcode);
  if (BytesAdded)
    *BytesAdded += (int)Count * 4;

  // True branch instruction
  BuildMI(MBB, MBB.end(), DL, get(FirstJmpOpcode)).addMBB(TBB);
  if (BytesAdded)
    *BytesAdded += 4;
  Count++;

  if (FBB) {
    // Two-way Conditional branch. Insert the second branch.
    BuildMI(MBB, MBB.end(), DL, get(TL45::JMPI)).addMBB(FBB);
    if (BytesAdded)
      *BytesAdded += 4;
    Count++;
  }

  return Count;
}

static unsigned int BrOpcode[] = {
    TL45::JO, TL45::JOI, TL45::JNO, TL45::JNOI, TL45::JS, TL45::JSI, TL45::JNS, TL45::JNSI,
    TL45::JE, TL45::JEI, TL45::JNE, TL45::JNEI, TL45::JB, TL45::JBI, TL45::JNB, TL45::JNBI,

    TL45::JBE, TL45::JBEI, TL45::JA, TL45::JAI, TL45::JL, TL45::JLI, TL45::JGE, TL45::JGEI,
    TL45::JLE, TL45::JLEI, TL45::JG, TL45::JGI, TL45::JMP, TL45::JMPI, 0
    };

static bool getAnalyzableBrOpc(unsigned Opc) {

  for (unsigned o : BrOpcode) {
    if (o == Opc) {
      return true;
    }
  }

  return false;
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
bool TL45InstrInfo::analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB, MachineBasicBlock *&FBB,
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
  if (!SecondLastOpc || !getAnalyzableBrOpc(SecondLastOpc)) {
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

unsigned TL45InstrInfo::removeBranch(MachineBasicBlock &MBB,
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
