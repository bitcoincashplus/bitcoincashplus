// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2017 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "uritests.h"

#include "chainparams.h"
#include "config.h"
#include "guiutil.h"
#include "walletmodel.h"

#include <QUrl>

void URITests::uriTestsBase58() {
    SendCoinsRecipient rv;
    QString scheme =
        QString::fromStdString(Params(CBaseChainParams::MAIN).CashAddrPrefix());
    QUrl uri;
    uri.setUrl(QString("175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W?req-dontexist="));
    QVERIFY(!GUIUtil::parseBitcoinURI(scheme, uri, &rv));

    uri.setUrl(
        QString("bitcoincashplus:175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W?dontexist="));
    QVERIFY(GUIUtil::parseBitcoinURI(scheme, uri, &rv));
    QVERIFY(rv.address == QString("175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == Amount(0));

    uri.setUrl(QString("bitcoincashplus:175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W?label="
                       "Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(scheme, uri, &rv));
    QVERIFY(rv.address == QString("175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W"));
    QVERIFY(rv.label == QString("Wikipedia Example Address"));
    QVERIFY(rv.amount == Amount(0));

    uri.setUrl(
        QString("bitcoincashplus:175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W?amount=0.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(scheme, uri, &rv));
    QVERIFY(rv.address == QString("175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == Amount(100000));

    uri.setUrl(
        QString("bitcoincashplus:175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W?amount=1.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(scheme, uri, &rv));
    QVERIFY(rv.address == QString("175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == Amount(100100000));

    uri.setUrl(
        QString("bitcoincashplus:175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W?amount=100&"
                "label=Wikipedia Example"));
    QVERIFY(GUIUtil::parseBitcoinURI(scheme, uri, &rv));
    QVERIFY(rv.address == QString("175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W"));
    QVERIFY(rv.amount == Amount(10000000000LL));
    QVERIFY(rv.label == QString("Wikipedia Example"));

    uri.setUrl(QString("bitcoincashplus:175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W?message="
                       "Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(scheme, uri, &rv));
    QVERIFY(rv.address == QString("175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W"));
    QVERIFY(rv.label == QString());

    QVERIFY(GUIUtil::parseBitcoinURI(scheme,
                                     "bitcoincashplus://"
                                     "175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W?"
                                     "message=Wikipedia Example Address",
                                     &rv));
    QVERIFY(rv.address == QString("175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W"));
    QVERIFY(rv.label == QString());

    uri.setUrl(
        QString("bitcoincashplus:175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W?req-message="
                "Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(scheme, uri, &rv));

    uri.setUrl(
        QString("bitcoincashplus:175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W?amount=1,"
                "000&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(scheme, uri, &rv));

    uri.setUrl(
        QString("bitcoincashplus:175tWpb8K1S7NmH4Zx6rewF9WQrcZv245W?amount=1,"
                "000.0&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(scheme, uri, &rv));
}

void URITests::uriTestsCashAddr() {
    SendCoinsRecipient rv;
    QUrl uri;
    QString scheme =
        QString::fromStdString(Params(CBaseChainParams::MAIN).CashAddrPrefix());
    uri.setUrl(QString("bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f?"
                       "req-dontexist="));
    QVERIFY(!GUIUtil::parseBitcoinURI(scheme, uri, &rv));

    uri.setUrl(QString(
        "bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f?dontexist="));
    QVERIFY(GUIUtil::parseBitcoinURI(scheme, uri, &rv));
    QVERIFY(rv.address ==
            QString("bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == Amount(0));

    uri.setUrl(
        QString("bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f?label="
                "Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(scheme, uri, &rv));
    QVERIFY(rv.address ==
            QString("bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f"));
    QVERIFY(rv.label == QString("Wikipedia Example Address"));
    QVERIFY(rv.amount == Amount(0));

    uri.setUrl(QString(
        "bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f?amount=0.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(scheme, uri, &rv));
    QVERIFY(rv.address ==
            QString("bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == Amount(100000));

    uri.setUrl(QString(
        "bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f?amount=1.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(scheme, uri, &rv));
    QVERIFY(rv.address ==
            QString("bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == Amount(100100000));

    uri.setUrl(QString(
        "bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f?amount=100&"
        "label=Wikipedia Example"));
    QVERIFY(GUIUtil::parseBitcoinURI(scheme, uri, &rv));
    QVERIFY(rv.address ==
            QString("bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f"));
    QVERIFY(rv.amount == Amount(10000000000LL));
    QVERIFY(rv.label == QString("Wikipedia Example"));

    uri.setUrl(QString(
        "bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f?message="
        "Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(scheme, uri, &rv));
    QVERIFY(rv.address ==
            QString("bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f"));
    QVERIFY(rv.label == QString());

    QVERIFY(GUIUtil::parseBitcoinURI(
        scheme, "bitcoincashplus://"
                "qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f?"
                "message=Wikipedia Example Address",
        &rv));
    QVERIFY(rv.address ==
            QString("bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f"));
    QVERIFY(rv.label == QString());

    uri.setUrl(QString(
        "bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f?req-message="
        "Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(scheme, uri, &rv));

    uri.setUrl(QString(
        "bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f?amount=1,"
        "000&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(scheme, uri, &rv));

    uri.setUrl(QString(
        "bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f?amount=1,"
        "000.0&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(scheme, uri, &rv));
}

namespace {
class UriTestConfig : public DummyConfig {
public:
    UriTestConfig(bool useCashAddr)
        : useCashAddr(useCashAddr), net(CBaseChainParams::MAIN) {}
    bool UseCashAddrEncoding() const override { return useCashAddr; }
    const CChainParams &GetChainParams() const override { return Params(net); }
    void SetChainParams(const std::string &n) { net = n; }

private:
    bool useCashAddr;
    std::string net;
};

} // anon ns

void URITests::uriTestFormatURI() {
    {
        UriTestConfig cfg(true);
        SendCoinsRecipient r;
        r.address = "bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f";
        r.message = "test";
        QString uri = GUIUtil::formatBitcoinURI(cfg, r);
        QVERIFY(uri == "bitcoincashplus:qqqprqq976hvnqkeajpc33u5rt92xw5vm5ylgfku0f?"
                       "message=test");
    }

    {
        UriTestConfig cfg(false);
        SendCoinsRecipient r;
        r.address = "CGXa8qa2kKjkmAYjHFsS5j6y4KNbKfNfUS";
        r.message = "test";
        QString uri = GUIUtil::formatBitcoinURI(cfg, r);
        QVERIFY(uri ==
                "bitcoincashplus:CGXa8qa2kKjkmAYjHFsS5j6y4KNbKfNfUS?message=test");
    }
}

void URITests::uriTestScheme() {
    {
        // cashaddr - scheme depends on selected chain params
        UriTestConfig config(true);
        config.SetChainParams(CBaseChainParams::MAIN);
        QVERIFY("bitcoincashplus" == GUIUtil::bitcoinURIScheme(config));
        config.SetChainParams(CBaseChainParams::TESTNET);
        QVERIFY("bcptest" == GUIUtil::bitcoinURIScheme(config));
        config.SetChainParams(CBaseChainParams::REGTEST);
        QVERIFY("bcpreg" == GUIUtil::bitcoinURIScheme(config));
    }
    {
        // legacy - scheme is "bitcoincash" regardless of chain params
        UriTestConfig config(false);
        config.SetChainParams(CBaseChainParams::MAIN);
        QVERIFY("bitcoincashplus" == GUIUtil::bitcoinURIScheme(config));
        config.SetChainParams(CBaseChainParams::TESTNET);
        QVERIFY("bitcoincashplus" == GUIUtil::bitcoinURIScheme(config));
        config.SetChainParams(CBaseChainParams::REGTEST);
        QVERIFY("bitcoincashplus" == GUIUtil::bitcoinURIScheme(config));
    }
}
