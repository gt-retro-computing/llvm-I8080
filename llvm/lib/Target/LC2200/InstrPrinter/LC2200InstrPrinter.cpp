//
// Created by codetector on 8/29/19.
//
void LC2200InstPrinter::printOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    const MCOperand &Op = MI->getOperand(OpNo);
    if (Op.isReg()) {
        printRegName(O, Op.getReg());
        return;
    }
    if (Op.isImm()) {
        O << "#" << Op.getImm();
        return;
    }
    assert(Op.isExpr() && "unknown operand kind in printOperand");
    printExpr(Op.getExpr(), O);
}

void LC2200InstPrinter::printRegName(raw_ostream &OS, unsigned RegNo) const {
    OS << StringRef(getRegisterName(RegNo)).lower();
}

void LC2200InstPrinter::printInst(const MCInst *MI, raw_ostream &O, StringRef Annot) {
    printInstruction(MI, O);
    printAnnotation(O, Annot);
}
