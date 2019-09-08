
#include "LC2200ISelLowering.h"
#include "LC2200TargetMachine.h"

#define DEBUG_TYPE "lc2200-target-lowering"

using namespace llvm;

#include "LC2200GenCallingConv.inc"

LC2200TargetLowering::LC2200TargetLowering(const LC2200TargetMachine &TM,
                                           const LC2200Subtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {

  // Set up the register classes.
  addRegisterClass(MVT::i32, &LC2200::GRRegsRegClass);

  // Compute derived properties from the register classes.
  computeRegisterProperties(STI.getRegisterInfo());

  ISD::CondCode illegalCCs[] = {
      ISD::CondCode::SETULT, ISD::CondCode::SETUGT, /*ISD::CondCode::SETUGE,*/ ISD::CondCode::SETULE,
      ISD::CondCode::SETLT, ISD::CondCode::SETGT, /*ISD::CondCode::SETGE,*/ ISD::CondCode::SETLE,
  };

  for (ISD::CondCode cc : illegalCCs) {
    setCondCodeAction(cc, MVT::i32, Expand);
  }

  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1, Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i32, Expand);

  setOperationAction(ISD::SUB, MVT::i32, Expand);

  setOperationAction(ISD::SHL, MVT::i32, Custom);
  setOperationAction(ISD::SRL, MVT::i32, Custom);

  setOperationAction(ISD::MUL, MVT::i32, Expand);
  setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::SDIV, MVT::i32, Expand);
  setOperationAction(ISD::UDIV, MVT::i32, Expand);
  setOperationAction(ISD::SREM, MVT::i32, Expand);
  setOperationAction(ISD::SDIVREM, MVT::i32, Expand);
  setOperationAction(ISD::UREM, MVT::i32, Expand);


  setOperationAction(ISD::BR_CC, MVT::Other, Custom);
  setOperationAction(ISD::BR_CC, MVT::i32, Custom);

  setOperationAction(ISD::BR, MVT::Other, Custom);

  setOperationAction(ISD::SELECT_CC, MVT::i32, Custom);

  setOperationAction(ISD::AND, MVT::i32, Custom);
  setOperationAction(ISD::OR, MVT::i32, Custom);
  setOperationAction(ISD::XOR, MVT::i32, Custom);

  setOperationAction(ISD::SETCC, MVT::i32, Expand);
  setOperationAction(ISD::SETCC, MVT::Other, Expand);
}

void LC2200TargetLowering::analyzeInputArgs(
    MachineFunction &MF, CCState &CCInfo,
    const SmallVectorImpl<ISD::InputArg> &Ins, bool IsRet) const {
  unsigned NumArgs = Ins.size();
  FunctionType *FType = MF.getFunction().getFunctionType();

  for (unsigned i = 0; i != NumArgs; ++i) {
    MVT ArgVT = Ins[i].VT;
    ISD::ArgFlagsTy ArgFlags = Ins[i].Flags;

    if (IsRet) {
      // caller interpreting returned values
      if (RetCC_LC2200(i, ArgVT, ArgVT, CCValAssign::Full, ArgFlags, CCInfo)) {
        LLVM_DEBUG(dbgs() << "InputArg #" << i << " has unhandled type "
                          << EVT(ArgVT).getEVTString() << '\n');
        llvm_unreachable(nullptr);
      }
    } else {
      // callee interpreting input args
      if (CC_LC2200(i, ArgVT, ArgVT, CCValAssign::Full, ArgFlags, CCInfo)) {
        LLVM_DEBUG(dbgs() << "InputArg #" << i << " has unhandled type "
                          << EVT(ArgVT).getEVTString() << '\n');
        llvm_unreachable(nullptr);
      }
    }
  }
}

