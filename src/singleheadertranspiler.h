#pragma once
#include "transpiler.h"

namespace htcpp{

class SingleHeaderTranspiler : public Transpiler{
public:
    explicit SingleHeaderTranspiler(std::string  className);
    std::string generateCode() const override;

private:
    std::string className_;
};

}
