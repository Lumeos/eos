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

#ifndef LUMEOS_ADDRESS_H
#define LUMEOS_ADDRESS_H

#include <eosiolib/eosio.hpp>

#include <boost/algorithm/string.hpp>

#include <string>
#include <vector>

namespace lumeos {

struct address {
    std::string m_street;
    std::string m_city;
    std::string m_country;
    uint32_t m_postalCode;

    address() : m_street(), m_city(), m_country(), m_postalCode(0) {}

    address(std::string const& addressStr)
        : m_street(), m_city(), m_country(), m_postalCode(0) {
        std::vector<std::string> tokens;
        boost::split(tokens, addressStr, boost::is_any_of(":"));
        eosio_assert(
            tokens.size() == 4,
            std::string("Invalid address format specified: " + addressStr)
                .c_str());
        m_street = tokens[0];
        m_city = tokens[1];
        m_country = tokens[2];
        m_postalCode = std::stoul(tokens[3]);
    }

    operator std::string() const {
        return m_street + " " + m_city + " " + m_country + " " +
               std::to_string(m_postalCode);
    }

    EOSLIB_SERIALIZE(address, (m_street)(m_city)(m_country)(m_postalCode))
};
}  // namespace lumeos

#endif  // LUMEOS_ADDRESS_H