void LC2200TargetLowering::analyzeOutputArgs(
    MachineFunction &MF, CCState &CCInfo,
    const SmallVectorImpl<ISD::OutputArg> &Outs, bool IsRet,
    CallLoweringInfo *CLI) const {
  unsigned NumArgs = Outs.size();

  for (unsigned i = 0; i != NumArgs; i++) {
    MVT ArgVT = Outs[i].VT;
    ISD::ArgFlagsTy ArgFlags = Outs[i].Flags;
    // Type *OrigTy = CLI ? CLI->getArgs()[Outs[i].OrigArgIndex].Ty : nullptr;

    if (IsRet) {
      // callee emitting return values
      if (RetCC_LC2200(i, ArgVT, ArgVT, CCValAssign::Full, ArgFlags, CCInfo)) {
        LLVM_DEBUG(dbgs() << "OutputArg #" << i << " has unhandled type "
                          << EVT(ArgVT).getEVTString() << "\n");
        llvm_unreachable(nullptr);
      }
    } else {
      // caller emitting args
      if (CC_LC2200(i, ArgVT, ArgVT, CCValAssign::Full, ArgFlags, CCInfo)) {
        LLVM_DEBUG(dbgs() << "OutputArg #" << i << " has unhandled type "
                          << EVT(ArgVT).getEVTString() << "\n");
        llvm_unreachable(nullptr);
      }
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
    //        Val = DAG.getNode(RISCVISD::FMV_X_ANYEXTW_RV64, DL, MVT::i64,
    //        Val); break;
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
  int FI = MFI.CreateFixedObject(ValVT.getSizeInBits() / 32,
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
  // TODO Vararg

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
  ////    RISCVMachineFunctionInfo *RVFI =
  /// MF.getInfo<RISCVMachineFunctionInfo>();
  //
  //    // Offset of the first variable argument from stack pointer, and size of
  //    // the vararg save area. For now, the varargs save area is either zero
  //    or
  //    // large enough to hold a0-a7.
  //    int VaArgOffset, VarArgsSaveSize;
  //
  //    // If all registers are allocated, then all varargs must be passed on
  //    the
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
  //    // If saving an odd number of registers then create an extra stack slot
  //    to
  //    // ensure that the frame pointer is 2*XLEN-aligned, which in turn
  //    ensures
  //    // offsets to even-numbered registered remain 2*XLEN-aligned.
  //    if (Idx % 2) {
  //      FI = MFI.CreateFixedObject(XLenInBytes, VaArgOffset -
  //      (int)XLenInBytes,
  //                                 true);
  //      VarArgsSaveSize += XLenInBytes;
  //    }
  //
  //    // Copy the integer registers that may have been used for passing
  //    varargs
  //    // to the vararg save area.
  //    for (unsigned I = Idx; I < ArgRegs.size();
  //         ++I, VaArgOffset += XLenInBytes) {
  //      const Register Reg = RegInfo.createVirtualRegister(RC);
  //      RegInfo.addLiveIn(ArgRegs[I], Reg);
  //      SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, Reg, XLenVT);
  //      FI = MFI.CreateFixedObject(XLenInBytes, VaArgOffset, true);
  //      SDValue PtrOff = DAG.getFrameIndex(FI,
  //      getPointerTy(DAG.getDataLayout())); SDValue Store =
  //      DAG.getStore(Chain, DL, ArgValue, PtrOff,
  //                                   MachinePointerInfo::getFixedStack(MF,
  //                                   FI));
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

SDValue
LC2200TargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                  bool IsVarArg,
                                  const SmallVectorImpl<ISD::OutputArg> &Outs,
                                  const SmallVectorImpl<SDValue> &OutVals,
                                  const SDLoc &DL, SelectionDAG &DAG) const {
  // Stores the assignment of the return value to a location.
  SmallVector<CCValAssign, 16> RVLocs;

  // Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  analyzeOutputArgs(DAG.getMachineFunction(), CCInfo, Outs, /*IsRet=*/true,
                    nullptr);

  SDValue Glue;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
    SDValue Val = OutVals[i];
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    if (VA.getLocVT() == MVT::i32 && VA.getValVT() == MVT::f64) {
      // Handle returning f64 on RV32D with a soft float ABI.
      llvm_unreachable("not supported");
      //      assert(VA.isRegLoc() && "Expected return via registers");
      //      SDValue SplitF64 = DAG.getNode(RISCVISD::SplitF64, DL,
      //                                     DAG.getVTList(MVT::i32, MVT::i32),
      //                                     Val);
      //      SDValue Lo = SplitF64.getValue(0);
      //      SDValue Hi = SplitF64.getValue(1);
      //      Register RegLo = VA.getLocReg();
      //      Register RegHi = RegLo + 1;
      //      Chain = DAG.getCopyToReg(Chain, DL, RegLo, Lo, Glue);
      //      Glue = Chain.getValue(1);
      //      RetOps.push_back(DAG.getRegister(RegLo, MVT::i32));
      //      Chain = DAG.getCopyToReg(Chain, DL, RegHi, Hi, Glue);
      //      Glue = Chain.getValue(1);
      //      RetOps.push_back(DAG.getRegister(RegHi, MVT::i32));
    } else {
      // Handle a 'normal' return.
      Val = convertValVTToLocVT(DAG, Val, VA, DL);
      Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Val, Glue);

      // Guarantee that all emitted copies are stuck together.
      Glue = Chain.getValue(1);
      RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
    }
  }

  RetOps[0] = Chain; // Update chain.

  // Add the glue node if we have it.
  if (Glue.getNode()) {
    RetOps.push_back(Glue);
  }

  // Interrupt service routines use different return instructions.
  const Function &Func = DAG.getMachineFunction().getFunction();
  if (Func.hasFnAttribute("interrupt")) {
    llvm_unreachable("interrupts not supported");
    //    if (!Func.getReturnType()->isVoidTy())
    //      report_fatal_error(
    //              "Functions with the interrupt attribute must have void
    //              return type!");
    //
    //    MachineFunction &MF = DAG.getMachineFunction();
    //    StringRef Kind =
    //            MF.getFunction().getFnAttribute("interrupt").getValueAsString();
    //
    //    unsigned RetOpc;
    //    if (Kind == "user")
    //      RetOpc = RISCVISD::URET_FLAG;
    //    else if (Kind == "supervisor")
    //      RetOpc = RISCVISD::SRET_FLAG;
    //    else
    //      RetOpc = RISCVISD::MRET_FLAG;
    //
    //    return DAG.getNode(RetOpc, DL, MVT::Other, RetOps);
  }

  return DAG.getNode(LC2200ISD::RET, DL, MVT::Other, RetOps);
}

