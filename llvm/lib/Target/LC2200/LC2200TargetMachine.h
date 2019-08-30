const LC3InstrInfo *getInstrInfo() const override {
	return getSubtargetImpl()->getInstrInfo();
} 