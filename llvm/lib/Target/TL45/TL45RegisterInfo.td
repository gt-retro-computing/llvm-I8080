class TL45Reg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "TL45";
  int SpillSize = 1;
  int SpillAlignment = 1;
}

def r0  : TL45Reg< 0,  "r0">;
def r1  : TL45Reg< 1,  "r1">;
def r2  : TL45Reg< 2,  "r2">;
def r3  : TL45Reg< 3,  "r3">;
def r4  : TL45Reg< 4,  "r4">;
def r5  : TL45Reg< 5,  "r5">;
def r6  : TL45Reg< 6,  "r6">;
def r7  : TL45Reg< 7,  "r7">;
def r8  : TL45Reg< 8,  "r8">;
def r9  : TL45Reg< 9,  "r9">;
def r10 : TL45Reg<10, "r10">;
def r11 : TL45Reg<11, "r11">;
def r12 : TL45Reg<12, "r12">;
def r13 : TL45Reg<13, "r13">;
def bp  : TL45Reg<14,  "bp">;
def sp  : TL45Reg<15,  "sp">;

def pc  : TL45Reg<17, "pc">;
def eflags  : TL45Reg<18, "eflags">;


def GRRegs : RegisterClass<"TL45", [i32], 32, 
( add
r0, r1, r2, r3,
r4, r5, r6, r7,
r8, r9, r10, r11,
r12, r13, bp, sp
)>;

def SRCRegs : RegisterClass<"TL45", [i32], 32,
( add
r0, r1, r2, r3,
r4, r5, r6, r7,
r8, r9, r10, r11,
r12, r13, bp, sp
)>;
