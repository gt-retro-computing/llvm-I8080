
#ifndef TL45TARGETASMINFO_H
#define TL45TARGETASMINFO_H

#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
	class StringRef;
	class Target;
	class TL45MCAsmInfo : public MCAsmInfoELF {
	virtual void anchor();
	public:
	  explicit TL45MCAsmInfo(const Triple &TT);

  };
} // namespace llvm

#endif 