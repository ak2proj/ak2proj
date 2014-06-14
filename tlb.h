#pragma once
#include <cstdint>
#include <vector>

struct tlb_entry
{
    std::uint64_t virtual_address;
    std::uint64_t physical_address;
    std::uint64_t age_counter;
    bool is_valid;
};

class tlb
{
public:
    tlb(std::size_t max_size): _buffer{max_size * 4}, _max_size(max_size), found_in_tlb(0), missed_in_tlb(0), replaced(0)
    {
    }
    
    virtual ~tlb() = default;        
    
    bool in(std::uint64_t address)
    {
        return (_buffer[(address >> 12) % _max_size].is_valid && _buffer[(address >> 12) % _max_size].virtual_address == address)
            || (_buffer[(address >> 12) % _max_size].is_valid && _buffer[(address >> 12) % _max_size + _max_size].virtual_address == address)
            || (_buffer[(address >> 12) % _max_size].is_valid && _buffer[(address >> 12) % _max_size + _max_size * 2].virtual_address == address)
            || (_buffer[(address >> 12) % _max_size].is_valid && _buffer[(address >> 12) % _max_size + _max_size * 3].virtual_address == address);
    }
    
    void save_translation(std::uint64_t address, std::uint64_t physical_address)
    {
        std::uint64_t max = _buffer[(address >> 12) % _max_size].age_counter;
        std::size_t max_i = 0;
        std::size_t index = (address >> 12) % _max_size;
        
        for (auto i = 0ull; i < 4; ++i)
        {
            auto & ref = _buffer[(address >> 12) % _max_size + _max_size * i];
            
            if (!ref.is_valid)
            {
                std::cout << "[tlb] saving translation in slot " << i << " for hash " << ((address >> 12) % _max_size) << '\n';
                
                ref = tlb_entry{ address, physical_address, 0, true };
                return;
            }
            
            if (ref.age_counter > max)
            {
                max = ref.age_counter;
                index = (address >> 12) % _max_size + _max_size * i;
            }
        }

        std::cout << "[tlb] replacing a tlb entry for 0x" << _buffer[index].virtual_address << " with an entry for 0x" << address << '\n';
        ++replaced;
        std::cout << "[tlb] saving translation in slot " << max_i << " for hash " << ((address >> 12) % _max_size) << '\n';
        _buffer[index] = tlb_entry{ address, physical_address, 0, true };
    }
    
    std::uint64_t get_translation(std::uint64_t address)
    {
        if (!in(address))
        {
            std::terminate();
        }
        
        for (auto i = 0ull; i < 4; ++i)
        {
            auto & ref = _buffer[(address >> 12) % _max_size + _max_size * i];
            
            if (ref.virtual_address == address && ref.is_valid)
            {
                ++ref.age_counter;
            }
        }

        for (auto i = 0ull; i < 4; ++i)
        {
            auto & ref = _buffer[(address >> 12) % _max_size + _max_size * i];
            
            if (ref.virtual_address == address && ref.is_valid)
            {
                ref.age_counter = 0;
                return ref.physical_address;
            }
        }
        
        std::terminate();
    }
    
    void invalidate_translation(std::uint64_t address)
    {
        if (!in(address))
        {
            return;
        }
        
        for (auto i = 0ull; i < 4; ++i)
        {
            auto & ref = _buffer[(address >> 12) % _max_size + _max_size * i];
            
            if (ref.virtual_address == address && ref.is_valid)
            {
                ref.is_valid = false;
                return;
            }
        }
        
        std::terminate();
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
