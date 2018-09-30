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

#include "users.hpp"

#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

#include <vector>

namespace {

auto getUserItr(eosio::name const accountName,
                lumeos::Users::userIndex const& users) {
    auto result = users.find(accountName);
    eosio_assert(
        result != users.end(),
        std::string("User not found: " + accountName.to_string()).c_str());

    return result;
}

}  // namespace

void lumeos::Users::createuser(eosio::name const accountName, uint32_t userId,
                               std::string const& ipfsHash) {
    require_auth(_self);

    // TODO: Check userId existance, someone might want to override
    // TODO: validate hash format

    userIndex users(_self, _self);
    eosio_assert(users.find(accountName) == users.end(),
                 "User already exists.");

    users.emplace(_self, [&](auto& user) {
        user.m_accountName = accountName;
        user.m_userId = userId;
        user.m_ipfsHash = ipfsHash;
    });

    eosio::print(std::string("created: " + accountName.to_string()).c_str());
}

void lumeos::Users::updateuser(eosio::name const accountName, uint32_t userId,
                               std::string const& ipfsHash) {
    require_auth(_self);

    // TODO: Check userId existance, someone might want to override
    // TODO: validate hash format

    userIndex users(_self, _self);
    auto userItr = getUserItr(accountName, users);
    users.modify(userItr, _self, [&](auto& user) {
        user.m_accountName = accountName;
        user.m_userId = userId;
        user.m_ipfsHash = ipfsHash;
    });

    eosio::print(std::string("updated: " + accountName.to_string()).c_str());
}

void lumeos::Users::removeuser(eosio::name const accountName) {
    require_auth(_self);

    userIndex users(_self, _self);
    auto accountItr = users.find(accountName);
    eosio_assert(accountItr != users.end(), "User not found.");
    users.erase(accountItr);

    eosio::print(std::string("erased: " + accountName.to_string()).c_str());
}

void lumeos::Users::validateUser(eosio::name const accountName) {
    userIndex users(_self, _self);
    eosio_assert(
        users.find(accountName) != users.end(),
        std::string("User not found: " + accountName.to_string()).c_str());
}

void lumeos::Users::createpoll(uint32_t pollId, eosio::asset price,
                               std::string const& ipfsHash) {
    require_auth(_self);

    eosio_assert(price.is_valid(), "invalid price");
    eosio_assert(price.amount > 0, "must deposit positive quantity");

    pollIndex polls(_self, _self);
    // TODO: Check if poll already exists

    polls.emplace(_self, [&](auto& poll) {
        poll.m_pollId = pollId, poll.m_price = price;
        poll.m_ipfsHash = ipfsHash;
    });
}

// 'updatepoll' will be called very often on every poll answer.
// investigate if there is any savings on overloading without price update
void lumeos::Users::updatepoll(uint32_t pollId, eosio::asset price,
                               std::string const& ipfsHash) {
    require_auth(_self);

    eosio_assert(price.is_valid(), "invalid price");
    eosio_assert(price.amount > 0, "must deposit positive quantity");

    pollIndex polls(_self, _self);
    auto pollItr = polls.find(pollId);
    eosio_assert(pollItr != polls.end(), "Poll not found.");

    polls.modify(pollItr, _self, [&](auto& poll) {
        poll.m_pollId = pollId, poll.m_price = price;
        poll.m_ipfsHash = ipfsHash;
    });
}

void lumeos::Users::removepoll(eosio::name const& accountName,
                               uint32_t pollId) {
    require_auth(_self);
    validateUser(accountName);
    pollIndex polls(_self, _self);

    auto pollItr = polls.find(pollId);
    eosio_assert(pollItr != polls.end(), "Poll not found.");

    polls.erase(pollItr);
}

void lumeos::Users::buy(eosio::name const& buyer, uint32_t pollId) {
    eosio::print("legit here");
    require_auth(buyer);
    validateUser(buyer);  // just as safety that person is in the system

    pollIndex polls(_self, _self);
    auto pollItr = polls.find(pollId);
    eosio_assert(pollItr != polls.end(), "Poll not found.");

    eosio::asset pollPrice = pollItr->m_price;
    eosio_assert(pollPrice.is_valid(), "invalid price");
    eosio_assert(pollPrice.amount > 0, "must deposit positive quantity");
    eosio::action(eosio::permission_level{buyer, N(active)}, N(lumeos.token),
                  N(transfer),
                  make_tuple(buyer, N(lumeosbank), pollPrice, std::string("")))
        .send();
}