SDValue
LC2200TargetLowering::LowerCall(CallLoweringInfo &CLI,
                                SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &DL = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  bool &IsTailCall = CLI.IsTailCall;
  CallingConv::ID CallConv = CLI.CallConv;
  bool IsVarArg = CLI.IsVarArg;
  EVT PtrVT = getPointerTy(DAG.getDataLayout());
  MVT XLenVT = MVT::i32;

  MachineFunction &MF = DAG.getMachineFunction();

  // Analyze the operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState ArgCCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  analyzeOutputArgs(MF, ArgCCInfo, Outs, /*IsRet=*/false, &CLI);

  IsTailCall = false;
  // Check if it's really possible to do a tail call.
  //  if (IsTailCall)
  //    IsTailCall = isEligibleForTailCallOptimization(ArgCCInfo, CLI, MF,
  //    ArgLocs);
  //
  //  if (IsTailCall)
  //    ++NumTailCalls;
  //  else
  if (CLI.CS && CLI.CS.isMustTailCall())
    report_fatal_error("failed to perform tail call elimination on a call "
                       "site marked musttail");

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = ArgCCInfo.getNextStackOffset();

  // Create local copies for byval args
  SmallVector<SDValue, 8> ByValArgs;
  for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
    ISD::ArgFlagsTy Flags = Outs[i].Flags;
    if (!Flags.isByVal())
      continue;

    SDValue Arg = OutVals[i];
    unsigned Size = Flags.getByValSize();
    unsigned Align = Flags.getByValAlign();

    int FI = MF.getFrameInfo().CreateStackObject(Size, Align, /*isSS=*/false);
    SDValue FIPtr = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
    SDValue SizeNode = DAG.getConstant(Size, DL, XLenVT);

    Chain = DAG.getMemcpy(Chain, DL, FIPtr, Arg, SizeNode, Align,
                          /*IsVolatile=*/false,
                          /*AlwaysInline=*/false, IsTailCall,
                          MachinePointerInfo(), MachinePointerInfo());
    ByValArgs.push_back(FIPtr);
  }

  if (!IsTailCall)
    Chain = DAG.getCALLSEQ_START(Chain, NumBytes, 0, CLI.DL);

  // Copy argument values to their designated locations.
  SmallVector<std::pair<Register, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;
  SDValue StackPtr;
  for (unsigned i = 0, j = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue ArgValue = OutVals[i];
    ISD::ArgFlagsTy Flags = Outs[i].Flags;

    // IsF64OnRV32DSoftABI && VA.isMemLoc() is handled below in the same way
    // as any other MemLoc.

    // Promote the value if needed.
    // For now, only handle fully promoted and indirect arguments.
    if (VA.getLocInfo() == CCValAssign::Indirect) {
      // Store the argument in a stack slot and pass its address.
      SDValue SpillSlot = DAG.CreateStackTemporary(Outs[i].ArgVT);
      int FI = cast<FrameIndexSDNode>(SpillSlot)->getIndex();
      MemOpChains.push_back(
          DAG.getStore(Chain, DL, ArgValue, SpillSlot,
                       MachinePointerInfo::getFixedStack(MF, FI)));
      // If the original argument was split (e.g. i128), we need
      // to store all parts of it here (and pass just one address).
      unsigned ArgIndex = Outs[i].OrigArgIndex;
      assert(Outs[i].PartOffset == 0);
      while (i + 1 != e && Outs[i + 1].OrigArgIndex == ArgIndex) {
        SDValue PartValue = OutVals[i + 1];
        unsigned PartOffset = Outs[i + 1].PartOffset;
        SDValue Address = DAG.getNode(ISD::ADD, DL, PtrVT, SpillSlot,
                                      DAG.getIntPtrConstant(PartOffset, DL));
        MemOpChains.push_back(
            DAG.getStore(Chain, DL, PartValue, Address,
                         MachinePointerInfo::getFixedStack(MF, FI)));
        ++i;
      }
      ArgValue = SpillSlot;
    } else {
      ArgValue = convertValVTToLocVT(DAG, ArgValue, VA, DL);
    }

    // Use local copy if it is a byval arg.
    if (Flags.isByVal())
      ArgValue = ByValArgs[j++];

    if (VA.isRegLoc()) {
      // Queue up the argument copies and emit them at the end.
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), ArgValue));
    } else {
      assert(VA.isMemLoc() && "Argument not register or memory");
      assert(!IsTailCall && "Tail call not allowed if stack is used "
                            "for passing parameters");

      // Work out the address of the stack slot.
      if (!StackPtr.getNode())
        StackPtr = DAG.getCopyFromReg(Chain, DL, LC2200::sp, PtrVT);
      SDValue Address =
          DAG.getNode(ISD::ADD, DL, PtrVT, StackPtr,
                      DAG.getIntPtrConstant(VA.getLocMemOffset(), DL));

      // Emit the store.
      MemOpChains.push_back(
          DAG.getStore(Chain, DL, ArgValue, Address, MachinePointerInfo()));
    }
  }

  // Join the stores, which are independent of one another.
  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);

  SDValue Glue;

  // Build a sequence of copy-to-reg nodes, chained and glued together.
  for (auto &Reg : RegsToPass) {
    Chain = DAG.getCopyToReg(Chain, DL, Reg.first, Reg.second, Glue);
    Glue = Chain.getValue(1);
  }

  // If the callee is a GlobalAddress/ExternalSymbol node, turn it into a
  // TargetGlobalAddress/TargetExternalSymbol node so that legalize won't
  // split it and then direct call can be matched by PseudoCALL.
  if (GlobalAddressSDNode *S = dyn_cast<GlobalAddressSDNode>(Callee)) {
    const GlobalValue *GV = S->getGlobal();

    unsigned OpFlags = 0; // RISCVII::MO_CALL;
    //    if (!getTargetMachine().shouldAssumeDSOLocal(*GV->getParent(), GV))
    //      OpFlags = RISCVII::MO_PLT;

    Callee = DAG.getTargetGlobalAddress(GV, DL, PtrVT, 0, OpFlags);
  } else if (ExternalSymbolSDNode *S = dyn_cast<ExternalSymbolSDNode>(Callee)) {
    unsigned OpFlags = 0; // RISCVII::MO_CALL;
                          //
                          //    if
    //    (!getTargetMachine().shouldAssumeDSOLocal(*MF.getFunction().getParent(),
    //                                                 nullptr))
    //      OpFlags = RISCVII::MO_PLT;

    Callee = DAG.getTargetExternalSymbol(S->getSymbol(), PtrVT, OpFlags);
  }

  // The first call operand is the chain and the second is the target address.
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (auto &Reg : RegsToPass)
    Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));

  if (!IsTailCall) {
    // Add a register mask operand representing the call-preserved registers.
    const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();
    const uint32_t *Mask = TRI->getCallPreservedMask(MF, CallConv);
    assert(Mask && "Missing call preserved mask for calling convention");
    Ops.push_back(DAG.getRegisterMask(Mask));
  }

  // Glue the call to the argument copies, if any.
  if (Glue.getNode())
    Ops.push_back(Glue);

  // Emit the call.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);

  //  if (IsTailCall) {
  //    MF.getFrameInfo().setHasTailCall();
  //    return DAG.getNode(RISCVISD::TAIL, DL, NodeTys, Ops);
  //  }

  Chain = DAG.getNode(LC2200ISD::CALL, DL, NodeTys, Ops);
  Glue = Chain.getValue(1);

  // Mark the end of the call, which is glued to the call itself.
  Chain = DAG.getCALLSEQ_END(Chain, DAG.getConstant(NumBytes, DL, PtrVT, true),
                             DAG.getConstant(0, DL, PtrVT, true), Glue, DL);
  Glue = Chain.getValue(1);

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState RetCCInfo(CallConv, IsVarArg, MF, RVLocs, *DAG.getContext());
  analyzeInputArgs(MF, RetCCInfo, Ins, /*IsRet=*/true);

  // Copy all of the result registers out of their specified physreg.
  for (auto &VA : RVLocs) {
    // Copy the value out
    SDValue RetValue =
        DAG.getCopyFromReg(Chain, DL, VA.getLocReg(), VA.getLocVT(), Glue);
    // Glue the RetValue to the end of the call sequence
    Chain = RetValue.getValue(1);
    Glue = RetValue.getValue(2);

    RetValue = convertLocVTToValVT(DAG, RetValue, VA, DL);

    InVals.push_back(RetValue);
  }

  return Chain;
}

