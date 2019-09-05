
#include "LC2200TargetMachine.h"
#include "LC2200ISelLowering.h"

#define DEBUG_TYPE "lc2200-target-lowering"

using namespace llvm;


LC2200TargetLowering::LC2200TargetLowering(const LC2200TargetMachine &TM, const LC2200Subtarget &STI) :
  TargetLowering(TM), Subtarget(STI) {

}

void LC2200TargetLowering::analyzeInputArgs(
        MachineFunction &MF, CCState &CCInfo,
        const SmallVectorImpl<ISD::InputArg> &Ins, bool IsRet) const {
  unsigned NumArgs = Ins.size();
  FunctionType *FType = MF.getFunction().getFunctionType();

  for (unsigned i = 0; i != NumArgs; ++i) {
    MVT ArgVT = Ins[i].VT;
    ISD::ArgFlagsTy ArgFlags = Ins[i].Flags;

    Type *ArgTy = nullptr;
    if (IsRet)
      ArgTy = FType->getReturnType();
    else if (Ins[i].isOrigArg())
      ArgTy = FType->getParamType(Ins[i].getOrigArgIndex());

    LC2200ABI::ABI ABI = MF.getSubtarget<LC2200Subtarget>().getTargetABI();
    if (CC_LC2200(MF.getDataLayout(), ABI, i, ArgVT, ArgVT, CCValAssign::Full,
                 ArgFlags, CCInfo, /*IsRet=*/true, IsRet, ArgTy)) {
      LLVM_DEBUG(dbgs() << "InputArg #" << i << " has unhandled type "
                        << EVT(ArgVT).getEVTString() << '\n');
      llvm_unreachable(nullptr);
    }
  }
}

// Convert Val to a ValVT. Should not be called for CCValAssign::Indirect
// values.
static SDValue convertLocVTToValVT(SelectionDAG &DAG, SDValue Val,
                                   const CCValAssign &VA, const SDLoc &DL) {
  switch (VA.getLocInfo()) {
    default:
      llvm_unreachable("Unexpected CCValAssign::LocInfo");
    case CCValAssign::Full:
      break;
    case CCValAssign::BCvt:
//      if (VA.getLocVT() == MVT::i64 && VA.getValVT() == MVT::f32) {
//        Val = DAG.getNode(RISCVISD::FMV_W_X_RV64, DL, MVT::f32, Val);
//        break;
//      }
      Val = DAG.getNode(ISD::BITCAST, DL, VA.getValVT(), Val);
      break;
  }
  return Val;
}

// The caller is responsible for loading the full value if the argument is
// passed with CCValAssign::Indirect.
static SDValue unpackFromRegLoc(SelectionDAG &DAG, SDValue Chain,
                                const CCValAssign &VA, const SDLoc &DL) {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();
  EVT LocVT = VA.getLocVT();
  SDValue Val;
  const TargetRegisterClass *RC;

  switch (LocVT.getSimpleVT().SimpleTy) {
    default:
      llvm_unreachable("Unexpected register type");
    case MVT::i32:
      RC = &LC2200::GRRegsRegClass;
      break;
  }

  Register VReg = RegInfo.createVirtualRegister(RC);
  RegInfo.addLiveIn(VA.getLocReg(), VReg);
  Val = DAG.getCopyFromReg(Chain, DL, VReg, LocVT);

  if (VA.getLocInfo() == CCValAssign::Indirect)
    return Val;

  return convertLocVTToValVT(DAG, Val, VA, DL);
}

static SDValue convertValVTToLocVT(SelectionDAG &DAG, SDValue Val,
                                   const CCValAssign &VA, const SDLoc &DL) {
  EVT LocVT = VA.getLocVT();

  switch (VA.getLocInfo()) {
    default:
      llvm_unreachable("Unexpected CCValAssign::LocInfo");
    case CCValAssign::Full:
      break;
    case CCValAssign::BCvt:
//      if (VA.getLocVT() == MVT::i64 && VA.getValVT() == MVT::f32) {
//        Val = DAG.getNode(RISCVISD::FMV_X_ANYEXTW_RV64, DL, MVT::i64, Val);
//        break;
//      }
      Val = DAG.getNode(ISD::BITCAST, DL, LocVT, Val);
      break;
  }
  return Val;
}

// The caller is responsible for loading the full value if the argument is
// passed with CCValAssign::Indirect.
static SDValue unpackFromMemLoc(SelectionDAG &DAG, SDValue Chain,
                                const CCValAssign &VA, const SDLoc &DL) {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  EVT LocVT = VA.getLocVT();
  EVT ValVT = VA.getValVT();
  EVT PtrVT = MVT::getIntegerVT(DAG.getDataLayout().getPointerSizeInBits(0));
  int FI = MFI.CreateFixedObject(ValVT.getSizeInBits() / 8,
                                 VA.getLocMemOffset(), /*Immutable=*/true);
  SDValue FIN = DAG.getFrameIndex(FI, PtrVT);
  SDValue Val;

  ISD::LoadExtType ExtType;
  switch (VA.getLocInfo()) {
    default:
      llvm_unreachable("Unexpected CCValAssign::LocInfo");
    case CCValAssign::Full:
    case CCValAssign::Indirect:
    case CCValAssign::BCvt:
      ExtType = ISD::NON_EXTLOAD;
      break;
  }
  Val = DAG.getExtLoad(
          ExtType, DL, LocVT, Chain, FIN,
          MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FI), ValVT);
  return Val;
}

