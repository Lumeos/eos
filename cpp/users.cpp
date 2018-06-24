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

void lumeos::Users::createuser(
    eosio::name const accountName, std::string const& name,
    std::string const& email,
    std::string const& addressStr,  // "street:city:country:postal_code"
    uint32_t dateOfBirth) {
    require_auth(accountName);

    userIndex users(_self, _self);
    eosio_assert(users.find(accountName) == users.end(),
                 "User already exists.");
    users.available_primary_key();

    users.emplace(accountName, [&](auto& user) {
        user.m_accountName = accountName;
        user.m_name = name;
        user.m_email = email;
        user.m_dateOfBirth = dateOfBirth;
        user.m_address = address(addressStr);
    });
}

void lumeos::Users::removeuser(eosio::name const accountName,
                               std::string const& feedback) {
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

void lumeos::Users::setemail(eosio::name const accountName,
                             std::string const& email) {
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

void lumeos::Users::setname(eosio::name const accountName,
                            std::string const& name) {
    require_auth(accountName);

    userIndex users(_self, _self);

    auto accountItr = users.find(accountName);
    eosio_assert(
        accountItr != users.end(),
        std::string("User not found: " + accountName.to_string()).c_str());

    users.modify(accountItr, accountName,
                 [&](auto& user) { user.m_name = name; });
}

void lumeos::Users::setdob(eosio::name const accountName, uint32_t YYYYMMDD) {
    require_auth(accountName);

    userIndex users(_self, _self);

    auto accountItr = users.find(accountName);
    eosio_assert(
        accountItr != users.end(),
        std::string("User not found: " + accountName.to_string()).c_str());

    users.modify(accountItr, accountName,
                 [&](auto& user) { user.m_dateOfBirth = YYYYMMDD; });
}

void lumeos::Users::getuser(eosio::name const accountName) {
    userIndex users(_self, _self);

    eosio_assert(users.find(accountName) != users.end(), "User not found");
    auto const& currentUser = users.get(accountName);
    eosio::print(static_cast<std::string>(currentUser).c_str());
}

void lumeos::Users::updateflist(eosio::name const firstAccountName,
                                eosio::name const secondAccountName,
                                bool becomingFriends) {
    eosio_assert(firstAccountName != secondAccountName, "Cannot self friend");
    require_auth(_self);

    if (becomingFriends) {
        makeFriends(firstAccountName, secondAccountName);
    } else {
        unfriend(firstAccountName, secondAccountName);
    }
}

void lumeos::Users::validateUser(eosio::name const accountName) {
    userIndex users(_self, _self);
    eosio_assert(
        users.find(accountName) != users.end(),
        std::string("User not found: " + accountName.to_string()).c_str());
}

void lumeos::Users::makeFriends(eosio::name const firstAccountName,
                                eosio::name const secondAccountName) {
    userIndex users(_self, _self);

    auto firstAccountItr = getUserItr(firstAccountName, users);
    auto secondAccountItr = getUserItr(secondAccountName, users);

    users.modify(firstAccountItr, _self, [&](auto& user) {
        user.m_friends.emplace_back(secondAccountName);
    });
    users.modify(secondAccountItr, _self, [&](auto& user) {
        user.m_friends.emplace_back(firstAccountName);
    });
}

void lumeos::Users::unfriend(eosio::name const firstAccountName,
                             eosio::name const secondAccountName) {
    userIndex users(_self, _self);

    auto firstAccountItr = getUserItr(firstAccountName, users);
    auto secondAccountItr = getUserItr(secondAccountName, users);

    users.modify(firstAccountItr, _self, [&](auto& user) {
        auto& friends = user.m_friends;
        friends.erase(
            std::remove(friends.begin(), friends.end(), secondAccountName),
            friends.end());
    });
    users.modify(secondAccountItr, _self, [&](auto& user) {
        auto& friends = user.m_friends;
        friends.erase(
            std::remove(friends.begin(), friends.end(), firstAccountName),
            friends.end());
    });
}

void lumeos::Users::createpoll(eosio::name const& accountName,
                               std::string const& question,
                               std::vector<std::string> const& answers,
                               std::vector<std::string> const& tags) {
    require_auth(accountName);
    validateUser(accountName);

    pollIndex polls(_self, _self);

    polls.emplace(_self, [&](auto& poll) {
        poll.m_pollId = polls.available_primary_key();
        poll.m_question = question;
        std::transform(answers.cbegin(), answers.cend(),
                       std::back_inserter(poll.m_choices),
                       [](std::string const& val) { return PollChoice(val); });
        poll.m_tags = tags;
        poll.m_creator = accountName;
    });
}

void lumeos::Users::removepoll(eosio::name const& accountName,
                               uint64_t pollId) {
    require_auth(accountName);
    validateUser(accountName);
    pollIndex polls(_self, _self);

    auto pollItr = polls.find(pollId);
    eosio_assert(pollItr != polls.end(), "Poll not found.");

    // user can only delete poll if he created it and no one else answered yet.
    eosio_assert(pollItr->m_creator == accountName,
                 "You need to be creator to delete poll.");
    eosio_assert(pollItr->m_participants.empty(),
                 "You cannot delete poll that has been answered already.");

    polls.erase(pollItr);
}

void lumeos::Users::answerpoll(eosio::name const& accountName, uint64_t pollId,
                               uint8_t answerIndex) {
    require_auth(accountName);
    validateUser(accountName);

    pollIndex polls(_self, _self);

    auto pollItr = polls.find(pollId);
    eosio_assert(pollItr != polls.end(), "Poll not found.");

    // creator can answer own poll
    eosio_assert(std::find(pollItr->m_participants.cbegin(),
                           pollItr->m_participants.cend(),
                           accountName) == pollItr->m_participants.end(),
                 "User already answered this poll.");
    eosio_assert(pollItr->m_choices.size() > answerIndex,
                 "Index is larger than number of choices");
    polls.modify(pollItr, _self, [&](auto& poll) {
        poll.m_participants.push_back(accountName);
        poll.m_choices[answerIndex].m_count += 1;
    });
}
