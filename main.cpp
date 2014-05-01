#include <iostream>
#include <memory>

#include "memory.h"
#include "mmu.h"
#include "core.h"
#include "basic_decoder.h"

int main()
{
    std::cout << std::hex;
    
    auto phys = std::make_shared<physical_memory>(4096 * 4096);
    auto mmu = std::make_shared<amd64_mmu>(phys);
    
    core<basic_decoder> cpu{ mmu, phys };
    cpu.loop();
}