// Transform physical registers into virtual registers.
SDValue LC2200TargetLowering::LowerFormalArguments(
        SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
        const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
        SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {

  assert(!IsVarArg && "var arg not yet supported");

  switch (CallConv) {
    default:
      report_fatal_error("Unsupported calling convention");
    case CallingConv::C:
    case CallingConv::Fast:
      break;
  }

  MachineFunction &MF = DAG.getMachineFunction();

  const Function &Func = MF.getFunction();
  if (Func.hasFnAttribute("interrupt")) {
    if (!Func.arg_empty())
      report_fatal_error(
              "Functions with the interrupt attribute cannot have arguments!");

    StringRef Kind =
            MF.getFunction().getFnAttribute("interrupt").getValueAsString();

    if (!(Kind == "user" || Kind == "supervisor" || Kind == "machine"))
      report_fatal_error(
              "Function interrupt attribute argument not supported!");
  }

  EVT PtrVT = getPointerTy(DAG.getDataLayout());
  MVT XLenVT = MVT::i32;
  unsigned XLenInBytes = 32 / 8;
  // Used with vargs to acumulate store chains.
  std::vector<SDValue> OutChains;

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  analyzeInputArgs(MF, CCInfo, Ins, /*IsRet=*/false);

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue ArgValue;
    // Passing f64 on RV32D with a soft float ABI must be handled as a special
    // case.
//    if (VA.getLocVT() == MVT::i32 && VA.getValVT() == MVT::f64)
//      ArgValue = unpackF64OnRV32DSoftABI(DAG, Chain, VA, DL);
//    else
    if (VA.isRegLoc())
      ArgValue = unpackFromRegLoc(DAG, Chain, VA, DL);
    else
      ArgValue = unpackFromMemLoc(DAG, Chain, VA, DL);

    if (VA.getLocInfo() == CCValAssign::Indirect) {
      // If the original argument was split and passed by reference (e.g. i128
      // on RV32), we need to load all parts of it here (using the same
      // address).
      InVals.push_back(DAG.getLoad(VA.getValVT(), DL, Chain, ArgValue,
                                   MachinePointerInfo()));
      unsigned ArgIndex = Ins[i].OrigArgIndex;
      assert(Ins[i].PartOffset == 0);
      while (i + 1 != e && Ins[i + 1].OrigArgIndex == ArgIndex) {
        CCValAssign &PartVA = ArgLocs[i + 1];
        unsigned PartOffset = Ins[i + 1].PartOffset;
        SDValue Address = DAG.getNode(ISD::ADD, DL, PtrVT, ArgValue,
                                      DAG.getIntPtrConstant(PartOffset, DL));
        InVals.push_back(DAG.getLoad(PartVA.getValVT(), DL, Chain, Address,
                                     MachinePointerInfo()));
        ++i;
      }
      continue;
    }
    InVals.push_back(ArgValue);
  }

//  if (IsVarArg) {
//    ArrayRef<MCPhysReg> ArgRegs = makeArrayRef(ArgGPRs);
//    unsigned Idx = CCInfo.getFirstUnallocated(ArgRegs);
//    const TargetRegisterClass *RC = &RISCV::GPRRegClass;
//    MachineFrameInfo &MFI = MF.getFrameInfo();
//    MachineRegisterInfo &RegInfo = MF.getRegInfo();
////    RISCVMachineFunctionInfo *RVFI = MF.getInfo<RISCVMachineFunctionInfo>();
//
//    // Offset of the first variable argument from stack pointer, and size of
//    // the vararg save area. For now, the varargs save area is either zero or
//    // large enough to hold a0-a7.
//    int VaArgOffset, VarArgsSaveSize;
//
//    // If all registers are allocated, then all varargs must be passed on the
//    // stack and we don't need to save any argregs.
//    if (ArgRegs.size() == Idx) {
//      VaArgOffset = CCInfo.getNextStackOffset();
//      VarArgsSaveSize = 0;
//    } else {
//      VarArgsSaveSize = XLenInBytes * (ArgRegs.size() - Idx);
//      VaArgOffset = -VarArgsSaveSize;
//    }
//
//    // Record the frame index of the first variable argument
//    // which is a value necessary to VASTART.
//    int FI = MFI.CreateFixedObject(XLenInBytes, VaArgOffset, true);
////    RVFI->setVarArgsFrameIndex(FI);
//
//    // If saving an odd number of registers then create an extra stack slot to
//    // ensure that the frame pointer is 2*XLEN-aligned, which in turn ensures
//    // offsets to even-numbered registered remain 2*XLEN-aligned.
//    if (Idx % 2) {
//      FI = MFI.CreateFixedObject(XLenInBytes, VaArgOffset - (int)XLenInBytes,
//                                 true);
//      VarArgsSaveSize += XLenInBytes;
//    }
//
//    // Copy the integer registers that may have been used for passing varargs
//    // to the vararg save area.
//    for (unsigned I = Idx; I < ArgRegs.size();
//         ++I, VaArgOffset += XLenInBytes) {
//      const Register Reg = RegInfo.createVirtualRegister(RC);
//      RegInfo.addLiveIn(ArgRegs[I], Reg);
//      SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, Reg, XLenVT);
//      FI = MFI.CreateFixedObject(XLenInBytes, VaArgOffset, true);
//      SDValue PtrOff = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
//      SDValue Store = DAG.getStore(Chain, DL, ArgValue, PtrOff,
//                                   MachinePointerInfo::getFixedStack(MF, FI));
//      cast<StoreSDNode>(Store.getNode())
//              ->getMemOperand()
//              ->setValue((Value *)nullptr);
//      OutChains.push_back(Store);
//    }
////    RVFI->setVarArgsSaveSize(VarArgsSaveSize);
//  }

  // All stores are grouped in one node to allow the matching between
  // the size of Ins and InVals. This only happens for vararg functions.
  if (!OutChains.empty()) {
    OutChains.push_back(Chain);
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, OutChains);
  }

  return Chain;
}