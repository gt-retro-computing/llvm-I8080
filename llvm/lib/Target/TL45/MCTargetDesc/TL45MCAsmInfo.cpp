 #include "TL45MCAsmInfo.h"
#include "llvm/ADT/StringRef.h"

using namespace llvm;

void TL45MCAsmInfo::anchor() {}

TL45MCAsmInfo::TL45MCAsmInfo(const Triple &TT) {
    //TODO: Assembly formatting
	Data32bitsDirective = "\tDW\t";
  CommentString = ";";
//	AscizDirective = 0;
//	HiddenVisibilityAttr = MCSA_Invalid;
//	HiddenDeclarationVisibilityAttr = MCSA_Invalid;
//	ProtectedVisibilityAttr = MCSA_Invalid;

  // compatibility with non-LLVM LC2200 assemblers
  PrivateGlobalPrefix = "_lpg";
  PrivateLabelPrefix = "_lp";
  LinkerPrivateGlobalPrefix = "_llp";
  IsLittleEndian = false;

  UsesNonexecutableStackSection = false;
  HasDotTypeDotSizeDirective = false;
  HasSingleParameterDotFile = false;
  NeedsLocalForSize = false;
  HasIdentDirective = false;
}
