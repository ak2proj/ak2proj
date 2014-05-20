#pragma once

#include <iostream>
#include <cstdint>

#include "memory.h"
#include "mmu.h"
#include "instruction.h"

template<typename Decoder>
class core
{
    friend Decoder;
public:
    core(std::shared_ptr<mmu> mmu, std::shared_ptr<memory> mem) : _mmu{ std::move(mmu) }, _mem{ std::move(mem) }
    {
    }
    
    void loop()
    {
        Decoder dec{ *this };
        
        while (!_done)
        {
            _execute(dec.decode());
        }
    }
    
    std::uintmax_t ip() const
    {
        return _instruction_ptr;
    }
    
private:
    void _execute(const instruction & inst)
    {
        if (inst.type() == instructions::exit)
        {
            _done = true;
            return;
        }
        
        switch (inst.type())
        {
            case instructions::read:
                std::cout << "[cpu] reading value from 0x" << inst.source_operand().value() << " into r" << inst.destination_operand().register_index() << '\n';
                _registers[inst.destination_operand().register_index()] = _read<std::uint64_t>(inst.source_operand().value());
                std::cout << "[cpu] read 0x" << _registers[inst.destination_operand().register_index()] << '\n';
                break;
            
            case instructions::write:
                std::cout << "[cpu] writing 0x" << _registers[inst.source_operand().register_index()] << " from r" << inst.source_operand().register_index() 
                    << " to memory at 0x" << inst.destination_operand().value() << '\n';
                _write(inst.destination_operand().value(), _registers[inst.source_operand().register_index()]);
                break;
            
            case instructions::load:
                std::cout << "[cpu] loading 0x" << inst.source_operand().value() << " into r" << inst.destination_operand().register_index() << '\n';
                _registers[inst.destination_operand().register_index()] = inst.source_operand().value();
                break;
                    
            case instructions::load_paging_register:
                std::cout << "[cpu] loading 0x" << _registers[inst.source_operand().register_index()] << " into mmu primary register\n";
                _mmu->write_register(_registers[inst.source_operand().register_index()]);
                _mmu->enable();
                break;
            
            default:
                throw std::runtime_error{ "attempting to execute an unknown instruction" };
        }
        
        _instruction_ptr += inst.length();
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
    
    std::uintmax_t _instruction_ptr = 0;
    std::uint64_t _registers[8] = {};
    bool _done = false;
};
