#pragma once

#include "REPL.h"

namespace lpubsppop01 {
namespace REPLReaderWriter {

ref class REPLTemplates abstract sealed
{
public:
    static REPL^ GetByName(String^ templateName);
};

}
}