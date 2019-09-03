//
// Created by codetector on 9/2/19.
//

#include "llvm/ADT/STLExtras.h"
#include "MCTargetDesc/LC2200MCTargetDesc.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/TargetRegistry.h"

#define DEBUG_TYPE "asm-matcher"

using namespace llvm;

namespace {
struct LC2200Operand;

class LC2200AsmParser : public MCTargetAsmParser {
  const MCSubtargetInfo &STI;
  MCAsmParser &Parser;

  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode, OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo, bool MatchingInlineAsm) override;

  bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override;

  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;

  bool ParseDirective(AsmToken DirectiveID) override;

  bool ParseRegister(unsigned &RegNo, OperandVector &Operands);

  bool ParseImmediate(OperandVector &Operands);

  bool ParseOperand(OperandVector &Operands);

  bool ParseSymbolReference(OperandVector &Operands);

#define GET_ASSEMBLER_HEADER

#include "LC2200GenAsmMatcher.inc"

public:
  LC2200AsmParser(const MCSubtargetInfo &_STI, MCAsmParser &_Parser,
                  const MCInstrInfo &MII, const MCTargetOptions &Options)
          : MCTargetAsmParser(Options, _STI, MII), STI(_STI), Parser(_Parser) {
//            setAvailableFeatures(ComputeAvailableFeatures(STI.getFeatureBits()));
    MCAsmParserExtension::Initialize(Parser);
  }
};


struct LC2200Operand : MCParsedAsmOperand {
  enum KindTy {
    k_Token,
    k_Register,
    k_Immediate,
    k_Memory
  } Kind;
  SMLoc StartLoc, EndLoc;

  struct Token {
    const char *Data;
    unsigned Length;
  };

  struct RegOp {
    unsigned Num;
  };

  struct ImmOp {
    const MCExpr *Val;
  };

  struct MemOp {
    unsigned BaseNum;
    const MCExpr *Off;
  };

  union {
    struct Token Tok;
    struct RegOp Reg;
    struct ImmOp Imm;
    struct MemOp Mem;
  };

  LC2200Operand(KindTy K) : MCParsedAsmOperand(), Kind(K) {}

public:
  LC2200Operand(const LC2200Operand &o) : MCParsedAsmOperand() {
    Kind = o.Kind;
    StartLoc = o.StartLoc;
    EndLoc = o.EndLoc;
    switch (Kind) {
      case k_Register:
        Reg = o.Reg;
        break;
      case k_Immediate:
        Imm = o.Imm;
        break;
      case k_Token:
        Tok = o.Tok;
        break;
      case k_Memory:
        Mem = o.Mem;
        break;
    }
  }

  /// getStartLoc - Gets location of the first token of this operand
  SMLoc getStartLoc() const override { return StartLoc; }

  /// getEndLoc - Gets location of the last token of this operand
  SMLoc getEndLoc() const override { return EndLoc; }

  unsigned getReg() const override {
    assert (Kind == k_Register && "Invalid access!");
    return Reg.Num;
  }

  const MCExpr *getImm() const {
    assert (Kind == k_Immediate && "Invalid access!");
    return Imm.Val;
  }

  StringRef getToken() const {
    assert (Kind == k_Token && "Invalid access!");
    return StringRef(Tok.Data, Tok.Length);
  }

  unsigned getMemBaseNum() const {
    assert((Kind == k_Memory) && "Invalid access!");
    return Mem.BaseNum;
  }

  const MCExpr *getMemOff() const {
    assert((Kind == k_Memory) && "Invalid access!");
    return Mem.Off;
  }

  // Functions for testing operand type
  bool isReg() const override { return Kind == k_Register; }

  bool isImm() const override { return Kind == k_Immediate; }

  bool isToken() const override { return Kind == k_Token; }

  bool isMem() const override { return Kind == k_Memory; }

