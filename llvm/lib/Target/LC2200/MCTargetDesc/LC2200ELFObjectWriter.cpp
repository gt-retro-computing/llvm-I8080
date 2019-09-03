//
// Created by codetector on 9/3/19.
//
#include "MCTargetDesc/LC2200BaseInfo.h"
#include "MCTargetDesc/LC2200FixupKinds.h"
#include "MCTargetDesc/LC2200MCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
    class LC2200ELFObjectWriter : public MCELFObjectTargetWriter {
    public:
        LC2200ELFObjectWriter(uint8_t OSABI);

        virtual ~LC2200ELFObjectWriter();

    protected:
        unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                const MCFixup &Fixup, bool IsPCRel) const override;
    };
}

LC2200ELFObjectWriter::LC2200ELFObjectWriter(uint8_t OSABI) : MCELFObjectTargetWriter(false, OSABI, ELF::EM_LC2200, true){}

LC2200ELFObjectWriter::~LC2200ELFObjectWriter() {}

unsigned LC2200ELFObjectWriter::getRelocType(MCContext &Ctx, const MCValue &Target, const MCFixup &Fixup,
                                             bool IsPCRel) const {
    // TODO: Reloc???
    return (unsigned)Fixup.getKind();
}

std::unique_ptr<MCObjectWriter> llvm::createLC2200ELFObjectWriter(raw_pwrite_stream &OS, uint8_t OSABI) {
    auto MOTW = std::make_unique<LC2200ELFObjectWriter>(OSABI);
    return createELFObjectWriter(std::move(MOTW), OS, true);
}
