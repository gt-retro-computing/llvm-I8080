class ECE2031Reg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "ECE2031";
  int SpillSize = 1;
  int SpillAlignment = 1;
}

def acc: ECE2031Reg<1, "AC">;

def AccReg : RegisterClass<"ECE2031", [i16], 16,
( add acc
)>;