  void addExpr(MCInst &Inst, const MCExpr *Expr) const {
    // Add as immediates where possible. Null MCExpr = 0
    if (Expr == 0)
      Inst.addOperand(MCOperand::createImm(0));
    else if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr))
      Inst.addOperand(MCOperand::createImm(CE->getValue()));
    else
      Inst.addOperand(MCOperand::createExpr(Expr));
  }

  void addRegOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getReg()));
  }

  void addImmOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    addExpr(Inst, getImm());
  }

  void addMemOperands(MCInst &Inst, unsigned N) const {
    assert(N == 2 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getMemBaseNum()));
    addExpr(Inst, getMemOff());
  }

  void print(raw_ostream &OS) const override {
    switch (Kind) {
      case k_Register:
        OS << "Reg<" << Reg.Num << ">";
        break;
      case k_Immediate:
        OS << "Imm<" << Imm.Val << ">";
        break;
      case k_Token:
        OS << Tok.Data;
        break;
      case k_Memory:
        OS << "Mem<Reg<" << Mem.BaseNum << ">, " << Mem.Off << ">";
        break;
    }
  }

  static std::unique_ptr<LC2200Operand> CreateToken(StringRef Str, SMLoc S) {
    auto Op = std::make_unique<LC2200Operand>(k_Token);
    Op->Tok.Data = Str.data();
    Op->Tok.Length = Str.size();
    Op->StartLoc = S;
    Op->EndLoc = S;
    return Op;
  }

  static std::unique_ptr<LC2200Operand> createReg(unsigned RegNo,
                                                  SMLoc S, SMLoc E) {
    auto Op = std::make_unique<LC2200Operand>(k_Register);
    Op->Reg.Num = RegNo;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<LC2200Operand> createImm(const MCExpr *Val,
                                                  SMLoc S, SMLoc E) {
    auto Op = std::make_unique<LC2200Operand>(k_Immediate);
    Op->Imm.Val = Val;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<LC2200Operand> CreateMem(unsigned BaseNum,
                                                  const MCExpr *Off,
                                                  SMLoc S, SMLoc E) {
    auto Op = std::make_unique<LC2200Operand>(k_Memory);
    Op->Mem.BaseNum = BaseNum;
    Op->Mem.Off = Off;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }
};
} // namespace (anonymous)

static unsigned MatchRegisterName(StringRef Name); // Body is generate by tableGen

bool
LC2200AsmParser::MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode, OperandVector &Operands, MCStreamer &Out,
                                         uint64_t &ErrorInfo, bool MatchingInlineAsm) {
  MCInst Inst;

  if (MatchInstructionImpl(Operands, Inst, ErrorInfo, MatchingInlineAsm)) {
    Out.EmitInstruction(Inst, this->STI);
    return false;
  } else {
    return Error(IDLoc, "Error Parsing Instruction"); //TODO Better Info
  }
}

bool LC2200AsmParser::ParseRegister(unsigned &RegNo, OperandVector &Operands) {
  SMLoc S = Parser.getTok().getLoc();
  SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);

  switch (getLexer().getKind()) {
    default:
      return true;
    case AsmToken::Dollar:
      const AsmToken &Next = getLexer().Lex();
      if (Next.getKind() == AsmToken::Identifier) {
        StringRef regName = StringRef("$"+Next.getIdentifier().str());
        RegNo = MatchRegisterName(regName);
        if (RegNo != 0) {
          getLexer().Lex();
          Operands.push_back(LC2200Operand::createReg(RegNo, S, E));
          return false;
        }
      }
      getLexer().UnLex(Next);
      return true;
  }
}

bool LC2200AsmParser::ParseRegister(unsigned &RegNo,
                                    SMLoc &StartLoc, SMLoc &EndLoc) {
  SmallVector<std::unique_ptr<MCParsedAsmOperand>, 1> Operands;
  if (ParseRegister(RegNo, Operands))
    return true;

  LC2200Operand &Operand = static_cast<LC2200Operand &>(*Operands.front());
  StartLoc = Operand.getStartLoc();
  EndLoc = Operand.getEndLoc();
  return false;
}

bool LC2200AsmParser::ParseImmediate(OperandVector &Operands) {
  SMLoc S = Parser.getTok().getLoc();
  SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);

  const MCExpr *EVal;
  AsmToken::TokenKind kind = getLexer().getKind();
  switch (kind) {
    default:
      return true;
    case AsmToken::LParen:
    case AsmToken::Plus:
    case AsmToken::Minus:
    case AsmToken::Integer:
      if (getParser().parseExpression(EVal))
        return true;

      Operands.push_back(LC2200Operand::createImm(EVal, S, E));
      return false;
  }
}

