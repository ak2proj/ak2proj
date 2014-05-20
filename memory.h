#pragma once

#include <iostream>
#include <memory>

class memory
{
public:
    virtual ~memory() {}
    
    virtual void write(std::uintmax_t address, std::uint8_t value) = 0;
    virtual void write(std::uintmax_t address, std::uint16_t value) = 0;
    virtual void write(std::uintmax_t address, std::uint32_t value) = 0;
    virtual void write(std::uintmax_t address, std::uint64_t value) = 0;
    virtual void read(std::uintmax_t address, std::uint8_t & value) const = 0;
    virtual void read(std::uintmax_t address, std::uint16_t & value) const = 0;
    virtual void read(std::uintmax_t address, std::uint32_t & value) const = 0;
    virtual void read(std::uintmax_t address, std::uint64_t & value) const = 0;
};
 
class physical_memory : public memory
{
public:
    physical_memory(std::uintmax_t size) : _size{ size }, _memory{ new std::uint8_t[size]() }
    {
    }
    
    virtual void write(std::uintmax_t address, std::uint8_t value) override
    {
        _write(address, value);
    }
    
    virtual void write(std::uintmax_t address, std::uint16_t value) override
    {
        _write(address, value);
    }
    
    virtual void write(std::uintmax_t address, std::uint32_t value) override
    {
        _write(address, value);
    }
    
    virtual void write(std::uintmax_t address, std::uint64_t value) override
    {
        _write(address, value);
    }
    
    virtual void read(std::uintmax_t address, std::uint8_t & value) const override
    {
        _read(address, value);
    }
    
    virtual void read(std::uintmax_t address, std::uint16_t & value) const override
    {
        _read(address, value);
    }
    
    virtual void read(std::uintmax_t address, std::uint32_t & value) const override
    {
        _read(address, value);
    }
    
    virtual void read(std::uintmax_t address, std::uint64_t & value) const override
    {
        _read(address, value);
    }

private:
    template<typename Datatype>
    void _write(std::uintmax_t address, Datatype value)
    {
        if (address > _size)
        {
            throw std::out_of_range{ "out of bounds access to physical_memory" };
        }
        
        std::cout << "[physmem] writing 0x" << static_cast<std::uintmax_t>(value) << " of size " << std::dec << sizeof(Datatype) << std::hex << " at 0x" << address << '\n';
        
        for (std::uintmax_t i = 0; i < sizeof(Datatype); ++i)
        {
            _memory[address++] = (value >> (i * 8)) & 0xff;
        }
    }
    
    template<typename Datatype>
    void _read(std::uintmax_t address, Datatype & ret) const
    {
        ret = {};
        
        if (address > _size)
        {
            throw std::out_of_range{ "out of bounds access to physical_memory" };
        }
        
        std::cout << "[physmem] reading value of size " << std::dec << sizeof(Datatype) << std::hex << " from 0x" << address << '\n';
        
        for (std::uintmax_t i = 0; i < sizeof(Datatype); ++i)
        {
            ret |= static_cast<std::uint64_t>(_memory[address++]) << (i * 8);
        }
        
        std::cout << "[physmem] *0x" << address - sizeof(Datatype) << " = 0x" << static_cast<std::uintmax_t>(ret) << '\n';
    }
    
    std::uintmax_t _size;
    std::unique_ptr<std::uint8_t[]> _memory;
};
