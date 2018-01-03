// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_POW_H
#define BITCOIN_POW_H

#include <cstdint>
#include "consensus/params.h"

class CBlockHeader;
class CBlockIndex;
class Config;
class uint256;
class CChainParams;

uint32_t GetNextWorkRequired(const CBlockIndex *pindexPrev,
                             const CBlockHeader *pblock, const Config &config);
uint32_t CalculateNextWorkRequired(const CBlockIndex *pindexPrev,
                                   int64_t nFirstBlockTime,
                                   const Config &config);

/**
 * Check whether a block hash satisfies the proof-of-work requirement specified
 * by nBits
 */
bool CheckProofOfWork(uint256 hash, uint32_t nBits, bool postfork, const Config &config);

/**
 * Check whether a block hash satisfies the proof-of-work requirement specified
 * by nBits
 */
bool CheckProofOfWork(uint256 hash, uint32_t nBits, bool postfork, const Consensus::Params& );

/**
 * Bitcoin cash's difficulty adjustment mechanism.
 */
uint32_t GetNextCashPlusWorkRequired(const CBlockIndex *pindexPrev,
                                 const CBlockHeader *pblock,
                                 const Config &config);

/** Check whether the Equihash solution in a block header is valid */
bool CheckEquihashSolution(const CBlockHeader *pblock, const Config &config);

#endif // BITCOIN_POW_H
