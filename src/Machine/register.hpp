#pragma once

namespace RISCVS {

class Register {
public:
    using RegisterType = uint32_t;

    enum class REGISTER_MODE {
        DEFAULT,
        ZERO,
    };

    Register(REGISTER_MODE mode = REGISTER_MODE::DEFAULT) : mode(mode) {}

    Register(const Register&) = delete;
    Register& operator=(const Register&) = delete;
    Register(Register&&) = delete;
    Register& operator=(Register&&) = delete;

    Register& operator=(const RegisterType val) {
        switch (mode) {
            case REGISTER_MODE::DEFAULT: {
                value = val;
                break;
            }
            case REGISTER_MODE::ZERO: {
                value = 0U;
                break;
            }
        }

        return *this;
    }

    operator RegisterType() const {
        return value;
    }

private:
    REGISTER_MODE mode;
    RegisterType value = 0U;
};

}