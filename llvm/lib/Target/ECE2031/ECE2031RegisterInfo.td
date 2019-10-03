class ECE2031Reg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "ECE2031";
  int SpillSize = 1;
  int SpillAlignment = 1;
}

def zero : ECE2031Reg<0, "$zero">;
def at: ECE2031Reg<1, "$at">;
def v0: ECE2031Reg<2, "$v0">;
def a0 : ECE2031Reg<3, "$a0">;
def a1 : ECE2031Reg<4, "$a1">;
def a2 : ECE2031Reg<5, "$a2">;
def t0 : ECE2031Reg<6, "$t0">;
def t1 : ECE2031Reg<7, "$t1">;
def t2 : ECE2031Reg<8, "$t2">;
def s0 : ECE2031Reg<9, "$s0">;
def s1 : ECE2031Reg<10, "$s1">;
def s2 : ECE2031Reg<11, "$s2">;
def k0: ECE2031Reg<12, "$k0">;
def sp: ECE2031Reg<13, "$sp">;
def fp: ECE2031Reg<14, "$fp">;
def ra: ECE2031Reg<15, "$ra">;
def pc: ECE2031Reg<16, "$pc">;


def GRRegs : RegisterClass<"ECE2031", [i32], 32,
( add zero, at, v0, 
a0, a1, a2, 
t0, t1, t2,
s0, s1, s2,
k0, 
sp, // Stack Pointer
fp, // Frame Pointer
ra  // Return Address
)>;

def SRCRegs : RegisterClass<"ECE2031", [i32], 32,
( add zero, at, v0, 
a0, a1, a2, 
t0, t1, t2,
s0, s1, s2, 
k0, 
sp, // Stack Pointer
fp, // Frame Pointer
ra  // Return Address
)>;