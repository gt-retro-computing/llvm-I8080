def RetCC_I8080 : CallingConv<[
    CCIfType<[i8], CCAssignToReg<[A, L, H, E, D, C, B]>>,
    CCIfType<[i16], CCAssignToReg<[BC, DE, HL]>>
]>;

def CC_I8080 : CallingConv<[
    CCIfType<[i8], CCAssignToReg<[A, B, C, D, E, H, L]>>,
    CCIfType<[i16], CCAssignToReg<[BC, DE, HL]>>,
    CCIfType<[i8], CCAssignToStack<1,1>>
    CCIfType<[i16], CCAssignToStack<2,1>>
]>;

def CC_Save : CalleeSavedRegs<(add A, BC, DE, HL)>;