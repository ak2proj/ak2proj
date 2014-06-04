#pragma once

#include <exception>
#include <cstdint>
#include <unordered_map>

#include "memory.h"
#include "tlb.h"

class page_fault : public std::runtime_error
{
};
 
class mmu
{
public:
    mmu(std::shared_ptr<memory> mem, std::uintmax_t tlb_size) : _mem{ std::move(mem)}, _tlb{ new tlb(tlb_size) }
    {
    }
    
    virtual ~mmu() {}
 
    virtual void write_register(std::uint64_t value, std::uint64_t reg = 0)
    {
        _registers[reg] = value;
    }
    
    virtual std::uint64_t read_register(std::uint64_t reg = 0) const
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
 
    virtual std::uint64_t translate(std::uint64_t address, access_type type = data_read()) const = 0;
    
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
    std::unordered_map<std::uint64_t, std::uint64_t> _registers;
    std::unique_ptr<tlb> _tlb;
    bool _enabled = false;
};

class amd64_mmu : public mmu
{
public:
    using mmu::mmu;
    
    void statistics()
    {
        std::cout<<"[tlb statistic] TLB usage rate is: "<<(100*_tlb->getFound()/(float)(_tlb->getFound()+_tlb->getMissed()))<<" with "<<_tlb->getFound()<<" founds and ";
        std::cout<<_tlb->getMissed()<<" misses"<<std::endl;
    }
    
    virtual std::uint64_t translate(std::uint64_t address, access_type = data_read()) const override
    {
    
        if (_enabled)
        {
            std::uint64_t rest = address & 4095ull;
            std::uint64_t page = address & ~4095ull;
            
            if (_tlb->in(page))
            {
                _tlb->found();
            
                std::cout << "[mmu] page 0x" << page << " found in tlb while translating 0x" << address << '\n';
                std::uint64_t result = _tlb->get_translation(page);
                
                if (!(result & 1))
                    throw std::runtime_error{"Not present"};
                    
                result &= ~4095ull;
                    
                std::cout << "[mmu] saved translation for 0x" << page << " is 0x" << result << '\n';
                return result | rest;
            }
            
            else
            {
                _tlb->missed();
                
                std::cout << "[mmu] 0x" << address << " not in tlb\n";
                std::cout << "[mmu] attempting to translate 0x" << address << '\n';
            
                //the first address from cr3
                std::uint64_t cr3 = read_register();
            
                std::cout << "[mmu] cr3 = 0x" << cr3 << '\n';
            
                cr3 &= ~4095ull;
                std::cout << "[mmu] pml4 is at 0x" << cr3 << '\n';

                std::uint64_t pml4e = (address >> 39) & 511;
                std::uint64_t pdpte = (address >> 30) & 511;
                std::uint64_t pde = (address >> 21) & 511;
                std::uint64_t pte = (address >> 12) & 511;
        
                std::cout << "[mmu] physical address is pml4[0x" << pml4e << "][0x" << pdpte << "][0x" << pde << "][0x" << pte << "] | 0x" << rest << '\n';

                std::uint64_t pml4 = cr3;
                std::uint64_t pdpt;
                std::uint64_t pd;
                std::uint64_t pt;
                std::uint64_t physical_address;

                _mem->read(pml4 + pml4e * 8, pdpt);
                std::cout << "[mmu] pml4[0x" << pml4e << "] = 0x" << pdpt << '\n';
                if (!(pdpt & 1))
                {
                    throw std::runtime_error{ "not present" };
                }
            
                pdpt &= ~4095ull;
                std::cout << "[mmu] pdpt is at 0x" << pdpt << '\n';
            
                _mem->read(pdpt + pdpte * 8, pd);
                std::cout << "[mmu] pdpt[0x" << pdpte << "] = 0x" << pd << '\n';
                if (!(pd & 1))
                {
                    throw std::runtime_error{ "not present" };
                }

                pd &= ~4095ull;
                std::cout << "[mmu] pd is at 0x" << pd << '\n';
            
                _mem->read(pd + pde * 8, pt);
                std::cout << "[mmu] pd[0x" << pde << "] = 0x" << pt << '\n';
                if (!(pt & 1))
                {
                    throw std::runtime_error{ "not present" };
                }

                pt &= ~4095ull;
                std::cout << "[mmu] pt is at 0x" << pt << '\n';
            
                _mem->read(pt + pte * 8, physical_address);
                std::cout << "[mmu] pt[0x" << pte << "] = 0x" << physical_address << '\n';
                if (!(physical_address & 1))
                {
                    throw std::runtime_error{ "not present" };
                }

                _tlb->save_translation(address & ~4095ull, physical_address);
    
                physical_address &= ~4095ull;
                physical_address |= rest;

                std::cout << "[mmu] translated 0x" << address << " to 0x" << physical_address << '\n';

                return physical_address;
            }
        }
        
        else
        {
            return address;
        }
    }
};
