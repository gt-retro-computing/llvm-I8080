class SDTCisChain<int OpNum> : SDTCisVT<OpNum, OtherVT>;
class SDTCisI8   <int OpNum> : SDTCisVT<OpNum, i8>;
class SDTCisFlag <int OpNum> : SDTCisI8<OpNum>;
class SDTCisI16  <int OpNum> : SDTCisVT<OpNum, i16>;
class SDTCisPtr  <int OpNum> : SDTCisVT<OpNum, iPTR>;

def SDTUnOpRF   : SDTypeProfile<2, 1, [SDTCisInt<0>,
                                       SDTCisFlag<1>,
                                       SDTCisSameAs<2, 0>]>;
def SDTUnOpRFF  : SDTypeProfile<2, 2, [SDTCisInt<0>,
                                       SDTCisFlag<1>,
                                       SDTCisSameAs<2, 0>,
                                       SDTCisFlag<3>]>;
def SDTBinOpRF  : SDTypeProfile<2, 2, [SDTCisInt<0>,
                                       SDTCisFlag<1>,
                                       SDTCisSameAs<2, 0>,
                                       SDTCisSameAs<3, 0>]>;
def SDTBinOpRFF : SDTypeProfile<2, 3, [SDTCisInt<0>,
                                       SDTCisFlag<1>,
                                       SDTCisSameAs<2, 0>,
                                       SDTCisSameAs<3, 0>,
                                       SDTCisFlag<4>]>;
def SDTBinOpF   : SDTypeProfile<1, 2, [SDTCisFlag<0>,
                                       SDTCisInt<1>,
                                       SDTCisSameAs<2, 1>]>;
def SDTI8080Wrapper       : SDTypeProfile<1, 1, [SDTCisPtrTy<0>,
                                               SDTCisSameAs<1, 0>]>;
def SDT_I8080mlt          : SDTypeProfile<1, 1, [SDTCisI16<0>, SDTCisI16<1>]>;
def SDT_I8080sext         : SDTypeProfile<1, 1, [SDTCisInt<0>, SDTCisFlag<1>]>;
def SDT_I8080TCRet        : SDTypeProfile<0, 1, [SDTCisPtrTy<0>]>;
def SDT_I8080Call         : SDTypeProfile<0, -1, [SDTCisPtr<0>]>;
def SDT_I8080CallSeqStart : SDCallSeqStart<[SDTCisPtr<0>, SDTCisPtr<1>]>;
def SDT_I8080CallSeqEnd   : SDCallSeqEnd<[SDTCisPtr<0>, SDTCisPtr<1>]>;
def SDT_I8080BrCond       : SDTypeProfile<0, 3, [SDTCisChain<0>,
                                               SDTCisI8<1>,
                                               SDTCisFlag<2>]>;
def SDT_I8080Select       : SDTypeProfile<1, 4, [SDTCisInt<0>,
                                               SDTCisSameAs<1, 0>,
                                               SDTCisSameAs<2, 0>,
                                               SDTCisI8<3>,
                                               SDTCisI8<4>]>;
def SDT_I8080Pop          : SDTypeProfile<1, 0, [SDTCisPtr<0>]>;
def SDT_I8080Push         : SDTypeProfile<0, 1, [SDTCisPtr<0>]>;
