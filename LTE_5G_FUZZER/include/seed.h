#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "patch.h"

#include <nlohmann/json.hpp>

class Seed {
public:

    Seed() : id(total_seeds++) {}
    Seed(std::vector<std::shared_ptr<Patch>> patches) : id(total_seeds++), patches(patches) {}
    Seed(const Seed& seed) : edges_covered(seed.edges_covered), id(seed.id), patches(seed.patches) {}

public:
    inline void add_patch(std::shared_ptr<Patch> patch) {
        patches.push_back(std::move(patch));
    }

    inline std::vector<std::shared_ptr<Patch>> get_patches() {
        return patches;
    }

    inline const std::vector<std::shared_ptr<Patch>> get_patches() const {
        return patches;
    }

    inline const int get_patches_size() const {
        return patches.size();
    }

    inline const uint32_t get_id() const {
        return id;
    }

    inline const bool is_empty() const {
        return patches.empty();
    }

    std::ostream& dump(std::ostream&) const;

    uint32_t edges_covered = 0;

private:

    static uint64_t total_seeds;
    uint64_t id = 0;
    std::vector<std::shared_ptr<Patch>> patches;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Seed, id, patches, edges_covered)

};

std::ostream& operator<< (std::ostream& os, const Seed& seed);