SDValue LC2200TargetLowering::LowerOperation(SDValue Op,
                                             SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  default:
    report_fatal_error("unimplemented operand");
  case ISD::SHL:
    return lowerShiftLeft(Op, DAG);
  case ISD::BR_CC:
    return lowerBrCc(Op, DAG);
  case ISD::BR:
    return lowerBr(Op, DAG);
  case ISD::SELECT_CC:
    return lowerSelectCc(Op, DAG);
  case ISD::AND:
    return lowerAnd(Op, DAG);
  case ISD::OR:
    return lowerOr(Op, DAG);
  case ISD::XOR:
    return lowerXor(Op, DAG);

  case ISD::SRL:
    return ExpandLibCall("__srlu", Op, false, DAG);
  }
}

SDValue LC2200TargetLowering::ExpandLibCall(const char *LibcallName, SDValue Op, bool isSigned, SelectionDAG &DAG) const {
  SDNode *Node = Op.getNode();

  TargetLowering::ArgListTy Args;
  TargetLowering::ArgListEntry Entry;
  for (const SDValue &Op : Node->op_values()) {
    EVT ArgVT = Op.getValueType();
    Type *ArgTy = ArgVT.getTypeForEVT(*DAG.getContext());
    Entry.Node = Op;
    Entry.Ty = ArgTy;
    Entry.IsSExt = shouldSignExtendTypeInLibCall(ArgVT, isSigned);
    Entry.IsZExt = !shouldSignExtendTypeInLibCall(ArgVT, isSigned);
    Args.push_back(Entry);
  }
  SDValue Callee = DAG.getExternalSymbol(LibcallName, getPointerTy(DAG.getDataLayout()));

  EVT RetVT = Node->getValueType(0);
  Type *RetTy = RetVT.getTypeForEVT(*DAG.getContext());

  // By default, the input chain to this libcall is the entry node of the
  // function. If the libcall is going to be emitted as a tail call then
  // TLI.isUsedByReturnOnly will change it to the right chain if the return
  // node which is being folded has a non-entry input chain.
  SDValue InChain = DAG.getEntryNode();

  // isTailCall may be true since the callee does not reference caller stack
  // frame. Check if it's in the right position and that the return types match.
  SDValue TCChain = InChain;
  const Function &F = DAG.getMachineFunction().getFunction();
  bool isTailCall =
      isInTailCallPosition(DAG, Node, TCChain) &&
      (RetTy == F.getReturnType() || F.getReturnType()->isVoidTy());
  if (isTailCall)
    InChain = TCChain;

  TargetLowering::CallLoweringInfo CLI(DAG);
  bool signExtend = shouldSignExtendTypeInLibCall(RetVT, isSigned);
  CLI.setDebugLoc(SDLoc(Node))
     .setChain(InChain)
     .setLibCallee(CallingConv::C, RetTy, Callee, std::move(Args))
     .setTailCall(isTailCall)
     .setSExtResult(signExtend)
     .setZExtResult(!signExtend)
     .setIsPostTypeLegalization(true);

  std::pair<SDValue, SDValue> CallInfo = LowerCallTo(CLI);

  if (!CallInfo.second.getNode()) {
    LLVM_DEBUG(dbgs() << "Created tailcall: "; DAG.getRoot().dump(&DAG));
    // It's a tailcall, return the chain (which is the DAG root).
    return DAG.getRoot();
  }

  LLVM_DEBUG(dbgs() << "Created libcall: "; CallInfo.first.dump(&DAG));
  return CallInfo.first;
}

