#include "TL45MCAsmInfo.h"
#include "llvm/ADT/StringRef.h"

using namespace llvm;

void TL45MCAsmInfo::anchor() {}

TL45MCAsmInfo::TL45MCAsmInfo(const Triple &TT) {
    //TODO: Assembly formatting
//	SupportsDebugInformation = false;
//	Data16bitsDirective = 0;
//	Data32bitsDirective = "\t.fill\t";
//	Data64bitsDirective = 0;
//	ZeroDirective = "\t.fill 0\t";
  CommentString = ";";
//	AscizDirective = 0;
//	HiddenVisibilityAttr = MCSA_Invalid;
//	HiddenDeclarationVisibilityAttr = MCSA_Invalid;
//	ProtectedVisibilityAttr = MCSA_Invalid;

  // compatibility with non-LLVM LC2200 assemblers
  PrivateGlobalPrefix = "_l";
  PrivateLabelPrefix = "_l";

//  GlobalDirective = "\t! globl\t";

  UsesNonexecutableStackSection = false;
  HasDotTypeDotSizeDirective = false;
  HasSingleParameterDotFile = false;
  NeedsLocalForSize = false;
  HasIdentDirective = false;


}