bool LC2200AsmParser::ParseSymbolReference(OperandVector &Operands) {
  SMLoc S = Parser.getTok().getLoc();
  StringRef Identifier;
  if (Parser.parseIdentifier(Identifier))
    return true;

  SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
//    MCSymbolRefExpr::VariantKind VK = getVariantKind(Identifier);
//    if(VK != MCSymbolRefExpr::VK_None) {
//        // Parse a relocation expression.
//        SMLoc ExprS = Parser.getTok().getLoc();
//        if(getLexer().isNot(AsmToken::LParen))
//            return Error(Parser.getTok().getLoc(),
//                         "expected a parenthesized expression");
//        getLexer().Lex();
//
//        const MCExpr *EVal;
//        if(getParser().parseExpression(EVal))
//            return true;
//
//        SMLoc ExprE = Parser.getTok().getLoc();
//        if(getLexer().isNot(AsmToken::RParen))
//            return Error(ExprE, "expected a closing parenthesis");
//        getLexer().Lex();
//
////        const MCExpr *Res = evaluateRelocExpr(EVal, VK);
////        if(!Res)
////            return Error(ExprS, "unsupported relocation expression");
//
//        Operands.push_back(LC2200Operand::createImm(Res, S, ExprE));
//        return false;
//    } else {
//        // Parse a symbol
//        MCSymbol *Sym = getContext().getOrCreateSymbol(Identifier);
//        const MCExpr *Res =
//                MCSymbolRefExpr::create(Sym, MCSymbolRefExpr::VK_None, getContext());
//        Operands.push_back(OR1KOperand::createImm(Res, S, E));
//        return false;
//    }
  return false;
}

bool LC2200AsmParser::ParseOperand(OperandVector &Operands) {
  // A register operand is always alone.
  unsigned RegNo;
  if (!ParseRegister(RegNo, Operands))
    return false;

  // An immediate or expression operand can be alone or followed
  // by a base register.
  SMLoc S = getLexer().getTok().getLoc();
  if (!ParseImmediate(Operands) || !ParseSymbolReference(Operands)) {
      return false;
    // Try parsing a base register.
//    if (getLexer().is(AsmToken::LParen)) {
//      getLexer().Lex();
//
//      if (getLexer().is(AsmToken::Identifier)) {
//        unsigned BaseNum;
//        BaseNum = MatchRegisterName(getLexer().getTok().getIdentifier());
//        if (BaseNum != 0) {
//          getLexer().Lex();
//
//          if (getLexer().is(AsmToken::RParen)) {
//            getLexer().Lex();
//
//            // We have a memory operand. Grab the offset from an immediate
//            // operand we parsed earlier and put it in a memory operand.
//            std::unique_ptr<LC2200Operand> Off(
//                    static_cast<LC2200Operand *>(Operands.back().release()));
//            Operands.pop_back();
//
//            SMLoc E = getLexer().getTok().getLoc();
//            Operands.push_back(LC2200Operand::CreateMem(BaseNum, Off->getImm(), S, E));
//            return false;
//          }
//        }
//      }
//    } else {
//      // Just an immediate or expression.
//      return false;
//    }
  }

  return Error(S, "unsupported operand");
}


bool LC2200AsmParser::ParseInstruction(ParseInstructionInfo &Info,
                                       StringRef Name,
                                       SMLoc NameLoc,
                                       OperandVector &Operands) {
  // First operand is token for instruction
  Operands.push_back(LC2200Operand::CreateToken(Name, NameLoc));

  // If there are no more operands, then finish
  if (getLexer().is(AsmToken::EndOfStatement))
    return false;

  // Parse first operand
  if (ParseOperand(Operands))
    return true;

  // Parse until end of statement, consuming commas between operands
  while (getLexer().isNot(AsmToken::EndOfStatement) &&
         getLexer().is(AsmToken::Comma)) {
    // Consume comma token
    getLexer().Lex();

    // Parse next operand
    if (ParseOperand(Operands))
      return true;
  }

  if (getLexer().isNot(AsmToken::EndOfStatement))
    return Error(getLexer().getTok().getLoc(),
                 "unexpected token in operand list");

  return false;
}

bool LC2200AsmParser::ParseDirective(AsmToken DirectiveID) {
  return true;
}

extern "C" void LLVMInitializeLC2200AsmParser() {
  RegisterMCAsmParser<LC2200AsmParser> X(TheLC2200Target);
}

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION

#include "LC2200GenAsmMatcher.inc"