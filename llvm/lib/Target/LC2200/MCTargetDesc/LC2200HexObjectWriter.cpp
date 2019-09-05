//
// Created by codetector on 9/4/19.
//

#include "LC2200MCTargetDesc.h"
#include "llvm/MC/MCObjectWriter.h"

using namespace llvm;

namespace {
class LC2200HexObjectWriter : MCObjectTargetWriter {
    Triple::ObjectFormatType getFormat() const override {
        return Triple::Hex;
    }
};

}
