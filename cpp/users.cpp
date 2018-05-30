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

#include <users.hpp>

#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

namespace lumeos {

// TODO: Add validations etc. cants use most of the boost here

struct Users : public eosio::contract {
    using userIndex = eosio::multi_index<N(user), user>;

   public:
    Users(account_name self) : contract(self) {}

    //@abi action
    void create(
        eosio::name const accountName, std::string const& name,
        std::string const& email,
        std::string const& addressStr,  // "street:city:country:postal_code"
        uint32_t dateOfBirth) {
        require_auth(accountName);

        userIndex users(_self, _self);
        eosio_assert(users.find(accountName) == users.end(),
                     "User already exists.");

        users.emplace(accountName, [&](auto& user) {
            user.m_accountName = accountName;
            user.m_name = name;
            user.m_email = email;
            user.m_dateOfBirth = dateOfBirth;
            user.m_address = address(addressStr);
        });
    }

    //@abi action
    void remove(eosio::name const accountName, std::string const& feedback) {
        // feedback for us to know why they deleting their account
        require_auth(accountName);
        userIndex users(_self, _self);
        auto accountItr = users.find(accountName);
        eosio_assert(accountItr != users.end(), "User not found.");

        users.erase(accountItr);
        eosio::print(std::string("erased: " + accountName.to_string() +
                                 " with reason: " + feedback)
                         .c_str());
    }

    // @abi action
    void setemail(eosio::name const accountName, std::string const& email) {
        // email validation should be done on front end,
        // i am restricted what I can use here.
        // but maybe some basic checks to make sure?
        require_auth(accountName);

        userIndex users(_self, _self);

        auto accountItr = users.find(accountName);
        eosio_assert(accountItr != users.end(), "User not found.");

        users.modify(accountItr, accountName,
                     [&](auto& user) { user.m_email = email; });
    }

    // @abi action
    void setname(eosio::name const accountName, std::string const& name) {
        require_auth(accountName);

        userIndex users(_self, _self);

        auto accountItr = users.find(accountName);
        eosio_assert(
            accountItr != users.end(),
            std::string("User not found: " + accountName.to_string()).c_str());

        users.modify(accountItr, accountName,
                     [&](auto& user) { user.m_name = name; });
    }

    // @abi action
    void setdob(eosio::name const accountName, uint32_t YYYYMMDD) {
        require_auth(accountName);

        userIndex users(_self, _self);

        auto accountItr = users.find(accountName);
        eosio_assert(
            accountItr != users.end(),
            std::string("User not found: " + accountName.to_string()).c_str());

        users.modify(accountItr, accountName,
                     [&](auto& user) { user.m_dateOfBirth = YYYYMMDD; });
    }

    // @abi action
    void getuser(eosio::name const accountName) {
        userIndex users(_self, _self);

        eosio_assert(users.find(accountName) != users.end(), "User not found");
        auto const& currentUser = users.get(accountName);
        eosio::print(static_cast<std::string>(currentUser).c_str());
    }

    auto getUserItr(eosio::name const accountName, userIndex const& users) {
        auto result = users.find(accountName);
        eosio_assert(
            result != users.end(),
            std::string("User not found: " + accountName.to_string()).c_str());

        return result;
    }

    void validateUser(eosio::name const accountName) {
        userIndex users(_self, _self);
        eosio_assert(
            users.find(accountName) != users.end(),
            std::string("User not found: " + accountName.to_string()).c_str());
    }

    void makeFriends(eosio::name const firstAccountName,
                     eosio::name const secondAccountName) {
        userIndex users(_self, _self);

        auto firstAccountItr = getUserItr(firstAccountName, users);
        auto secondAccountItr = getUserItr(secondAccountName, users);

        users.modify(firstAccountItr, _self,
                     [&](auto& user) { user.m_friends.insert(secondAccountName); });
        users.modify(secondAccountItr, _self,
                     [&](auto& user) { user.m_friends.insert(firstAccountName); });
    }

    void unfriend(eosio::name const firstAccountName,
                  eosio::name const secondAccountName) {
        userIndex users(_self, _self);

        auto firstAccountItr = getUserItr(firstAccountName, users);
        auto secondAccountItr = getUserItr(secondAccountName, users);

        users.modify(firstAccountItr, _self,
                     [&](auto& user) { user.m_friends.erase(secondAccountName); });
        users.modify(secondAccountItr, _self,
                     [&](auto& user) { user.m_friends.erase(firstAccountName); });
    }

    // @abi action
    void updateflist(eosio::name const firstAccountName,
                      eosio::name const secondAccountName, bool becomingFriends) {
        eosio_assert( firstAccountName != secondAccountName, "Cannot self friend" );
        require_auth( _self );

        if (becomingFriends) {
            makeFriends(firstAccountName, secondAccountName);
        } else {
            unfriend(firstAccountName, secondAccountName);
        }
    }
};  // Users

EOSIO_ABI(Users, (create)(remove)(setemail)(setname)(setdob)(getuser)(updateflist))

}  // namespace lumeos
