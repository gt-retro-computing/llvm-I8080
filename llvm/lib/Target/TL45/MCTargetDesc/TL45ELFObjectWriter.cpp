//
// Created by codetector on 9/3/19.
//
//#include "MCTargetDesc/TL45BaseInfo.h"
//#include "MCTargetDesc/TL45FixupKinds.h"
#include "MCTargetDesc/TL45MCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
    class TL45ELFObjectWriter : public MCELFObjectTargetWriter {
    public:
        TL45ELFObjectWriter(uint8_t OSABI);

        virtual ~TL45ELFObjectWriter();

    protected:
        unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                const MCFixup &Fixup, bool IsPCRel) const override;
    };
}

TL45ELFObjectWriter::TL45ELFObjectWriter(uint8_t OSABI) : MCELFObjectTargetWriter(false, OSABI, ELF::EM_TL45, false){}

TL45ELFObjectWriter::~TL45ELFObjectWriter() {}

unsigned TL45ELFObjectWriter::getRelocType(MCContext &Ctx, const MCValue &Target, const MCFixup &Fixup,
                                             bool IsPCRel) const {
    // TODO: Reloc???
    return (unsigned)Fixup.getKind();
}

std::unique_ptr<MCObjectTargetWriter> llvm::createTL45ELFObjectWriter(uint8_t OSABI) {
    return std::make_unique<TL45ELFObjectWriter>(OSABI);
}
