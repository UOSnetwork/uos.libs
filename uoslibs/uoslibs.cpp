


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
//#include <chainbase/chainbase.hpp>
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

    namespace in_progress{




        void table_worker1(){
            using namespace std;
            eosio::chain::controller &cc = eosio::app().get_plugin<eosio::chain_plugin>().chain();

            uint64_t code = N(calctest1111);
            uint64_t table = N(reports);
            uint64_t scope = code;
            const eosio::chain::account_object *code_accnt = cc.db().find<eosio::chain::account_object, eosio::chain::by_name>(code);
            if(code_accnt== nullptr){
                elog("calctest1111 not found");
                return;
            }
            eosio::abi_def abi;
            eosio::abi_serializer abis;

            abi = code_accnt->get_abi();
            abis.set_abi(abi,fc::microseconds(1000));

//-----
            for(auto item : abi.tables){
                cout<<"+++"<<endl;
                cout<<item.name.to_string()<<endl;
                for(auto i : item.key_names){
                    cout<<"-"<<i<<endl;
                }
                for(auto i : item.key_types){
                    cout<<"="<<i<<endl;
                }
                cout<<item.index_type<<endl;
                cout<<item.type<<endl;
            }

            cout<<fc::json::to_string(abi)<<endl;
//-------

            uint64_t table_with_index = table & 0xFFFFFFFFFFFFFFF0ULL;
            table_with_index |= (0&0x000000000000000FULL); //secondary index
            uint64_t table_with_index2 = table & 0xFFFFFFFFFFFFFFF0ULL;
            table_with_index2 |= (1&0x000000000000000FULL); //third index

//-------
            cout<<"||"<<eosio::name(table_with_index).to_string()<<endl;
//-------



            const auto* t_id = cc.db().find<eosio::chain::table_id_object, eosio::chain::by_code_scope_table>(boost::make_tuple(code, scope, table));
            const auto* index_t_id = cc.db().find<eosio::chain::table_id_object, eosio::chain::by_code_scope_table>(boost::make_tuple(code, scope, table_with_index));
            const auto* index_t_id2 = cc.db().find<eosio::chain::table_id_object, eosio::chain::by_code_scope_table>(boost::make_tuple(code, scope, table_with_index2));

            if(t_id!= nullptr)
                cout<<"Table found!"<<endl;

            if (index_t_id != nullptr)
                cout<<"Index found!"<<endl;
            if (index_t_id2 != nullptr)
                cout<<"Index2 found!"<<endl;

//            using  conv = eosio::chain_apis::keytype_converter<eosio::chain_apis::sha256,eosio::chain_apis::hex>;

            if (t_id != nullptr && index_t_id2 != nullptr) {
                const auto &secidx = cc.db().get_index<eosio::chain::index256_index, eosio::chain::by_secondary>();
                const chainbase::generic_index<eosio::chain::index256_index>& ssidx = cc.db().get_index<eosio::chain::index256_index>();
                auto *iii = &ssidx;


                for(auto rrrr = secidx.begin(); rrrr!=secidx.end();++rrrr){
                    cout<<"---"<<rrrr->primary_key<<endl;
                }
                cout<<"~~~~"<<index_t_id2->id._id<<endl;


                decltype(auto) low_tid(index_t_id2->id._id);
                decltype(auto) next_tid(index_t_id2->id._id + 1);
                auto lower = secidx.lower_bound(boost::make_tuple(low_tid));
                auto upper = secidx.lower_bound(boost::make_tuple(next_tid));

                for (auto itr = lower; itr != upper; ++itr) {

                    const auto* itr2 = cc.db().find<eosio::chain::key_value_object, eosio::chain::by_scope_primary>(boost::make_tuple(t_id->id, itr->primary_key));
                    if (itr2 == nullptr) {
                        cout<<"nullptr"<<endl;
                        continue;
                    }
                    vector<char> data;
                    eosio::chain_apis::read_only::copy_inline_row(*itr2,data);

                    auto ret = abis.binary_to_variant(abis.get_table_type(table),data,fc::microseconds(1000));
                    cout<<ret.get_object().find("block_num")->value().as_int64()<<endl;
                    cout<<"+++"<<endl;
                    cout<<fc::json::to_string(ret)<<endl;
                }

            }
        }
    }

}

