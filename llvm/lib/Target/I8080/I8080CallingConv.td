def RetCC_I8080 : CallingConv<[
    CCIfType<[i8], CCAssignToReg<[A]>>,
    CCIfType<[i16], CCAssignToReg<[BC]>>,
    CCIfType<[i8], CCAssignToStack<1,1>>,
    CCIfType<[i16], CCAssignToStack<2,1>>
]>;

def CC_I8080 : CallingConv<[
    CCIfType<[i8], CCAssignToReg<[A,B,C]>>,
    CCIfType<[i16], CCAssignToReg<[BC]>>,
    CCIfType<[i8], CCAssignToStack<1,1>>,
    CCIfType<[i16], CCAssignToStack<2,1>>
]>;

def CC_Save : CalleeSavedRegs<(add DE, HL)>;