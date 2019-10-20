#include "TL45Subtarget.h"
#include "TL45.h"
#include "llvm/Support/TargetRegistry.h"

#define DEBUG_TYPE "tl45-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "TL45GenSubtargetInfo.inc"

using namespace llvm;

TL45Subtarget::TL45Subtarget(const Triple &TT, const StringRef CPU, const StringRef FS, const TL45TargetMachine &TM) :
  TL45GenSubtargetInfo(TT, CPU, FS),
  TLInfo(TM, *this),
  FrameLowering(*this, /* Alignment in units of memory *not* bytes */ 1) {}
