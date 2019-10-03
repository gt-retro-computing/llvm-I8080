//
// Created by codetector on 9/3/19.
//

#include "MCTargetDesc/ECE2031MCTargetDesc.h"
#include "MCTargetDesc/ECE2031FixupKinds.h"
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
class ECE2031AsmBackend : public MCAsmBackend {
  Triple::OSType OSType;
public:
  ECE2031AsmBackend(const Target &T, Triple::OSType _OSType) : MCAsmBackend(support::endianness::little),
                                                              OSType(_OSType) {
  }

  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup, const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved, const MCSubtargetInfo *STI) const override;

  std::unique_ptr<MCObjectTargetWriter> createObjectTargetWriter() const override;

  unsigned int getNumFixupKinds() const override;

  bool mayNeedRelaxation(const MCInst &Inst, const MCSubtargetInfo &STI) const override;

  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value, const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override;

  void relaxInstruction(const MCInst &Inst, const MCSubtargetInfo &STI, MCInst &Res) const override;

  bool writeNopData(raw_ostream &OS, uint64_t Count) const override;
};
}

void ECE2031AsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup, const MCValue &Target,
                                  MutableArrayRef<char> Data, uint64_t Value, bool IsResolved,
                                  const MCSubtargetInfo *STI) const {
  exit(-1);
}

//std::unique_ptr<MCObjectTargetWriter> ECE2031AsmBackend::createObjectTargetWriter() const {
//  if (OSType == Triple::OSType::UnknownOS) {
//    return createECE2031ELFObjectWriter(0);
//  }
//  llvm_unreachable("Can not process the specified OS");
//}

unsigned int ECE2031AsmBackend::getNumFixupKinds() const {
  return ECE2031::Fixups::NumTargetFixupKinds;
}

bool ECE2031AsmBackend::mayNeedRelaxation(const MCInst &Inst, const MCSubtargetInfo &STI) const {
  return false;
}

bool ECE2031AsmBackend::fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value, const MCRelaxableFragment *DF,
                                            const MCAsmLayout &Layout) const {
  return false;
}

void ECE2031AsmBackend::relaxInstruction(const MCInst &Inst, const MCSubtargetInfo &STI, MCInst &Res) const {
}

bool ECE2031AsmBackend::writeNopData(raw_ostream &OS, uint64_t Count) const {
  if (Count == 0) {
    return true;
  }
  return false;
}

MCAsmBackend *llvm::createECE2031AsmBackend(const llvm::Target &T, const llvm::MCSubtargetInfo &STI,
                                           const llvm::MCRegisterInfo &MRI, const llvm::MCTargetOptions &Options) {
  return new ECE2031AsmBackend(T, STI.getTargetTriple().getOS());
}
