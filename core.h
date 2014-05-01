#pragma once

#include <iostream>
#include <cstdint>

#include "memory.h"
#include "mmu.h"
#include "instruction.h"

template<typename Decoder>
class core
{
public:
    core(std::shared_ptr<mmu> mmu, std::shared_ptr<memory> mem) : _mmu{ std::move(mmu) }, _mem{ std::move(mem) }
    {
    }
    
    void loop()
    {
        Decoder dec{ *this };
        
//        while (true) // <- this has to be enable in real code
        {
            _execute(dec.decode());
        }
    }
    
    std::uintmax_t ip() const
    {
        return _instruction_ptr;
    }
    
private:
    void _execute(const instruction &)
    {
        _write(0x123, 0x456ul);
        std::cout << _read<std::uint64_t>(0x123) <<  '\n';
    }
    
    template<typename Datatype>
    void _write(std::uintmax_t address, Datatype value)
    {
        _mem->write(_mmu->translate(address), value);
    }
    
    template<typename Datatype>
    Datatype _read(std::uintmax_t address)
    {
        Datatype ret{};
        _mem->read(_mmu->translate(address), ret);
        return ret;
    }
    
    const std::shared_ptr<mmu> _mmu;
    const std::shared_ptr<memory> _mem;
    
    std::uintmax_t _instruction_ptr;
    std::uint64_t _registers[8] = {};
};
