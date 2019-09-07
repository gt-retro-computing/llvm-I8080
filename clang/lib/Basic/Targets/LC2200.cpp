//
// Created by codetector on 9/7/19.
//

#include "LC2200.h"

using namespace clang;
using namespace clang::targets;

const char *const LC2200TargetInfo::GCCRegNames[] = {
        "$zero", "$at", "$v0", "$a0", "$a1", "$a2",
        "$t0", "$t1", "$t2",
        "$s0", "$s1", "$s2",
        "$k0", "$sp", "$fp", "$ra"
};

ArrayRef<const char *> LC2200TargetInfo::getGCCRegNames() const {
  return llvm::makeArrayRef(GCCRegNames);
}

const TargetInfo::GCCRegAlias LC2200TargetInfo::GCCRegAliases[] = {
        {{}, "$zero"},
        {{}, "$at"},
        {{}, "$v0"},
        {{}, "$a0"},
        {{}, "$a1"},
        {{}, "$a2"},
        {{}, "$t0"},
        {{}, "$t1"},
        {{}, "$t2"},
        {{}, "$s0"},
        {{}, "$s1"},
        {{}, "$s2"},
        {{}, "$k0"},
        {{}, "$sp"},
        {{}, "$fp"},
        {{}, "$ra"},
};

ArrayRef<TargetInfo::GCCRegAlias> LC2200TargetInfo::getGCCRegAliases() const {
  return llvm::makeArrayRef(GCCRegAliases);
}


void LC2200TargetInfo::getTargetDefines(const clang::LangOptions &Opts, clang::MacroBuilder &Builder) const {
}


