Bitcoin Cash Plus version 0.17.0 is now available from:

  <https://download.bitcoincashplus.org/0.17.0/>

This release includes the following features and fixes:

 - Remove the newdaaactivationtime configuration.
 - Do not use the NODE_BITCOIN_CASH service bit for preferencial peering anymore.
 - Only connect to node using the cash magic.
 - Remove indicator mentionning if a node uses the cash magic getpeerinfo RPC.
 - Add support for the new cashaddr format. The `-usecashaddr` flag can be used to select which format is used when presenting addresses to users. By default, Bitcoin Cash Plus will keep using the old format until Jan, 14 and then switch to the new format. Both format are now accepted as input.
