// Inserted within v20cns; bounded on-foot DTE authorization, not a world capability.
class DteLocalMotorSeal final {
    friend struct ::GTAngelAGI;
    friend class SpinalReflex;
    uint64_t frame_,digest_,issuedMs_,bodySequence_,worldVersion_;
    float forward_,turn_;
    DteLocalMotorSeal(uint64_t f,uint64_t d,uint64_t t,uint64_t b,uint64_t w,float forward,float turn)
        :frame_(f),digest_(d),issuedMs_(t),bodySequence_(b),worldVersion_(w),forward_(forward),turn_(turn){}
public:
    DteLocalMotorSeal()=delete;
};
