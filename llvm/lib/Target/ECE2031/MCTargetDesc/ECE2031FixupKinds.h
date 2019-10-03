//
// Created by codetector on 9/3/19.
//

#ifndef LLVM_ECE2031FIXUPKINDS_H
#define LLVM_ECE2031FIXUPKINDS_H

using namespace llvm;

namespace llvm {
namespace ECE2031 {
enum Fixups {
  FirstTargetFixupKind,
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};
}
}

#endif //LLVM_ECE2031FIXUPKINDS_H
