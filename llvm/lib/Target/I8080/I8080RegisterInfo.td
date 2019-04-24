class I8080Reg<string n, bits<16> Enc = -1> : Register<n> {
    let Namespace = "I8080";
    let HWEncoding = Enc;
}
class I8080RegWithSubRegs<string n, list<Register> sub = [], bits<16> enc = -1>
    :I8080Reg<n, enc> {
        let SubRegs = sub;
    }

let Namespace = "I8080" in {
    def sub_low : SubRegIndex<8>;
    def sub_high : SubRegIndex<8, 8>;
    def sub_short : SubRegIndex<16>;
}

def A : I8080Reg<"A", 7>;
def F : I8080Reg<"F">;
def B : I8080Reg<"B", 0>;
def C : I8080Reg<"C", 1>;
def D : I8080Reg<"D", 2>;
def E : I8080Reg<"E", 3>;
def H : I8080Reg<"H", 4>;
def L : I8080Reg<"L", 5>;

let SubRegIndices = [sub_high, sub_low], CoveredBySubRegs = 1 in {
    def AF : I8080RegWithSubRegs<"AF", [A,F], 3>;
    def BC : I8080RegWithSubRegs<"BC", [B,C], 0>;
    def DE : I8080RegWithSubRegs<"DE", [D,E], 1>;
    def HL : I8080RegWithSubRegs<"HL", [H,L], 2>;
}

def SP : I8080Reg<"SP", 3>;
def PC : I8080Reg<"PC">;

/* 
Copied from LLVM source for easy read
class RegisterClass<string namespace, list<ValueType> regTypes,
 int alignment, dag regList, RegAltNameIndex idx = NoRegAltName> */
class I8080RC8 <dag regList> : RegisterClass<"I8080", [i8 ], 8, regList>;
class I8080RC16<dag regList> : RegisterClass<"I8080", [i16], 8, regList>;

let CopyCost = -1,isAllocatable = 0 in
  def F8 : I8080RC8 <(add F)>;
def ALUR : I8080RC8<(add A)>;
def OR8 : I8080RC8<(add B, C, D, E, H, L)>; // OR: Operand
def GPR8 : I8080RC8<(add A, B, C, D, E, H, L)>;
def GPR16: I8080RC16<(add BC, DE, HL)>;

