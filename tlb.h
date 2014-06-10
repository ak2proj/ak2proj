#pragma once
#include <cstdint>
#include <vector>

struct tlb_entry
{
    std::uint64_t virtual_address;
    std::uint64_t physical_address;
    bool is_valid;
};

class tlb
{
public:
    tlb(std::size_t max_size): _buffer{max_size}, _max_size(max_size), found_in_tlb(0), missed_in_tlb(0), replaced(0)
    {
    }
    
    virtual ~tlb() = default;        
    
    bool in(std::uint64_t address)
    {
        return _buffer[(address >> 12) % _max_size].is_valid && _buffer[(address >> 12) % _max_size].virtual_address == address; 
    }
    
    void save_translation(std::uint64_t address, std::uint64_t physical_address)
    {
        if (_buffer[(address >> 12) % _max_size].is_valid)
        {
            std::cout << "[tlb] replacing a tlb entry for 0x" << _buffer[(address >> 12) % _max_size].virtual_address << " with an entry for 0x" << address << '\n';
            ++replaced;
        }
        
        _buffer[(address >> 12) % _max_size] = tlb_entry{ address, physical_address, true };
    }
    
    std::uint64_t get_translation(std::uint64_t address)
    {
        if (!in(address))
        {
            std::terminate();
        }
        
        return _buffer[(address >> 12) % _max_size].physical_address;
    }
    
    void invalidate_translation(std::uint64_t address)
    {
        if (!in(address))
        {
            return;
        }
        
        _buffer[(address >> 12) % _max_size].is_valid = false;
    }
    
    void found()
    {
        ++found_in_tlb;
    }
    
    void missed()
    {
        ++missed_in_tlb;
    }
    
    int getFound() const { return found_in_tlb;}
    int getMissed() const { return missed_in_tlb;}
    int getReplaced() const { return replaced; }
    
protected:
    std::vector<tlb_entry> _buffer;
    std::size_t _max_size;
    int found_in_tlb;
    int missed_in_tlb;
    int replaced;
};
