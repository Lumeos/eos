cmake_minimum_required(VERSION 3.5)
project(lumeos VERSION 1.0.0)

find_package(eosio.cdt)

### Only generate the wasm
#add_executable( lumeos.wasm lumeos.cpp )

### Generate the wasm and abi
# as of this writing, eosio.cdt tooling generated empty abi contract.
add_contract( lumeos lumeos lumeos.cpp )
