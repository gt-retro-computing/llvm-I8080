//
// Created by codetector on 9/3/19.
//

#ifndef LLVM_LC2200FIXUPKINDS_H
#define LLVM_LC2200FIXUPKINDS_H

using namespace llvm;

namespace llvm {
namespace LC2200 {
enum Fixups {
  FirstTargetFixupKind,
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};
}
}

#endif //LLVM_LC2200FIXUPKINDS_H
