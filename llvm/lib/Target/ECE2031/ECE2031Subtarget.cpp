#include "ECE2031Subtarget.h"
#include "ECE2031.h"
#include "llvm/Support/TargetRegistry.h"

#define DEBUG_TYPE "lc2200-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "ECE2031GenSubtargetInfo.inc"

using namespace llvm;

ECE2031Subtarget::ECE2031Subtarget(const Triple &TT, const StringRef CPU, const StringRef FS, const ECE2031TargetMachine &TM) :
  ECE2031GenSubtargetInfo(TT, CPU, FS),
  TLInfo(TM, *this),
  FrameLowering(*this, /* Alignment in units of memory *not* bytes */ 1) {}
