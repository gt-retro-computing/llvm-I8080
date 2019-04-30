class LC3Reg<bits<16> Enc, string n> : Register<n> {
    let HWEncoding = Enc;
    let Namespace = "LC3";
}

foreach i = 0-7 in {
    def R#i : LC3Reg<i, "r"#i>;
}

def GRRegs : RegisterClass<"LC3", [i16], 16,
  (add R0, R1, R2, R3, R4, R5, R6, R7)>;

