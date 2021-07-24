#pragma once
#include "transpiler.h"

namespace htcpp {

class SharedLibTranspiler : public Transpiler{
private:
    std::string generateCode() const override;
};

}

