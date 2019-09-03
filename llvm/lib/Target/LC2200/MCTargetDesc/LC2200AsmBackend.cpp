//
// Created by codetector on 9/3/19.
//

#include "MCTargetDesc/LC2200MCTargetDesc.h"
#include "MCTargetDesc/LC2200FixupKinds.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
    class LC2200AsmBackend : public MCAsmBackend {
        Triple::OSType OSType;
    public:
        LC2200AsmBackend(const Target &T, Triple::OSType _OSType) : MCAsmBackend(support::endianness::little),
                                                                    OSType(_OSType) {
        }

        void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup, const MCValue &Target, MutableArrayRef<char> Data,
                        uint64_t Value, bool IsResolved, const MCSubtargetInfo *STI) const override;
    };
}

void LC2200AsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup, const MCValue &Target,
                                  MutableArrayRef<char> Data, uint64_t Value, bool IsResolved,
                                  const MCSubtargetInfo *STI) const {

}

MCAsmBackend* llvm::createLC2200AsmBackend(const llvm::Target &T, const llvm::MCSubtargetInfo &STI,
                                           const llvm::MCRegisterInfo &MRI, const llvm::MCTargetOptions &Options) {
    return new LC2200AsmBackend(STI.getTargetTriple(), STI.getTargetTriple().getOS());
}
