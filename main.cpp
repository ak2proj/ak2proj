#include <iostream>
#include <memory>
#include <fstream>

#include "memory.h"
#include "mmu.h"
#include "core.h"
#include "basic_decoder.h"

int main(int argc,  char ** argv)
{
    std::cout << std::hex;
    
    auto phys = std::make_shared<physical_memory>(4096 * 4096);
    auto mmu = std::make_shared<amd64_mmu>(phys, 64);
    
    if (argc < 2)
    {
        std::cout << "Please provide a binary file for the program.\n";
        return 1;
    }
    
    std::fstream in{ argv[1], std::ios::in | std::ios::binary };
    std::vector<char> program;
    std::copy(std::istreambuf_iterator<char>{ in.rdbuf() }, std::istreambuf_iterator<char>{}, std::back_inserter(program));
    
    std::cout << "[main] ---=== loading program ===---\n";
    
    std::uintmax_t address = 0;
    for (auto byte : program)
    {
        phys->write(address++, static_cast<uint8_t>(byte));
    }
    
    std::cout << "[main] ---=== program loaded ===---\n";
    
    core<basic_decoder> cpu{ mmu, phys };
    cpu.loop();
    
    mmu->statistics();
    std::cout << "[main] ---=== exiting ===---\n";
}
