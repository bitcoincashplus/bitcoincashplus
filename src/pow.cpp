// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2017 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "chainparams.h"
#include "config.h"
#include "consensus/params.h"
#include "primitives/block.h"
#include "uint256.h"
#include "streams.h"
#include "util.h"
#include "validation.h"
#include "crypto/equihash.h"
#include <cassert>



/**
 * Compute the next required proof of work using the legacy Bitcoin difficulty
 */
 static uint32_t BitcoinGetNextWorkRequired(const CBlockIndex *pindexLast,
                                       const CBlockHeader *pblock,
                                       const Config &config) {
     const Consensus::Params &params = config.GetChainParams().GetConsensus();

     unsigned int nProofOfWorkLimit = UintToArith256(params.PowLimit(false)).GetCompact();

     // Genesis block
     if (pindexLast == NULL) return nProofOfWorkLimit;

     // Only change once per difficulty adjustment interval
     if ((pindexLast->nHeight + 1) % params.DifficultyAdjustmentInterval() !=0) {

         if (params.fPowAllowMinDifficultyBlocks) {
             // Special difficulty rule for testnet:
             // If the new block's timestamp is more than 2* 10 minutes then
             // allow mining of a min-difficulty block.
             if (pblock->GetBlockTime() >
                 pindexLast->GetBlockTime() + params.nPowTargetSpacing * 2) {
                 return nProofOfWorkLimit;
             }
             // Return the last non-special-min-difficulty-rules-block
             const CBlockIndex *pindex = pindexLast;
             while (pindex->pprev &&
                    pindex->nHeight % params.DifficultyAdjustmentInterval() !=
                        0 &&
                    pindex->nBits == nProofOfWorkLimit)
                 pindex = pindex->pprev;
             return pindex->nBits;
         }
         return pindexLast->nBits;
     }

     // Go back by what we want to be 14 days worth of blocks
     int nHeightFirst =pindexLast->nHeight - (params.DifficultyAdjustmentInterval() - 1);
     assert(nHeightFirst >= 0);
     const CBlockIndex *pindexFirst = pindexLast->GetAncestor(nHeightFirst);
     assert(pindexFirst);

     // Only change once per difficulty adjustment interval
     return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), config);
}


uint32_t GetNextWorkRequired(const CBlockIndex *pindexPrev,
                             const CBlockHeader *pblock, const Config &config) {
    const Consensus::Params &params = config.GetChainParams().GetConsensus();

    // Genesis block
    if (pindexPrev == nullptr) {
        return UintToArith256(params.PowLimit(false)).GetCompact();
    }

    // Special rule for regtest: we never retarget.
    if (params.fPowNoRetargeting) {
        return pindexPrev->nBits;
    }

    int nHeight = pindexPrev->nHeight + 1;
    bool postfork = nHeight >= params.BCPHeight;
    unsigned int nProofOfWorkLimit = UintToArith256(params.PowLimit(postfork)).GetCompact();
    if (postfork == false) {
        return BitcoinGetNextWorkRequired(pindexPrev, pblock, config);
    }

    else if (nHeight < params.BCPHeight + params.BCPPremineWindow) {
        return nProofOfWorkLimit;
    }
    else if (nHeight < params.BCPHeight + params.BCPPremineWindow + params.nPowAveragingWindow){
        return UintToArith256(params.powLimitStart).GetCompact();
    }

    return GetNextCashPlusWorkRequired(pindexPrev, pblock, config);
}


/* Deprecated for Bitcoin Cash Plus
*/
uint32_t CalculateNextWorkRequired(const CBlockIndex *pindexPrev,
                                   int64_t nFirstBlockTime,
                                   const Config &config) {
    const Consensus::Params &params = config.GetChainParams().GetConsensus();

    if (params.fPowNoRetargeting) {
        return pindexPrev->nBits;
    }

    // Limit adjustment step
    int64_t nActualTimespan = pindexPrev->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < params.nPowTargetTimespanLegacy / 4) {
        nActualTimespan = params.nPowTargetTimespanLegacy / 4;
    }

    if (nActualTimespan > params.nPowTargetTimespanLegacy * 4) {
        nActualTimespan = params.nPowTargetTimespanLegacy * 4;
    }

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.PowLimit(false));
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexPrev->nBits);
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespanLegacy;

    if (bnNew > bnPowLimit) bnNew = bnPowLimit;

    return bnNew.GetCompact();
}


/**
 * Check whether a block hash satisfies the proof-of-work requirement specified
 * by nBits
 */
bool CheckProofOfWork(uint256 hash, uint32_t nBits,bool postfork, const Consensus::Params& params){
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow ||
        bnTarget > UintToArith256(params.PowLimit(postfork))) {
        return false;
    }

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget) {
        return false;
    }

    return true;

}

bool CheckProofOfWork(uint256 hash, uint32_t nBits, bool postfork, const Config &config) {

  return CheckProofOfWork(hash,nBits,postfork,config.GetChainParams().GetConsensus());
}

/**
 * Compute a target based on the work done between 2 blocks and the time
 * required to produce that work.
 */
