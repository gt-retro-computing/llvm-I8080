//
// Created by codetector on 9/3/19.
//
#include "MCTargetDesc/ECE2031BaseInfo.h"
#include "MCTargetDesc/ECE2031FixupKinds.h"
#include "MCTargetDesc/ECE2031MCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
    class ECE2031ELFObjectWriter : public MCELFObjectTargetWriter {
    public:
        ECE2031ELFObjectWriter(uint8_t OSABI);

        virtual ~ECE2031ELFObjectWriter();

    protected:
        unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                const MCFixup &Fixup, bool IsPCRel) const override;
    };
}

ECE2031ELFObjectWriter::ECE2031ELFObjectWriter(uint8_t OSABI) : MCELFObjectTargetWriter(false, OSABI, ELF::EM_ECE2031, false){}

ECE2031ELFObjectWriter::~ECE2031ELFObjectWriter() {}

unsigned ECE2031ELFObjectWriter::getRelocType(MCContext &Ctx, const MCValue &Target, const MCFixup &Fixup,
                                             bool IsPCRel) const {
    // TODO: Reloc???
    return (unsigned)Fixup.getKind();
}

std::unique_ptr<MCObjectTargetWriter> llvm::createECE2031ELFObjectWriter(uint8_t OSABI) {
    return std::make_unique<ECE2031ELFObjectWriter>(OSABI);
}
