#include "LC2200Subtarget.h"
#include "LC2200.h"
#include "llvm/Support/TargetRegistry.h"

#define DEBUG_TYPE "lc3-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "LC2200GenSubtargetInfo.inc"

using namespace llvm;

LC3Subtarget::LC3Subtarget(const std::string &TT, const std::string &CPU, const std::string &FS, TOYTargetMachine &TM)
	DL("e-m:e-p:32:32-i1:8:32-i8:8:32-i16:16:32-i64:32-f64:32-a:0:32-n32"),
	InstrInfo(), TLInfo(TM), TSInfo(DL), FrameLowering()
	{} 