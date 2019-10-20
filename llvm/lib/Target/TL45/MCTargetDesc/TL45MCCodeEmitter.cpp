#define DEBUG_TYPE "mccodeemitter"
// #include "MCTargetDesc/TL45BaseInfo.h"
#include "MCTargetDesc/TL45MCTargetDesc.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCCodeEmitter.h"

using namespace llvm;
namespace {
    class TL45MCCodeEmitter : public MCCodeEmitter {
        TL45MCCodeEmitter(const TL45MCCodeEmitter &) = delete;
        void operator=(const TL45MCCodeEmitter &) = delete;
        const MCInstrInfo &InstrInfo;
        unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const;
    public:
        TL45MCCodeEmitter(const MCInstrInfo &MII) : InstrInfo(MII) {}

        ~TL45MCCodeEmitter(){}

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

MCCodeEmitter* llvm::createTL45MCCodeEmitter(const llvm::MCInstrInfo &MCII, const llvm::MCRegisterInfo &MRI,
                                               llvm::MCContext &Ctx) {
    return new TL45MCCodeEmitter(MCII);
}


unsigned TL45MCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                                                SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
    if (MO.isReg()) {
        llvm_unreachable("not yet implemented");
        // return getTL45RegisterNumbering(MO.getReg());
    }
    if (MO.isImm()) {
        return static_cast<unsigned>(MO.getImm());
    }
    //TODO Fixups
    return 0;
}

void TL45MCCodeEmitter::encodeInstruction(const MCInst &Inst, raw_ostream &OS, SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
    uint32_t CurByte = 0;
    unsigned Value = getBinaryCodeForInstr(Inst, Fixups, STI);
    EmitBEConstant(Value, 4, CurByte, OS);
}

#include "TL45GenMCCodeEmitter.inc"