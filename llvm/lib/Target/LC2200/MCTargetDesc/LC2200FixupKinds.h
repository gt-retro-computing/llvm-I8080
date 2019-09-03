//
// Created by codetector on 9/3/19.
//

#ifndef LLVM_LC2200FIXUPKINDS_H
#define LLVM_LC2200FIXUPKINDS_H

enum Fixups {
    FirstTargetFixupKind,
    LastTargetFixupKind,
    NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};

#endif //LLVM_LC2200FIXUPKINDS_H
