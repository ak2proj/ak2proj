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
            //the first address from cr3
            std::uintmax_t cr3 = read_register(); // default - r0


            std::uintmax_t pml4e = (address >> 39) & 511;
            std::uintmax_t pdpte = (address >> 30) & 511;
            std::uintmax_t pde = (address >> 21) & 511;
            std::uintmax_t pte = (address >> 12) & 511;

            std::uintmax_t pml4 = cr3;
            std::uintmax_t pdpt;
            std::uintmax_t pd;
            std::uintmax_t pt;
            std::uintmax_t physical_address;

            _mem->read(pml4+pml4e, pdpt);
            _mem->read(pdpt+pdpte, pd);
            _mem->read(pd+pde,     pt);
            _mem->read(pt+pte, physical_address);


            //& ~0xFFF flags?

            std::cout<<"MMU TRANSLATED virtual address:"<<std::hex<<address<<" into physical address:"<<physical_address<<std::endl;

            return physical_address;
        }
        
        else
        {
            return address;
        }
    }
};
