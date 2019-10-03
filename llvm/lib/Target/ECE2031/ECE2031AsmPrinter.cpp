//
// Created by codetector on 9/2/19.
//

#include "ECE2031.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/MC/MCStreamer.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"
namespace {
class ECE2031AsmPrinter : public AsmPrinter {

public:
  ECE2031AsmPrinter(TargetMachine &TM,
                   std::unique_ptr<MCStreamer> &&Streamer) : AsmPrinter(TM, std::move(Streamer)) {}

  void EmitInstruction(const MachineInstr *) override;

  bool emitPseudoExpansionLowering(MCStreamer &OutStreamer,
                                   const MachineInstr *MI);

};
}

static MCOperand lowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym,
                                    const AsmPrinter &AP) {
  MCContext &Ctx = AP.OutContext;

  const MCExpr *ME =
          MCSymbolRefExpr::create(Sym, MCSymbolRefExpr::VK_None, Ctx);

  if (!MO.isJTI() && !MO.isMBB() && MO.getOffset())
    ME = MCBinaryExpr::createAdd(
            ME, MCConstantExpr::create(MO.getOffset(), Ctx), Ctx);

  return MCOperand::createExpr(ME);
}

static bool LowerECE2031MachineOperandToMCOperand(const MachineOperand &MO,
                                               MCOperand &MCOp,
                                               const AsmPrinter &AP) {
  switch (MO.getType()) {
    default:
      report_fatal_error("LowerECE2031MachineInstrToMCInst: unknown operand type");
    case MachineOperand::MO_Register:
      // Ignore all implicit register operands.
      if (MO.isImplicit())
        return false;
      MCOp = MCOperand::createReg(MO.getReg());
      break;
    case MachineOperand::MO_RegisterMask:
      // Regmasks are like implicit defs.
      return false;
    case MachineOperand::MO_Immediate:
      MCOp = MCOperand::createImm(MO.getImm());
      break;
    case MachineOperand::MO_MachineBasicBlock:
      MCOp = lowerSymbolOperand(MO, MO.getMBB()->getSymbol(), AP);
      break;
    case MachineOperand::MO_GlobalAddress:
      MCOp = lowerSymbolOperand(MO, AP.getSymbol(MO.getGlobal()), AP);
      break;
    case MachineOperand::MO_BlockAddress:
      MCOp = lowerSymbolOperand(
              MO, AP.GetBlockAddressSymbol(MO.getBlockAddress()), AP);
      break;
    case MachineOperand::MO_ExternalSymbol:
      MCOp = lowerSymbolOperand(
              MO, AP.GetExternalSymbolSymbol(MO.getSymbolName()), AP);
      break;
    case MachineOperand::MO_ConstantPoolIndex:
      MCOp = lowerSymbolOperand(MO, AP.GetCPISymbol(MO.getIndex()), AP);
      break;
  }
  return true;
}

// Simple pseudo-instructions have their lowering (with expansion to real
// instructions) auto-generated.
#include "ECE2031GenMCPseudoLowering.inc"

void ECE2031AsmPrinter::EmitInstruction(const MachineInstr *MI) {
  if (emitPseudoExpansionLowering(*OutStreamer, MI))
    return;

  MCInst TmpInst;

  TmpInst.setOpcode(MI->getOpcode());

  for (const MachineOperand &MO : MI->operands()) {
    MCOperand MCOp;
    if (LowerECE2031MachineOperandToMCOperand(MO, MCOp, *this))
      TmpInst.addOperand(MCOp);
  }

  EmitToStreamer(*OutStreamer, TmpInst);
}

extern "C" void LLVMInitializeECE2031AsmPrinter() {
  RegisterAsmPrinter<ECE2031AsmPrinter> X(TheECE2031Target);
}