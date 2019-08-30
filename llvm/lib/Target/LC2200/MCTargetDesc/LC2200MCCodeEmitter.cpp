unsigned LC2200MCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO,
	SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
	if (MO.isReg()) {
		return CTX.getRegisterInfo()->getEncodingValue(MO.getReg());
}

void LC2200MCCodeEmitter::EncodeInstruction(const MCInst &MI, raw_ostream &OS, 
	SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
	const MCInstrDesc &Desc = MCII.get(MI.getOpcode());
	if (Desc.getSize() != 4) {
		llvm_unreachable("Unexpected instruction size!");
	}
	const uint32_t Binary = getBinaryCodeForInstr(MI, Fixups, STI);
	EmitConstant(Binary, Desc.getSize(), OS);
	++MCNumEmitted;
} 