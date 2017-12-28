// Copyright (c) 2017 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "config.h"
#include "dstencode.h"
#include "test/test_bitcoin.h"

#include <boost/test/unit_test.hpp>

namespace {

class DstCfgDummy : public DummyConfig {
public:
    DstCfgDummy() : useCashAddr(false) {}
    void SetCashAddrEncoding(bool b) override { useCashAddr = b; }
    bool UseCashAddrEncoding() const override { return useCashAddr; }

private:
    bool useCashAddr;
};

} // anon ns

BOOST_FIXTURE_TEST_SUITE(dstencode_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(test_addresses) {
    std::vector<uint8_t> hash = {0, 17, 128, 5, 246, 174, 201,
                                 130, 217, 236, 131, 136, 199,148,
                                 26, 202, 163, 58, 140, 221};

    const CTxDestination dstKey = CKeyID(uint160(hash));
    const CTxDestination dstScript = CScriptID(uint160(hash));

    std::string cashaddr_pubkey =
        "bitcoincash:qqqprqq976hvnqkeajpc33u5rt92xw5vm5mkhnvy5w";
    std::string cashaddr_script =
        "bitcoincash:pqqprqq976hvnqkeajpc33u5rt92xw5vm5vn2ut80n";
    std::string base58_pubkey = "CGUFXy9eQgs3eunVAEqFdS9tnkEcgLw9VD";
    std::string base58_script = "AFnEcRfCrnYfZk6439AfConxeDwu6kYGdb";

    const CChainParams &params = Params(CBaseChainParams::MAIN);
    DstCfgDummy cfg;

    // Check encoding
    cfg.SetCashAddrEncoding(true);
    BOOST_CHECK_EQUAL(cashaddr_pubkey, EncodeDestination(dstKey, params, cfg));
    BOOST_CHECK_EQUAL(cashaddr_script,
                      EncodeDestination(dstScript, params, cfg));
    cfg.SetCashAddrEncoding(false);
    BOOST_CHECK_EQUAL(base58_pubkey, EncodeDestination(dstKey, params, cfg));
    BOOST_CHECK_EQUAL(base58_script, EncodeDestination(dstScript, params, cfg));

    // Check decoding
    BOOST_CHECK(dstKey == DecodeDestination(cashaddr_pubkey, params));
    BOOST_CHECK(dstScript == DecodeDestination(cashaddr_script, params));
    BOOST_CHECK(dstKey == DecodeDestination(base58_pubkey, params));
    BOOST_CHECK(dstScript == DecodeDestination(base58_script, params));

    // Validation
    BOOST_CHECK(IsValidDestinationString(cashaddr_pubkey, params));
    BOOST_CHECK(IsValidDestinationString(cashaddr_script, params));
    BOOST_CHECK(IsValidDestinationString(base58_pubkey, params));
    BOOST_CHECK(IsValidDestinationString(base58_script, params));
    BOOST_CHECK(!IsValidDestinationString("notvalid", params));
}

BOOST_AUTO_TEST_SUITE_END()
