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

#include "lumeos.hpp"

#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

#include <vector>

namespace {

auto getUserItr(eosio::name const accountName,
                lumeos::Lumeos::userIndex const& users) {
    auto result = users.find(accountName.value);
    eosio_assert(
        result != users.end(),
        std::string("User not found: " + accountName.to_string()).c_str());

    return result;
}

}  // namespace

void lumeos::Lumeos::createuser(eosio::name const accountName,
                                std::string const& ipfsHash) {
    require_auth(_self);

    userIndex users(_self, _self.value);
    eosio_assert(users.find(accountName.value) == users.end(),
                 "User already exists.");

    users.emplace(_self, [&](auto& user) {
        user.m_accountName = accountName;
        user.m_ipfsHash = ipfsHash;
    });

    eosio::print(std::string("created: " + accountName.to_string()).c_str());
}

void lumeos::Lumeos::updateuser(eosio::name const accountName,
                                std::string const& ipfsHash) {
    require_auth(_self);

    userIndex users(_self, _self.value);
    auto userItr = getUserItr(accountName, users);
    users.modify(userItr, _self, [&](auto& user) {
        user.m_accountName = accountName;
        user.m_ipfsHash = ipfsHash;
    });
}

void lumeos::Lumeos::removeuser(eosio::name const accountName) {
    require_auth(_self);

    userIndex users(_self, _self.value);
    auto accountItr = users.find(accountName.value);
    eosio_assert(accountItr != users.end(), "User not found.");
    users.erase(accountItr);
}

void lumeos::Lumeos::validateUser(eosio::name const accountName) {
    userIndex users(_self, _self.value);
    eosio_assert(
        users.find(accountName.value) != users.end(),
        std::string("User not found: " + accountName.to_string()).c_str());
}

void lumeos::Lumeos::createpoll(uint64_t pollId, eosio::asset price,
                                std::string const& ipfsHash) {
    require_auth(_self);

    eosio_assert(price.is_valid(), "invalid price");
    eosio_assert(price.amount > 0, "must deposit positive quantity");

    pollIndex polls(_self, _self.value);
    eosio_assert(polls.find(pollId) == polls.end(), "Poll id already exists.");

    polls.emplace(_self, [&](auto& poll) {
        poll.m_pollId = pollId;
        poll.m_price = price;
        poll.m_ipfsHash = ipfsHash;
    });
}

// 'updatepoll' will be called very often on every poll answer.
// investigate if there is any savings on overloading without price update
void lumeos::Lumeos::updatepoll(uint64_t pollId, eosio::asset price,
                                std::string const& ipfsHash) {
    require_auth(_self);

    eosio_assert(price.is_valid(), "invalid price");
    eosio_assert(price.amount > 0, "must deposit positive quantity");

    pollIndex polls(_self, _self.value);
    auto pollItr = polls.find(pollId);
    eosio_assert(pollItr != polls.end(), "Poll not found.");

    polls.modify(pollItr, _self, [&](auto& poll) {
        poll.m_pollId = pollId, poll.m_price = price;
        poll.m_ipfsHash = ipfsHash;
    });
}

void lumeos::Lumeos::removepoll(uint64_t pollId) {
    require_auth(_self);
    pollIndex polls(_self, _self.value);

    auto pollItr = polls.find(pollId);
    eosio_assert(pollItr != polls.end(), "Poll not found.");

    polls.erase(pollItr);
}

void lumeos::Lumeos::buy(eosio::name const& buyer, uint64_t pollId) {
    require_auth(buyer);
    validateUser(buyer);  // just as safety that person is in the system

    pollIndex polls(_self, _self.value);
    auto pollItr = polls.find(pollId);
    eosio_assert(pollItr != polls.end(), "Poll not found.");

    eosio::asset pollPrice = pollItr->m_price;
    eosio_assert(pollPrice.is_valid(), "invalid price");
    eosio_assert(pollPrice.amount > 0, "negative price");
    std::string const memo = "b:" + buyer.to_string() +
                             ";pid:" + std::to_string(pollId) +
                             ";p:" + std::to_string(pollPrice.amount);
    eosio::action(eosio::permission_level{buyer, "active"_n}, "lumeostokens"_n,
                  "transfer"_n,
                  make_tuple(buyer, "lumeoslumeos"_n, pollPrice, memo))
        .send();
}
