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
        bool read; // otherwise, write
        bool data; // otherwise, code
    };
    
    static inline access_type data_read()
    {
        return { true, true };
    }
    
    static inline access_type data_write()
    {
        return { false, true };
    }
    
    static inline access_type code_read()
    {
        return { true, false };
    }
 
    virtual std::uintmax_t translate(std::uintmax_t address, access_type type = data_read()) const = 0;
    
    void enable()
    {
        _enabled = true;
    }
    
    void disable()
    {
        _enabled = false;
    }
    
protected:
    const std::shared_ptr<memory> _mem;
    std::unordered_map<std::uintmax_t, std::uintmax_t> _registers;
    bool _enabled = false;
};

class amd64_mmu : public mmu
{
public:
    using mmu::mmu;
    
    virtual std::uintmax_t translate(std::uintmax_t address, access_type = data_read()) const override
    {
        if (_enabled)
        {
            // insert mmu translation code here
            return 0;
        }
        
        else
        {
            return address;
        }
    }
};
