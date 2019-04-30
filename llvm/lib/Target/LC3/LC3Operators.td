def LC3imm5 : Operand<i16>, ImmLeaf<i16, [{
  return Imm >= 0 && Imm < 32;
}]>;