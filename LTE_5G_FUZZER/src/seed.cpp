#include "seed.h"

#include "helpers.h"

#include <iostream>

uint64_t Seed::total_seeds = 0;

std::ostream &Seed::dump(std::ostream& os) const
{
    os << GREEN << "================ SEED ID: " << id << " ================" << std::endl;
    os << "TOTAL PATCHES: " << patches.size() << std::endl;
    for (auto patch_ptr : patches) {
        patch_ptr->dump(os);
    }
    os << GREEN << "================ END OF SEED ID: " << id << "================" << CRESET << std::endl;
    return os;
}

std::ostream& operator<< (std::ostream& os, const Seed& seed) {
    return seed.dump(os);
}