


#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>


#include <uoslibs/test.hpp>
#include <uoslibs/uoslibs.hpp>

#include <eosio/chain_api_plugin/chain_api_plugin.hpp>
#include <eosio/chain/transaction.hpp>
#include <eosio/chain/fork_database.hpp>
#include <fc/io/json.hpp>

#include <iostream>

namespace uoslibs {


    namespace uostest {
        std::ostream &operator<<(std::ostream &out, uoslibs::uostest::for_hello item) {
            for (auto i = 0; i < item.k; i++) {
                out << "Hello!" << std::endl;
            }
            return out;
        }

        using std::cout;
        using std::endl;

        void hello() {
            cout << "hello" << endl;
            for_hello example;
            example.k = 100;
            cout << example << endl;
        }
    }

    namespace  blockworker{

//        struct get_transactions_from_block_params{
//        public:
//            std::string contract;
//            std::vector<std::string> action_names;
//        };
//
//        struct get_transactions_from_block_result{
//        public:
//            std::vector<fc::variant> actions;
//        };
//      -- implementation moved to hpp

        get_transactions_from_block_result parse_block(eosio::account_name &contract, action_set_t &actions_list, eosio::chain::signed_block_ptr &block){
            get_transactions_from_block_result result;
            auto ro_api = eosio::app().get_plugin<eosio::chain_plugin>().get_read_only_api();
            eosio::chain_apis::read_only::abi_bin_to_json_params bins;

            for (auto trs : block->transactions){
                auto trx_id = trs.trx.get<eosio::chain::packed_transaction>().get_transaction().id();
                for (auto action : trs.trx.get<eosio::chain::packed_transaction>().get_transaction().actions){
                    if(action.account == contract && (actions_list.find(action.name)!=actions_list.end())){
                        //parse action
                        bins.code = contract;
                        bins.action = action.name;
                        bins.binargs = action.data;
                        static fc::mutable_variant_object ret;
                        ret.set("block_number",block->block_num());
                        ret.set("trx_id", std::string(trx_id));
                        ret.set("contract",uint64_t (action.account));
                        ret.set("action",uint64_t (action.name));
                        ret.set("args",ro_api.abi_bin_to_json(bins).args);
                        result.actions.push_back(ret);
                    }
                    //todo:
                }
            }
            return result;
        }

        get_transactions_from_block_result get_transactions_from_block(const get_transactions_from_block_params& params, eosio::chain::signed_block_ptr &block){
//            elog("get_transactions_from_block ptr");
            eosio::account_name contract;
            std::set<eosio::action_name> actions_list;
            try {
                contract = params.contract;
                for(auto item: params.action_names){
                    actions_list.insert(item);
                }
            }
            catch (...){
                elog("Error in convertion params to names");
                return get_transactions_from_block_result();
            }

            return parse_block(contract, actions_list, block);
        }

        get_transactions_from_block_result get_transactions_from_block(const get_transactions_from_block_params& params, uint32_t block){

            auto last_irreversible_block = eosio::app().get_plugin<eosio::chain_plugin>().chain().last_irreversible_block_num();

            if(last_irreversible_block<block){
                elog("asked block number is larger than last irreversible");
                return get_transactions_from_block_result();
            }

            auto bptr = eosio::app().get_plugin<eosio::chain_plugin>().chain().fetch_block_by_number(block);

            // if this block has disappeared, get next block
            while(bptr== nullptr){
                block++;
                if(block >= last_irreversible_block)
                    return get_transactions_from_block_result();
                bptr = eosio::app().get_plugin<eosio::chain_plugin>().chain().fetch_block_by_number(block);
            }

            return get_transactions_from_block(params, bptr);
        }

        get_transactions_from_block_result get_transactions_from_block(const get_transactions_from_block_params& params, std::string str_block_id){

            fc::sha256 sha_block_id = fc::sha256(str_block_id);

            auto bptr = eosio::app().get_plugin<eosio::chain_plugin>().chain().fork_db().get_block(sha_block_id)->block;

            return get_transactions_from_block(params, bptr);
        }

        transactions_by_blocks get_transactions_from_blockrange(const get_transactions_from_block_params& params, uint32_t start_block_num, uint32_t finish_block_num){

            if(start_block_num>=finish_block_num){
                return transactions_by_blocks();
            }

            eosio::account_name contract;
            std::set<eosio::action_name> actions_list;

            try {
                contract = params.contract;
                for(auto item: params.action_names){
                    actions_list.insert(item);
                }
            }
            catch (...){
                elog("Error in convertion params to names");
                return transactions_by_blocks();
            }


            auto &cc = eosio::app().get_plugin<eosio::chain_plugin>().chain();

            transactions_by_blocks ret;
            if(finish_block_num>cc.last_irreversible_block_num())
                finish_block_num=cc.last_irreversible_block_num();

            for(auto i= start_block_num; i<=finish_block_num; i++){
                auto ptr = cc.fetch_block_by_number(i);
                if(ptr == nullptr)
                    continue;
                auto ret_tmp = parse_block(contract,actions_list,ptr);
                if(ret_tmp.actions.size()>0) {
                    ret[i] = ret_tmp;
                }

            }

            return ret;
        }

    }
    namespace tools{
        std::vector<eosio::chain::account_object> get_all_accounts(){
            eosio::chain::controller &cc = eosio::app().get_plugin<eosio::chain_plugin>().chain();

            auto *itr = cc.db().find<eosio::chain::account_object>();
            typedef typename chainbase::get_index_type<eosio::chain::account_object>::type index_type;
            const auto& idx = cc.db().get_index<index_type, eosio::chain::by_name>();
            std::vector<eosio::chain::account_object> ret;
            for(auto item : idx){
                ret.push_back(item);
            }
            return ret;
        }
    }

}
