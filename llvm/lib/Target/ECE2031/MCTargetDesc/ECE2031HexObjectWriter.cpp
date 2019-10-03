//
// Created by codetector on 9/4/19.
//

#include "ECE2031MCTargetDesc.h"
#include "llvm/MC/MCObjectWriter.h"

using namespace llvm;

namespace {
class ECE2031HexObjectWriter : MCObjectTargetWriter {
    Triple::ObjectFormatType getFormat() const override {
        return Triple::Hex;
    }
};

}
