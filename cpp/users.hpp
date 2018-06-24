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

#ifndef LUMEOS_USERS_H
#define LUMEOS_USERS_H

#include "poll.hpp"
#include "user.hpp"

#include <eosiolib/symbol.hpp>

namespace lumeos {

struct Users : public eosio::contract {
   public:
    using userIndex = eosio::multi_index<N(user), user>;
    using pollIndex = eosio::multi_index<N(poll), poll>;

    explicit Users(account_name self) : contract(self) {}

    // @abi action
    void createuser(
        eosio::name const accountName, std::string const& name,
        std::string const& email,
        std::string const& addressStr,  // "street:city:country:postal_code"
        uint32_t dateOfBirth);

    // @abi action
    void removeuser(eosio::name const accountName, std::string const& feedback);

    // @abi action
    void setemail(eosio::name const accountName, std::string const& email);

    // @abi action
    void setname(eosio::name const accountName, std::string const& name);

    // @abi action
    void setdob(eosio::name const accountName, uint32_t YYYYMMDD);

    // @abi action
    void getuser(eosio::name const accountName);

    // @abi action
    void updateflist(eosio::name const firstAccountName,
                     eosio::name const secondAccountName, bool becomingFriends);

    // @abi action
    void createpoll(eosio::name const& accountName, std::string const& question,
                    std::vector<std::string> const& answers,
                    std::vector<std::string> const& tags);

    // @abi action
    void removepoll(eosio::name const& accountName, uint64_t pollId);

    // @abi action
    void answerpoll(eosio::name const& accountName, uint64_t pollId,
                    uint8_t answerIndex);

   private:
    void validateUser(eosio::name const accountName);

    void makeFriends(eosio::name const firstAccountName,
                     eosio::name const secondAccountName);

    void unfriend(eosio::name const firstAccountName,
                  eosio::name const secondAccountName);

    eosio::symbol_type LUME =
        eosio::symbol_type(eosio::string_to_symbol(4, "LUME"));

};  // Users

EOSIO_ABI(Users, (createuser)(removeuser)(setemail)(setname)(setdob)(getuser)(
                     updateflist)(createpoll)(answerpoll)(removepoll))

}  // namespace lumeos

#endif  // LUMEOS_USERS_H
