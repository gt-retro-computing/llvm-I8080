def RetCC_LC3 : CallingConv<[
    CCIfType<[i16], CCAssignToStack<1,0>>
]>;

def CC_LC3 : CallingConv<[
    CCIfType<[i8], CCPromoteToType<i16>>,
    CCIfType<[i16], CCAssignToStack<1,0>>
]>;

def CC_Save : CalleeSavedRegs<(add R0, R1, R2, R3, R4, R7)>;
