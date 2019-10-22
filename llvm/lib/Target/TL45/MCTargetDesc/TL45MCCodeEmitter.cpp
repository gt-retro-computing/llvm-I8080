#define DEBUG_TYPE "mccodeemitter"
#include "MCTargetDesc/TL45BaseInfo.h"
#include "MCTargetDesc/TL45MCTargetDesc.h"
#include "MCTargetDesc/TL45FixupKinds.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCCodeEmitter.h"

STATISTIC(MCNumEmitted, "Number of MC instructions emitted");
STATISTIC(MCNumFixups, "Number of MC fixups created");

using namespace llvm;
namespace {
    class TL45MCCodeEmitter : public MCCodeEmitter {
        TL45MCCodeEmitter(const TL45MCCodeEmitter &) = delete;
        void operator=(const TL45MCCodeEmitter &) = delete;
        const MCInstrInfo &InstrInfo;
        unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const;

      unsigned getImmOpValue(const MCInst &MI, unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

      unsigned getJumpTargetOpValue(const MCInst &MI, unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;
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

unsigned TL45MCCodeEmitter::getImmOpValue(const MCInst &MI, unsigned OpNo,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  // bool EnableRelax = STI.getFeatureBits()[TL45::FeatureRelax];
  const MCOperand &MO = MI.getOperand(OpNo);

  MCInstrDesc const &Desc = InstrInfo.get(MI.getOpcode());
  // unsigned MIFrm = Desc.TSFlags & TL45II::InstFormatMask;

  // If the destination is an immediate, there is nothing to do.
  if (MO.isImm())
    return MO.getImm();

  assert(MO.isExpr() &&
         "getImmOpValue expects only expressions or immediates");
  const MCExpr *Expr = MO.getExpr();
  MCExpr::ExprKind Kind = Expr->getKind();
  TL45::Fixups FixupKind = TL45::fixup_tl45_invalid;
  bool RelaxCandidate = false;
  if (Kind == MCExpr::Target) {
    Expr->dump();
    llvm_unreachable("todo, support MCExpr::Target");
//    const TL45MCExpr *RVExpr = cast<TL45MCExpr>(Expr);
//
//    switch (RVExpr->getKind()) {
//    case TL45MCExpr::VK_TL45_None:
//    case TL45MCExpr::VK_TL45_Invalid:
//    case TL45MCExpr::VK_TL45_32_PCREL:
//      llvm_unreachable("Unhandled fixup kind!");
//    case TL45MCExpr::VK_TL45_TPREL_ADD:
//      // tprel_add is only used to indicate that a relocation should be emitted
//      // for an add instruction used in TP-relative addressing. It should not be
//      // expanded as if representing an actual instruction operand and so to
//      // encounter it here is an error.
//      llvm_unreachable(
//          "VK_TL45_TPREL_ADD should not represent an instruction operand");
//    case TL45MCExpr::VK_TL45_LO:
//      if (MIFrm == TL45II::InstFormatI)
//        FixupKind = TL45::fixup_tl45_lo12_i;
//      else if (MIFrm == TL45II::InstFormatS)
//        FixupKind = TL45::fixup_tl45_lo12_s;
//      else
//        llvm_unreachable("VK_TL45_LO used with unexpected instruction format");
//      RelaxCandidate = true;
//      break;
//    case TL45MCExpr::VK_TL45_HI:
//      FixupKind = TL45::fixup_tl45_hi20;
//      RelaxCandidate = true;
//      break;
//    case TL45MCExpr::VK_TL45_PCREL_LO:
//      if (MIFrm == TL45II::InstFormatI)
//        FixupKind = TL45::fixup_tl45_pcrel_lo12_i;
//      else if (MIFrm == TL45II::InstFormatS)
//        FixupKind = TL45::fixup_tl45_pcrel_lo12_s;
//      else
//        llvm_unreachable(
//            "VK_TL45_PCREL_LO used with unexpected instruction format");
//      RelaxCandidate = true;
//      break;
//    case TL45MCExpr::VK_TL45_PCREL_HI:
//      FixupKind = TL45::fixup_tl45_pcrel_hi20;
//      RelaxCandidate = true;
//      break;
//
//    }
  } else if (Kind == MCExpr::SymbolRef &&
             cast<MCSymbolRefExpr>(Expr)->getKind() == MCSymbolRefExpr::VK_None) {

    FixupKind = TL45::fixup_tl45_lo16_i;
    //    if (Desc.getOpcode() == TL45::JAL) {
//      FixupKind = TL45::fixup_tl45_jal;
//    } else if (MIFrm == TL45II::InstFormatB) {
//      FixupKind = TL45::fixup_tl45_branch;
//    } else if (MIFrm == TL45II::InstFormatCJ) {
//      FixupKind = TL45::fixup_tl45_rvc_jump;
//    } else if (MIFrm == TL45II::InstFormatCB) {
//      FixupKind = TL45::fixup_tl45_rvc_branch;
//    }
  }

  assert(FixupKind != TL45::fixup_tl45_invalid && "Unhandled expression!");

  Fixups.push_back(
      MCFixup::create(0, Expr, MCFixupKind(FixupKind), MI.getLoc()));
  ++MCNumFixups;

  // Ensure an R_TL45_RELAX relocation will be emitted if linker relaxation is
  // enabled and the current fixup will result in a relocation that may be
  // relaxed.
//  if (EnableRelax && RelaxCandidate) {
//    const MCConstantExpr *Dummy = MCConstantExpr::create(0, Ctx);
//    Fixups.push_back(
//        MCFixup::create(0, Dummy, MCFixupKind(TL45::fixup_tl45_relax),
//                        MI.getLoc()));
//    ++MCNumFixups;
//  }

  return 0;
}

unsigned TL45MCCodeEmitter::getJumpTargetOpValue(const MCInst &MI, unsigned OpNo,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  // bool EnableRelax = STI.getFeatureBits()[TL45::FeatureRelax];
  const MCOperand &MO = MI.getOperand(OpNo);

  MCInstrDesc const &Desc = InstrInfo.get(MI.getOpcode());
  // unsigned MIFrm = Desc.TSFlags & TL45II::InstFormatMask;

  // If the destination is an immediate, there is nothing to do.
  if (MO.isImm())
    return MO.getImm();

  assert(MO.isExpr() &&
         "getImmOpValue expects only expressions or immediates");
  const MCExpr *Expr = MO.getExpr();
  MCExpr::ExprKind Kind = Expr->getKind();
  TL45::Fixups FixupKind = TL45::fixup_tl45_invalid;
  bool RelaxCandidate = false;
  if (Kind == MCExpr::Target) {
    Expr->dump();
    llvm_unreachable("todo, support MCExpr::Target");
//    const TL45MCExpr *RVExpr = cast<TL45MCExpr>(Expr);
//
//    switch (RVExpr->getKind()) {
//    case TL45MCExpr::VK_TL45_None:
//    case TL45MCExpr::VK_TL45_Invalid:
//    case TL45MCExpr::VK_TL45_32_PCREL:
//      llvm_unreachable("Unhandled fixup kind!");
//    case TL45MCExpr::VK_TL45_TPREL_ADD:
//      // tprel_add is only used to indicate that a relocation should be emitted
//      // for an add instruction used in TP-relative addressing. It should not be
//      // expanded as if representing an actual instruction operand and so to
//      // encounter it here is an error.
//      llvm_unreachable(
//          "VK_TL45_TPREL_ADD should not represent an instruction operand");
//    case TL45MCExpr::VK_TL45_LO:
//      if (MIFrm == TL45II::InstFormatI)
//        FixupKind = TL45::fixup_tl45_lo12_i;
//      else if (MIFrm == TL45II::InstFormatS)
//        FixupKind = TL45::fixup_tl45_lo12_s;
//      else
//        llvm_unreachable("VK_TL45_LO used with unexpected instruction format");
//      RelaxCandidate = true;
//      break;
//    case TL45MCExpr::VK_TL45_HI:
//      FixupKind = TL45::fixup_tl45_hi20;
//      RelaxCandidate = true;
//      break;
//    case TL45MCExpr::VK_TL45_PCREL_LO:
//      if (MIFrm == TL45II::InstFormatI)
//        FixupKind = TL45::fixup_tl45_pcrel_lo12_i;
//      else if (MIFrm == TL45II::InstFormatS)
//        FixupKind = TL45::fixup_tl45_pcrel_lo12_s;
//      else
//        llvm_unreachable(
//            "VK_TL45_PCREL_LO used with unexpected instruction format");
//      RelaxCandidate = true;
//      break;
//    case TL45MCExpr::VK_TL45_PCREL_HI:
//      FixupKind = TL45::fixup_tl45_pcrel_hi20;
//      RelaxCandidate = true;
//      break;
//
//    }
  } else if (Kind == MCExpr::SymbolRef &&
             cast<MCSymbolRefExpr>(Expr)->getKind() == MCSymbolRefExpr::VK_None) {

    FixupKind = TL45::fixup_tl45_lo16_i;
    //    if (Desc.getOpcode() == TL45::JAL) {
//      FixupKind = TL45::fixup_tl45_jal;
//    } else if (MIFrm == TL45II::InstFormatB) {
//      FixupKind = TL45::fixup_tl45_branch;
//    } else if (MIFrm == TL45II::InstFormatCJ) {
//      FixupKind = TL45::fixup_tl45_rvc_jump;
//    } else if (MIFrm == TL45II::InstFormatCB) {
//      FixupKind = TL45::fixup_tl45_rvc_branch;
//    }
  }

  assert(FixupKind != TL45::fixup_tl45_invalid && "Unhandled expression!");

  Fixups.push_back(
      MCFixup::create(0, Expr, MCFixupKind(FixupKind), MI.getLoc()));
  ++MCNumFixups;

  // Ensure an R_TL45_RELAX relocation will be emitted if linker relaxation is
  // enabled and the current fixup will result in a relocation that may be
  // relaxed.
//  if (EnableRelax && RelaxCandidate) {
//    const MCConstantExpr *Dummy = MCConstantExpr::create(0, Ctx);
//    Fixups.push_back(
//        MCFixup::create(0, Dummy, MCFixupKind(TL45::fixup_tl45_relax),
//                        MI.getLoc()));
//    ++MCNumFixups;
//  }

  return 0;
}


unsigned TL45MCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                                                SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
    if (MO.isReg()) {
        return getTL45RegisterNumbering(MO.getReg());
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