SDValue LC2200TargetLowering::lowerShiftLeft(SDValue Op, SelectionDAG &DAG) const {
  SDLoc DL(Op);
  SDValue Vl = Op.getOperand(0);
  SDValue ShiftAmt = Op.getOperand(1);

  if (ShiftAmt.getOpcode() != ISD::Constant) {
    return ExpandLibCall("__shlu", Op, false, DAG);
  }

  // fold constant shift into repeated ADDs

  EVT VT = Vl.getValueType();
  uint64_t ShAmt = cast<ConstantSDNode>(ShiftAmt.getNode())->getZExtValue();

  for (uint64_t i = 0; i < ShAmt; i++) {
    Vl = DAG.getNode(ISD::ADD, DL, VT, Vl, Vl);
  }

  return Vl;
}

SDValue LC2200TargetLowering::lowerBr(SDValue Op, SelectionDAG &DAG) const {
  SDLoc DagLoc(Op);
  SDValue Chain = Op.getOperand(0);
  SDValue Dest = Op.getOperand(1);
  SDLoc DL(Op);

  SDValue Jmp = DAG.getNode(LC2200ISD::JMP, DL, MVT::Other, Chain, Dest);
  return Jmp;
}

SDValue LC2200TargetLowering::lowerBrCc(SDValue Op, SelectionDAG &DAG) const {
  SDLoc DagLoc(Op);
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS = Op.getOperand(2);
  SDValue RHS = Op.getOperand(3);
  SDValue Dest = Op.getOperand(4);

  SDLoc DL(Op);
  SDValue Cmp = DAG.getNode(LC2200ISD::CMP_SKIP, DL, MVT::Glue,
                            DAG.getConstant(CC, DL, MVT::i32), LHS, RHS);
  SDValue Jmp = DAG.getNode(LC2200ISD::JMP, DL, MVT::Other, Chain, Dest, Cmp);

  return Jmp;
}

