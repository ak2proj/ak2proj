#pragma once

#include "core.h"
#include "instruction.h"

class basic_decoder
{
public:
    basic_decoder(core<basic_decoder> & core) : _core(core)
    {
    }
    
    instruction decode() const
    {
        return {};
    }
    
private:
    core<basic_decoder> & _core;
};