static arith_uint256 ComputeTarget(const CBlockIndex *pindexFirst,
                                   const CBlockIndex *pindexLast,
                                   const Consensus::Params &params) {
    assert(pindexLast->nHeight > pindexFirst->nHeight);

    /**
     * From the total work done and the time it took to produce that much work,
     * we can deduce how much work we expect to be produced in the targeted time
     * between blocks.
     */
    arith_uint256 work = pindexLast->nChainWork - pindexFirst->nChainWork;
    work *= params.nPowTargetSpacing;

    // In order to avoid difficulty cliffs, we bound the amplitude of the
    // adjustment we are going to do to a factor in [0.5, 2].
    int64_t nActualTimespan =
        int64_t(pindexLast->nTime) - int64_t(pindexFirst->nTime);
    if (nActualTimespan > 288 * params.nPowTargetSpacing) {
        nActualTimespan = 288 * params.nPowTargetSpacing;
    } else if (nActualTimespan < 72 * params.nPowTargetSpacing) {
        nActualTimespan = 72 * params.nPowTargetSpacing;
    }

    work /= nActualTimespan;

    /**
     * We need to compute T = (2^256 / W) - 1 but 2^256 doesn't fit in 256 bits.
     * By expressing 1 as W / W, we get (2^256 - W) / W, and we can compute
     * 2^256 - W as the complement of W.
     */
    return (-work) / work;
}

/**
 * To reduce the impact of timestamp manipulation, we select the block we are
 * basing our computation on via a median of 3.
 */
static const CBlockIndex *GetSuitableBlock(const CBlockIndex *pindex) {
    assert(pindex->nHeight >= 3);

    /**
     * In order to avoid a block with a very skewed timestamp having too much
     * influence, we select the median of the 3 top most blocks as a starting
     * point.
     */
    const CBlockIndex *blocks[3];
    blocks[2] = pindex;
    blocks[1] = pindex->pprev;
    blocks[0] = blocks[1]->pprev;

    // Sorting network.
    if (blocks[0]->nTime > blocks[2]->nTime) {
        std::swap(blocks[0], blocks[2]);
    }

    if (blocks[0]->nTime > blocks[1]->nTime) {
        std::swap(blocks[0], blocks[1]);
    }

    if (blocks[1]->nTime > blocks[2]->nTime) {
        std::swap(blocks[1], blocks[2]);
    }

    // We should have our candidate in the middle now.
    return blocks[1];
}

/**
 * Compute the next required proof of work using a weighted average of the
 * estimated hashrate per block.
 *
 * Using a weighted average ensure that the timestamp parameter cancels out in
 * most of the calculation - except for the timestamp of the first and last
 * block. Because timestamps are the least trustworthy information we have as
 * input, this ensures the algorithm is more resistant to malicious inputs.
 */
uint32_t GetNextCashPlusWorkRequired(const CBlockIndex *pindexPrev,
                                 const CBlockHeader *pblock,
                                 const Config &config) {

    const Consensus::Params &params = config.GetChainParams().GetConsensus();

    // This cannot handle the genesis block and early blocks in general.
    assert(pindexPrev);

    // Special difficulty rule for testnet:
    // If the new block's timestamp is more than 2* 10 minutes then allow
    // mining of a min-difficulty block.
    if (params.fPowAllowMinDifficultyBlocks &&
        (pblock->GetBlockTime() >
         pindexPrev->GetBlockTime() + 2 * params.nPowTargetSpacing)) {
        return UintToArith256(params.PowLimit(true)).GetCompact();
    }

    // Compute the difficulty based on the full adjustment interval.
    const uint32_t nHeight = pindexPrev->nHeight;
    assert(nHeight >= params.DifficultyAdjustmentInterval());

    // Get the last suitable block of the difficulty interval.
    const CBlockIndex *pindexLast = GetSuitableBlock(pindexPrev);
    assert(pindexLast);

    // Get the first suitable block of the difficulty interval.
    uint32_t nHeightFirst = nHeight - 144;
    const CBlockIndex *pindexFirst =
        GetSuitableBlock(pindexPrev->GetAncestor(nHeightFirst));
    assert(pindexFirst);

    // Compute the target based on time and work done during the interval.
    const arith_uint256 nextTarget =
        ComputeTarget(pindexFirst, pindexLast, params);

    const arith_uint256 powLimit = UintToArith256(params.PowLimit(true));
    if (nextTarget > powLimit) {
        return powLimit.GetCompact();
    }

    return nextTarget.GetCompact();
}


bool CheckEquihashSolution(const CBlockHeader *pblock, const Config &config)
{
    unsigned int n = config.GetChainParams().EquihashN();
    unsigned int k = config.GetChainParams().EquihashK();

    // Hash state
    crypto_generichash_blake2b_state state;
    EhInitialiseState(n, k, state);

    // I = the block header minus nonce and solution.
    CEquihashInput I{*pblock};
    // I||V
    CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
    ss << I;
    ss << pblock->nNonce;

    // H(I||V||...
    crypto_generichash_blake2b_update(&state, (unsigned char*)&ss[0], ss.size());

    bool isValid;
    EhIsValidSolution(n, k, state, pblock->nSolution, isValid);
    if (!isValid)
        return error("CheckEquihashSolution(): invalid solution");

    return true;
}