SDValue LC2200TargetLowering::lowerSelectCc(SDValue Op,
                                            SelectionDAG &DAG) const {
  SDLoc DagLoc(Op);
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  SDValue TrueValue = Op.getOperand(2);
  SDValue FalseValue = Op.getOperand(3);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();

  SDLoc DL(Op);
  EVT TrueType = TrueValue.getValueType();
  EVT FalseType = FalseValue.getValueType();
  if (TrueType != FalseType) {
    llvm_unreachable("mismatched types of select_cc true and false nodes");
  }

  SDValue Cmp = DAG.getNode(LC2200ISD::CMP_SKIP, DL, MVT::Glue,
                            DAG.getConstant(CC, DL, MVT::i32), LHS, RHS);
  SDValue SelectMove = DAG.getNode(LC2200ISD::SELECT_MOVE, DL, TrueType,
                                   TrueValue, FalseValue, Cmp);
  return SelectMove;
}

static SDValue notValue(SelectionDAG &DAG, const SDLoc &DL, EVT VT, SDValue Value) {
  return DAG.getNode(LC2200ISD::NAND, DL, VT, Value, Value);
}

SDValue LC2200TargetLowering::lowerAnd(SDValue Op, SelectionDAG &DAG) const {
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  EVT VT = LHS.getValueType();
  SDLoc DL(Op);
  // A AND B = (A NAND B) NAND (A NAND B)
  SDValue Nand = DAG.getNode(LC2200ISD::NAND, DL, VT, LHS, RHS);
  SDValue And = notValue(DAG, DL, VT, Nand);
  return And;
}

