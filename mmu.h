#pragma once

#include <exception>
#include <cstdint>
#include <unordered_map>

#include "memory.h"

class page_fault : public std::runtime_error
{
};
 
class mmu
{
public:
    mmu(std::shared_ptr<memory> mem) : _mem{ std::move(mem) }
    {
    }
    
    virtual ~mmu() {}
 
    virtual void write_register(std::uintmax_t value, std::uintmax_t reg = 0)
    {
        _registers[reg] = value;
    }
    
    virtual std::uintmax_t read_register(std::uintmax_t reg = 0) const
    {
        return _registers.at(reg);
    }
    
    struct access_type
    {
        bool read = true; // otherwise, write
        bool data = true; // otherwise, code
    };
    
    static constexpr access_type data_read = { true, true };
    static constexpr access_type data_write = { false, true};
    static constexpr access_type code_read = { true, false };
 
    virtual std::uintmax_t translate(std::uintmax_t address, access_type type = {}) const = 0;
    
protected:
    const std::shared_ptr<memory> _mem;
    std::unordered_map<std::uintmax_t, std::uintmax_t> _registers;
};

class amd64_mmu : public mmu
{
public:
    using mmu::mmu;
    
    virtual std::uintmax_t translate(std::uintmax_t, access_type = {}) const override
    {
        return 0;
    }
};
