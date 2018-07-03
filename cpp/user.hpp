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

#ifndef LUMEOS_USER_H
#define LUMEOS_USER_H

#include "address.hpp"

#include <eosiolib/eosio.hpp>

#include <string>
#include <vector>

namespace lumeos {

//@abi table user i64
struct user {
    eosio::name m_accountName;
    std::string m_name;
    std::string m_email;
    uint32_t m_dateOfBirth;  // YYYYMMDD, eos build is limitied, so no
                             // boost::date here
    lumeos::address m_address;
    std::vector<eosio::name> m_friends;
    uint32_t m_numCreated;

    user()
        : m_accountName(), m_name(), m_email(), m_dateOfBirth(0), m_address(), m_numCreated(0) {}

    uint64_t primary_key() const { return m_accountName; }

    operator std::string() const {
        // TODO: write something reasonable
        // stringstream fails in calloc during contract setup in wasm
        // boost::property_tree does not compile without rtti.
        // so this uglyness
        std::string result = "accountName: " + m_accountName.to_string() +
                             " "
                             "displayName: " +
                             m_name + " " + "email: " + m_email + " " +
                             "dob: " + std::to_string(m_dateOfBirth) +
                             " "
                             "address: " +
                             static_cast<std::string>(m_address) +
                             " #friends: " + std::to_string(m_friends.size()) +
                             " # poll created: " + std::to_string(m_numCreated);
        return result;
    }

    EOSLIB_SERIALIZE(user, (m_accountName)(m_name)(m_email)(m_dateOfBirth)(
                               m_address)(m_friends)(m_numCreated))
};

}  // namespace lumeos

#endif  // LUMEOS_USER_H
