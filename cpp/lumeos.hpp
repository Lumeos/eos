//
//  Copyright (c) 2018, Respective Authors all rights reserved.
//
//  The MIT License
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to
//  deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
//  sell copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.
//

#ifndef LUMEOS_LUMEOS_H
#define LUMEOS_LUMEOS_H

#include "poll.hpp"
#include "user.hpp"

#include <eosiolib/asset.hpp>
#include <eosiolib/symbol.hpp>

namespace lumeos {

CONTRACT Lumeos : public eosio::contract {
   public:
    using eosio::contract::contract;
    using userIndex = eosio::multi_index<"user"_n, user>;
    using pollIndex = eosio::multi_index<"poll"_n, poll>;

    ACTION createuser(eosio::name const accountName, std::string const& ipfsHash);

    ACTION updateuser(eosio::name const accountName, std::string const& ipfsHash);

    // @abi action
    void removeuser(eosio::name const accountName);

    // @abi action
    void createpoll(uint32_t pollId, eosio::asset price,
                    std::string const& ipfsHash);

    // @abi action
    void updatepoll(uint32_t pollId, eosio::asset price,
                    std::string const& ipfsHash);

    // @abi action
    void removepoll(eosio::name const& accountName, uint32_t pollId);

    // @abi action
    void buy(eosio::name const& buyer, uint32_t pollId);

   private:
    void validateUser(eosio::name const accountName);

};  // Lumeos

EOSIO_DISPATCH(Lumeos, (updatepoll)(createuser)(updateuser)(removeuser)(createpoll)(
                     removepoll)(buy))

}  // namespace lumeos

#endif  // LUMEOS_LUMEOS_H
