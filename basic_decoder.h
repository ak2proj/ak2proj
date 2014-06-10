#pragma once

#include <map>

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
        static std::map<std::uint8_t, instructions> instruction_map{
            { 0, instructions::exit }, 
            { 1, instructions::read }, 
            { 2, instructions::write }, 
            { 3, instructions::load },
            { 4, instructions::load_paging_register }, 
            { 5, instructions::jump }, 
            { 6, instructions::invlpg }
        };
        
        static std::map<instructions, std::string> instruction_names{
            { instructions::exit, "exit" }, 
            { instructions::read, "read" }, 
            { instructions::write, "write" },
            { instructions::load, "load" }, 
            { instructions::load_paging_register, "load paging register" },
            { instructions::jump, "jump" }, 
            { instructions::invlpg, "invlpg" }
        };
        
        std::cout << "[decoder] decoding instruction at 0x" << _core.ip() << '\n';
        
        instructions type = instruction_map[_core._read<std::uint8_t>(_core.ip(), true)];
        operand dest, src;
        std::uintmax_t length = 1;
        
        if (type == instructions::read || type == instructions::load)
        {
            dest = { true, _core._read<std::uint8_t>(_core.ip() + length) };
            ++length;
        }
        
        else if (type == instructions::write)
        {
            dest = { false, _core._read<std::uint64_t>(_core.ip() + length) };
            length += 8;
        }
        
        if (type == instructions::read || type == instructions::load || type == instructions::invlpg || type == instructions::jump)
        {
            src = { false, _core._read<std::uint64_t>(_core.ip() + length) };
            length += 8;
        }
        
        else if (type == instructions::write || type == instructions::load_paging_register)
        {
            src = { true, _core._read<std::uint8_t>(_core.ip() + length) };
            ++length;
        }
        
        std::cout << "[decoder] decoded instruction `" << instruction_names[type] << "`\n instruction length: " << std::dec << length << std::hex;
        
        if (dest.is_valid())
        {
            std::cout << "\n destination: ";
            
            if (dest.is_register())
            {
                std::cout << "r" << dest.register_index();
            }
            
            else 
            {
                std::cout << "0x" << dest.value();
            }
        }
            
        if (src.is_valid())
        {
            std::cout << "\n source: ";
            
            if (src.is_register())
            {
                std::cout << "r" << src.register_index();
            }
            
            else 
            {
                std::cout << "0x" << src.value();
            }
        }

        std::cout << '\n';
        
        return { type, length, dest, src };
    }
    
private:
    core<basic_decoder> & _core;
};
