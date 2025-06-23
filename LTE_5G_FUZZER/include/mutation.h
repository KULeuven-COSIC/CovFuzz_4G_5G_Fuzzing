#pragma once

#include "field.h"
#include "mutator.h"
#include "packet.h"

#include <iostream>
#include <memory>

#include <nlohmann/json.hpp>

/**
 * Class responsible for mutating the value of the field
 */
class Mutation {
public:
        Mutation() = default;
        Mutation(mutator mut, Field f);
        Mutation(const Mutation& mutation);

        bool operator==(const Mutation& mut) const;
        bool operator!=(const Mutation& mut) const;

public:
        struct prepared_mutation {
                uint64_t new_value;
                parsed_field f;
        };

        bool prepare(const Packet& packet);
        std::vector<prepared_mutation> prepared_mutation_vector;

public:
        void apply(Packet *packet);
        uint64_t get_old_value() const;
        uint64_t get_new_value() const;
        mutator get_mutator() const;

public:
        std::ostream& dump(std::ostream&) const;
        friend void to_json(nlohmann::json& j, const Mutation& mut);
        friend void from_json(const nlohmann::json& j, Mutation& mut);

public:
        static uint64_t total_mutations;
        uint64_t id = 0;
        Field field;
        mutator mutator_;
        std::string mutator_name;
        uint64_t old_value;
        uint64_t new_value;
        bool is_completed;
        bool failed = false;
};

std::ostream& operator<<(std::ostream& os, const Mutation& mutation);
std::ostream& operator<<(std::ostream& os, const std::shared_ptr<Mutation>& mutation);

void to_json(nlohmann::json& j, const Mutation& mut);
void from_json(const nlohmann::json& j, Mutation& mut);

std::vector<std::shared_ptr<Mutation>> create_mutations(std::vector<Field>& fields);