SDValue LC2200TargetLowering::lowerOr(SDValue Op, SelectionDAG &DAG) const {
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  EVT VT = LHS.getValueType();
  SDLoc DL(Op);
  // A OR B = (NOT A) NAND (NOT B) = (A NAND A) NAND (B NAND B)
  SDValue NotA = notValue(DAG, DL, VT, LHS);
  SDValue NotB = notValue(DAG, DL, VT, RHS);
  SDValue Or = DAG.getNode(LC2200ISD::NAND, DL, VT, NotA, NotB);
  return Or;
}

SDValue LC2200TargetLowering::lowerXor(SDValue Op, SelectionDAG &DAG) const {
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);

  EVT VT = LHS.getValueType();
  SDLoc DL(Op);
  // A XOR B = ((((a NAND b) NAND a) NAND ((a NAND b) NAND b)))
  SDValue NandAB = DAG.getNode(LC2200ISD::NAND, DL, VT, LHS, RHS);

  SDValue NandABNandA = DAG.getNode(LC2200ISD::NAND, DL, VT, NandAB, LHS);
  SDValue NandABNandB = DAG.getNode(LC2200ISD::NAND, DL, VT, NandAB, RHS);

  SDValue Xor = DAG.getNode(LC2200ISD::NAND, DL, VT, NandABNandA, NandABNandB);
  return Xor;
}

const char *LC2200TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch ((LC2200ISD::NodeType)Opcode) {
  case LC2200ISD::FIRST_NUMBER:
    break;
  case LC2200ISD::RET:
    return "LC2200ISD::RET";
  case LC2200ISD::CALL:
    return "LC2200ISD::CALL";
  case LC2200ISD::JMP:
    return "LC2200ISD::JMP";
  case LC2200ISD::CMP_SKIP:
    return "LC2200ISD::CMP_SKIP";
  case LC2200ISD::SELECT_MOVE:
    return "LC2200ISD::SELECT_MOVE";
  }
  return nullptr;
}
