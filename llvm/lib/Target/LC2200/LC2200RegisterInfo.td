class LC2200Reg<bits<32> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "LC2200";
}

let Namespace = "LC2200" in {
    def zero : LC2200Reg<0, "\$zero">;
    def at: LC2200Reg<1, "\$at">;
    def v0: LC2200Reg<2, "\$v0">;
    foreach i = 0-4 in {
        def a#i : LC2200Reg<!add(i, 3), "\$a$i">
    }
    foreach i = 0-3 in {
        def s#i : LC2200Reg<!add(i, 8), "\$s$i">
    }
    def k0: LC2200Reg<12, "\$k0">;
    def sp: LC2200Reg<13, "\$sp">;
    def fp: LC2200Reg<14, "\$fp">;
    def ra: LC2200Reg<15, "\$ra">;
}

def GRRegs : RegisterClass<"LC2200", [i32], i32, 
( add at, v0, 
a0, a1, a2, a3, a4,
s0, s1, s2, s3,
k0, 
sp, // Stack Pointer
fp, // Frame Pointer
ra  // Return Address
)>;

def SRCRegs : RegisterClass<"LC2200", [i32], i32,
( add zero, v0, 
a0, a1, a2, a3, a4,
s0, s1, s2, s3,
k0, 
sp, // Stack Pointer
fp, // Frame Pointer
ra  // Return Address
)
>