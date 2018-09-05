
#pragma once

#include <vector>
#include <eosio/chain_plugin/chain_plugin.hpp>

namespace uoslibs{

    namespace blockworker {

        struct get_transactions_from_block_params{
        public:
            std::string contract;
            std::vector<std::string> action_names;
        };

        struct get_transactions_from_block_result{
        public:
            std::vector<fc::mutable_variant_object> actions;
        };

        typedef std::set<eosio::action_name> action_set_t;

        typedef std::map<uint32_t , get_transactions_from_block_result> transactions_by_blocks;

        get_transactions_from_block_result parse_block(eosio::account_name &, action_set_t &, eosio::chain::signed_block_ptr&);

        get_transactions_from_block_result get_transactions_from_block(const get_transactions_from_block_params&, eosio::chain::signed_block_ptr&);
        get_transactions_from_block_result get_transactions_from_block(const get_transactions_from_block_params&, uint32_t);
        get_transactions_from_block_result get_transactions_from_block(const get_transactions_from_block_params&, std::string);

        transactions_by_blocks get_transactions_from_blockrange(const get_transactions_from_block_params&, uint32_t , uint32_t);

    }

}