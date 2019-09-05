#include "LC2200Subtarget.h"
#include "LC2200.h"
#include "llvm/Support/TargetRegistry.h"

#define DEBUG_TYPE "lc2200-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "LC2200GenSubtargetInfo.inc"

using namespace llvm;

LC2200Subtarget::LC2200Subtarget(const Triple &TT, const StringRef CPU, const StringRef FS, const LC2200TargetMachine &TM) :
  LC2200GenSubtargetInfo(TT, CPU, FS),
  TLInfo(TM, *this),
  FrameLowering(*this, 4) {}
