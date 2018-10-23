
#pragma once

#include <vector>


#include <chainbase/chainbase.hpp>
#include <eosio/chain/contract_table_objects.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <int_t.h>


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

    } //end blockworker

    namespace tools{
        std::vector<eosio::chain::account_object> get_all_accounts();
    } //end tools

    namespace in_progress {
//todo: check conversion

        inline std::string
        reverse_bytes(std::string __val){
            if(__val.length()==0)return "";
            if(__val.length()==2)return __val;
            if(__val.length()%2>0){
                __val="0"+__val;
            }
            uint64_t steps = __val.length()/2;
            std::cout<<steps<<std::endl;
            std::string ret;
            try{
                for(auto i=0;i<steps;i++){
                    ret.push_back(__val[(steps-i-1)*2]);
                    ret.push_back(__val[(steps-i-1)*2+1]);
                }
            }catch (...){
                std::cout<<"error in reverse"<<std::endl;
            }
            return ret;
        }
//// to hex
        inline std::string
        to_string_hex(uint32_t __val){
            ilog("to str hex uint32_t");
            return __gnu_cxx::__to_xstring<std::string>(&std::vsnprintf,4 * sizeof(uint32_t),"%08lX", __val);
        }
        inline std::string
        to_string_hex(uint64_t __val){
            ilog("to str hex uint64_t");
            std::cout<<std::hex<<__val<<std::endl;
            return __gnu_cxx::__to_xstring<std::string>(&std::vsnprintf,4 * sizeof(uint64_t),"%016llX", __val);
        }
        inline std::string
        to_string_hex(unsigned __int128 __val){
            ilog("to str hex uint128");
            return to_string_hex(uint64_t(__val>>64))+to_string_hex(uint64_t(__val));
        }

        inline std::string
        to_string_hex(boost::multiprecision::uint128_t __val){
            ilog("to str hex bm128");
            std::stringstream ss;
            ss<<std::setfill('0')<<std::setw(32)<<std::hex<<std::uppercase<<__val;

            return ss.str();
        }
        inline std::string
        to_string_hex(boost::multiprecision::uint256_t __val){
            ilog("to str hex bm256");
            std::stringstream ss;
            ss<<std::setfill('0')<<std::setw(64)<<std::hex<<std::uppercase<<__val;
            return ss.str();
        }
        inline std::string
        to_string_hex(eosio::chain::key256_t __val){
            ilog("to str hex key256_t");
            return fc::to_hex((char*) __val._M_elems, sizeof(__val._M_elems));

//            return to_string_hex(__val[0])+to_string_hex(__val[1]);
        }

        inline std::string
        to_string_hex(fc::sha256 __val){
            ilog("to str hex fc::sha256");
            return std::string(__val);
        }

//// from hex
        template <class boost_mp_int>
        inline boost_mp_int
        _from_string_hex(const std::string __val){
            ilog("from str hex boost_mp_int");
            boost_mp_int ret=0;
            std::istringstream conv(__val);
            conv>>std::hex>>ret;
            return ret;
        }
        template<>
        inline unsigned __int128 _from_string_hex<unsigned __int128>(std::string __val) {
            uint64_t arr[2];
            auto size = __val.size();
            if(size > 16 && size <= 32 ) {
                std::string tmp[2];
                tmp[0] = __val.substr(0,16);
                tmp[1] = __val.substr(16);
                arr[0] = _from_string_hex<uint64_t>(tmp[0]);
                arr[1] = _from_string_hex<uint64_t>(tmp[1]);
                unsigned __int128 ret = arr[0];
                ret = ret << 64;
                ret = ret | (unsigned __int128) arr[1];
                return ret;
            } else if (size<=16){
                arr[1] = 0;
                arr[0] =_from_string_hex<uint64_t>(__val);
                return arr[0];
            }
            return 0;
        }
        template <>
        inline eosio::chain::key256_t _from_string_hex<eosio::chain::key256_t>(std::string __val){
            ilog("from string hex key256_t");
            eosio::chain::key256_t ret;
//            unsigned __int128 res[2];
            fc::from_hex(__val,(char*)ret._M_elems , sizeof(ret._M_elems) );
            return ret;
//
//
//            auto size = __val.size();
//            ret[0]=ret[1]=0;
//            if( size >32 && size <= 64 ){
//                std::string tmp[2];
//                tmp[0] = __val.substr(0,32);
//                tmp[1] = __val.substr(32);
//                ret[0]= _from_string_hex<unsigned __int128>(tmp[0]);
//                ret[1]= _from_string_hex<unsigned __int128>(tmp[1]);
//                return ret;
//            } else if (size<=32){
//                ret[1] = 0;
//                ret[0] = _from_string_hex<unsigned __int128>(__val);
//                return ret;
//            }
//            return ret;
        }

        template <>
        inline fc::sha256 _from_string_hex<fc::sha256>(std::string __val) {
            return fc::sha256(__val);
        }


        inline auto from_string_hex_mpuint128(std::string __val){
            return _from_string_hex<boost::multiprecision::uint128_t>(__val);
        }

        inline auto from_string_hex_mpuint256(std::string __val){
            return _from_string_hex<boost::multiprecision::uint256_t>(__val);
        }

        inline eosio::chain::key256_t from_string_hex_key256(std::string __val){
            return _from_string_hex<eosio::chain::key256_t>(__val);
        }

        template<const char*key_type , const char *encoding=eosio::chain_apis::dec>
        class table_worker {
        private:


            typedef typename eosio::chain_apis::keytype_converter<key_type,encoding>::input_type _input_type;
            typedef typename eosio::chain_apis::keytype_converter<key_type,encoding>::index_type _index_type;
            typedef typename eosio::chain_apis::keytype_converter<key_type,encoding> _key_conv;
            using __index = _index_type;//decltype( (chainbase::generic_index<_index_type>)->indices());
            eosio::chain::name _code;
            eosio::chain::name _scope;
            eosio::chain::name _table;


            class container{
            public:
                const __index &secidx;

                chainbase::oid<eosio::chain::table_id_object> table_id;
                chainbase::oid<eosio::chain::table_id_object> index_table_id;

                container():
                    secidx(eosio::app().get_plugin<eosio::chain_plugin>().chain().db().get_index<_index_type>().indices())
                {ilog("Container created");};
                ~container(){ilog("container deleted");}

            };

            container               *_container;
            eosio::abi_def          _abi;
            eosio::abi_serializer   _abis;
            bool                    table_is_loaded =   false;

        public:

            table_worker(eosio::chain::name code=0, eosio::chain::name scope=0, eosio::chain::name table=0, uint64_t index_number=0);
            ~table_worker(){
                if(_container!= nullptr){
                    delete _container;
                }
                ilog("TableWorker deleted");
            }

            auto begin(){
                ilog("begin");
                return _container->secidx.begin();
            }
            auto end(){
                ilog("end");
                return _container->secidx.end();
            }


            fc::variant operator[](std::string secondary_key){
                fc::variant ret;
                _input_type idx = _from_string_hex<_input_type >(secondary_key);
                auto _key = _key_conv::function()( idx );
                std::cout<<secondary_key<<std::endl;
                std::cout<<to_string_hex(idx)<<std::endl;
                std::cout<<to_string_hex(_key)<<std::endl;

                const auto &secidx_by_secondary = _container->secidx.template get<eosio::chain::by_secondary>();

                auto itr = secidx_by_secondary.lower_bound(
                        boost::make_tuple(_container->index_table_id,_key)
                        );

                elog(to_string_hex(itr->secondary_key));
                elog(to_string_hex(_key));

                if(itr->secondary_key == _key){
                    elog("key==key");
                    const auto & key_value_idx = eosio::app().get_plugin<eosio::chain_plugin>().chain().db().get_index<eosio::chain::key_value_index,eosio::chain::by_scope_primary>();
                    auto itr2 = key_value_idx.find(boost::make_tuple(_container->table_id,itr->primary_key));
                    if(itr2 != key_value_idx.end()) {
                        elog("key != end");
                        std::vector<char> data;
                        eosio::chain_apis::read_only::copy_inline_row(*itr2,data);
                        ret = _abis.binary_to_variant(_abis.get_table_type(_table),data,fc::microseconds(1000));
                    }
                    else{
                        elog("key==end");
                    }
                }
                else{
                    elog("seckey != key");
                }

                return ret;
            }

            ////non-copyable class
            table_worker( const table_worker& ) = delete;
            void operator= (const table_worker& ) = delete;



//            template <typename CompatibleKey>
//            auto *find(CompatibleKey &&key){ if(secidx!= nullptr) return secidx->find(key); return nullptr;}


        };


        template<const char*key_type , const char *encoding>
        table_worker<key_type,encoding>::table_worker(eosio::chain::name code, eosio::chain::name scope,
                                                      eosio::chain::name table, uint64_t index_number ) {
            _code           = code;
            _scope          = scope;
            _table          = table;
            _container      = nullptr;
            table_is_loaded = false;

//// check table in abi
            const eosio::chain::account_object *code_accnt = eosio::app().get_plugin<eosio::chain_plugin>().chain().db().find<eosio::chain::account_object, eosio::chain::by_name>(_code);
            if(code_accnt== nullptr){
                elog("Account "+_code.to_string()+"not found");
                table_is_loaded = false;
                return;
            }
            _abi = code_accnt->get_abi();
            _abis.set_abi(_abi,fc::microseconds(1000));
            for(auto t_item: _abi.tables){
                if(t_item.name==_table){
                    table_is_loaded = true;
                    ilog(t_item.name.to_string());
                }
            }

            if(!table_is_loaded) {
                elog("Table not found in abi");
                return;
            }
//// try to load index

            if(index_number == 1) {
                table_is_loaded = false; //only additional indexes!
                elog("TableWorker works only with secondary indexes");
                return;
            }
            uint64_t uint_table = uint64_t (_table);
            index_number = index_number - 2;
            uint64_t table_with_index = uint_table & 0xFFFFFFFFFFFFFFF0ULL;
            table_with_index |= (index_number & 0x000000000000000FULL);
            const auto* t_id = eosio::app().get_plugin<eosio::chain_plugin>().chain().db().find<eosio::chain::table_id_object, eosio::chain::by_code_scope_table>(boost::make_tuple(_code, _scope, _table));
            const auto* index_t_id = eosio::app().get_plugin<eosio::chain_plugin>().chain().db().find<eosio::chain::table_id_object, eosio::chain::by_code_scope_table>(boost::make_tuple(_code, _scope, table_with_index));

            if (index_t_id == nullptr){
                table_is_loaded = false;//index in table not found
                elog("Index is not found");
                return;
            }

            _container = new container();
            _container->table_id = t_id->id;
            _container->index_table_id = index_t_id->id;



            ilog("TableWorker created");

        }
    }////end in progress

}
