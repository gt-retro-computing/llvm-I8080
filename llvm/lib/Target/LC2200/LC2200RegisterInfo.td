class LC2200Reg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "LC2200";
}

let Namespace = "LC2200" in {
    def zero : LC2200Reg<0, "$zero">;
    def at: LC2200Reg<1, "$at">;
    def v0: LC2200Reg<2, "$v0">;
    def a0 : LC2200Reg<3, "$a0">;
    def a1 : LC2200Reg<4, "$a1">;
    def a2 : LC2200Reg<5, "$a2">;
    def a3 : LC2200Reg<6, "$a3">;
    def a4 : LC2200Reg<7, "$a4">;
    def s0 : LC2200Reg<8, "$s0">;
    def s1 : LC2200Reg<9, "$s1">;
    def s2 : LC2200Reg<10, "$s2">;
    def s3 : LC2200Reg<11, "$s3">;
    def k0: LC2200Reg<12, "$k0">;
    def sp: LC2200Reg<13, "$sp">;
    def fp: LC2200Reg<14, "$fp">;
    def ra: LC2200Reg<15, "$ra">;
}

def GRRegs : RegisterClass<"LC2200", [i32], 32, 
( add at, v0, 
a0, a1, a2, a3, a4,
s0, s1, s2, s3,
k0, 
sp, // Stack Pointer
fp, // Frame Pointer
ra  // Return Address
)>;

def SRCRegs : RegisterClass<"LC2200", [i32], 32,
( add zero, v0, 
a0, a1, a2, a3, a4,
s0, s1, s2, s3,
k0, 
sp, // Stack Pointer
fp, // Frame Pointer
ra  // Return Address
)>;