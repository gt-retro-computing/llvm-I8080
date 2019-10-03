
#ifndef ECE2031TARGETASMINFO_H
#define ECE2031TARGETASMINFO_H

#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
	class StringRef;
	class Target;
	class ECE2031MCAsmInfo : public MCAsmInfoELF {
	virtual void anchor();
	public:
	  explicit ECE2031MCAsmInfo(const Triple &TT);

  };
} // namespace llvm

#endif 