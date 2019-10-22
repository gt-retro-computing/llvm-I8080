//===-- TL45InstPrinter.cpp - Convert TL45 MCInst to assembly syntax ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an TL45 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"

#include "TL45InstPrinter.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

//#define PRINT_ALIAS_INSTR
#include "TL45GenAsmWriter.inc"

void TL45InstPrinter::printRegName(raw_ostream &OS, unsigned RegNo) const {
  OS << StringRef(getRegisterName(RegNo)).lower();
}

void TL45InstPrinter::printInst(const MCInst *MI, raw_ostream &O,
                                StringRef Annot, const MCSubtargetInfo &STI) {
  printInstruction(MI, O);
  printAnnotation(O, Annot);
}

namespace TL45 {
static void printExpr(const MCExpr *Expr, raw_ostream &OS) {

  const MCSymbolRefExpr *SRE;
  const MCBinaryExpr *BinExp;
  switch (Expr->getKind()) {
    case MCExpr::ExprKind::SymbolRef:
      SRE = dyn_cast<MCSymbolRefExpr>(Expr);
      assert(SRE && "Unexpected MCExpr type.");
      OS << SRE->getSymbol();
      break;
    case MCExpr::ExprKind ::Binary:
      BinExp = dyn_cast<MCBinaryExpr>(Expr);
      assert(BinExp && "Failed to cast a binexp to MCBinaryExpr");
      printExpr(BinExp->getLHS(), OS);
      switch (BinExp -> getOpcode()) {
        case MCBinaryExpr::Opcode ::Add: OS << "+"; break;
        case MCBinaryExpr::Opcode ::Sub: OS << "-"; break;
        default: llvm_unreachable("unsupported operation");
      }
      printExpr(BinExp->getRHS(), OS);
    case MCExpr::ExprKind ::Constant:
      OS << dyn_cast<MCConstantExpr>(Expr)->getValue();
      break;
    default:
      Expr->dump();
      llvm_unreachable("Can't print this MCExpr");
  }
//  if SRE->getSymbol().
}
}

void TL45InstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    printRegName(O, Op.getReg());
    return;
  }

  if (Op.isImm()) {
    O << Op.getImm();
    return;
  }

  assert(Op.isExpr() && "unknown operand kind in printOperand");
  TL45::printExpr(Op.getExpr(), O);
}
