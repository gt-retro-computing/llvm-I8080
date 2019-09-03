#define DEBUG_TYPE "mccodeemitter"
#include "MCTargetDesc/LC2200BaseInfo.h"
#include "MCTargetDesc/LC2200MCTargetDesc.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCCodeEmitter.h"

using namespace llvm;
namespace {
    class LC2200MCCodeEmitter : public MCCodeEmitter {
        LC2200MCCodeEmitter(const LC2200MCCodeEmitter &) = delete;
        void operator=(const LC2200MCCodeEmitter &) = delete;
        const MCInstrInfo &InstrInfo;
        unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const;
    public:
        LC2200MCCodeEmitter(const MCInstrInfo &MII) : InstrInfo(MII) {}

        ~LC2200MCCodeEmitter(){}

        uint64_t getBinaryCodeForInstr(const MCInst &MI,
        SmallVectorImpl<MCFixup> &Fixups,
        const MCSubtargetInfo &STI) const;

        void EmitByte(unsigned char C, unsigned &CurByte, raw_ostream &OS) const {
            OS << (char)C;
            ++CurByte;
        }

        void EmitBEConstant(uint64_t Val, unsigned Size, unsigned &CurByte,
                            raw_ostream &OS) const {
            assert(Size <= 8 && "size too big in emit constant");

            for (int i = (Size-1)*8; i >= 0; i-=8)
                EmitByte((Val >> i) & 255, CurByte, OS);
        }

        void encodeInstruction(const MCInst &Inst, raw_ostream &OS, SmallVectorImpl<MCFixup> &Fixups,
                               const MCSubtargetInfo &STI) const override;
    };
} // end anonymous namespace

MCCodeEmitter* llvm::createLC2200MCCodeEmitter(const llvm::MCInstrInfo &MCII, const llvm::MCRegisterInfo &MRI,
                                               llvm::MCContext &Ctx) {
    return new LC2200MCCodeEmitter(MCII);
}


unsigned LC2200MCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                                                SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
    if (MO.isReg()) {
        return getLC2200RegisterNumbering(MO.getReg());
    }
    if (MO.isImm()) {
        return static_cast<unsigned>(MO.getImm());
    }
    //TODO Fixups
    return 0;
}

void LC2200MCCodeEmitter::encodeInstruction(const MCInst &Inst, raw_ostream &OS, SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
    uint32_t CurByte = 0;
    unsigned Value = getBinaryCodeForInstr(Inst, Fixups, STI);
    EmitBEConstant(Value, 4, CurByte, OS);
}

#include "LC2200GenMCCodeEmitter.inc"