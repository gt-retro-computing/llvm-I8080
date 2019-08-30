
#ifndef LC2200TARGETASMINFO_H
#define LC2200TARGETASMINFO_H
#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
	class StringRef;
	class Target;
	class LC2200MCAsmInfo : public MCAsmInfoELF {
	virtual void anchor();
	public:
	 explicit LC2200MCAsmInfo(StringRef TT);
	};
} // namespace llvm

#endif 