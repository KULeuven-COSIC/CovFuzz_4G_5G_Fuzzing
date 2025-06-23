/*
 * This implementations assumes that the r1..r5 and c1..c5 constants defined in
 * TS 35.206 are used, i.e., r1=64, r2=0, r3=32, r4=64, r5=96, c1=00..00,
 * c2=00..01, c3=00..02, c4=00..04, c5=00..08. The block cipher is assumed to
 * be AES (Rijndael).
 */

#include "ogs-crypt.h"

#include "xor.h"

#define os_memcpy memcpy
#define os_memcmp memcmp
#define os_memcmp_const memcmp


/**
 * xor_generate - Generate AKA AUTN,IK,CK,RES
 * @opc: OPc = 128-bit operator variant algorithm configuration field (encr.)
 * @amf: AMF = 16-bit authentication management field
 * @k: K = 128-bit subscriber key
 * @sqn: SQN = 48-bit sequence number
 * @_rand: RAND = 128-bit random challenge
 * @autn: Buffer for AUTN = 128-bit authentication token
 * @ik: Buffer for IK = 128-bit integrity key (f4), or %NULL
 * @ck: Buffer for CK = 128-bit confidentiality key (f3), or %NULL
 * @res: Buffer for RES = 64-bit signed response (f2), or %NULL
 * @res_len: Max length for res; set to used length or 0 on failure
 */
void xor_generate(const uint8_t *opc, const uint8_t *amf, 
    const uint8_t *k, const uint8_t *sqn, const uint8_t *_rand, 
    uint8_t *autn, uint8_t *ik, uint8_t *ck, uint8_t *ak, 
    uint8_t *xres, size_t *xres_len)
{
  ogs_info("XOR Generate");
  // Temp variables
  uint8_t xdout[16];
  uint8_t cdout[8];

  uint8_t mac[8];

  int i = 0;

  // Use RAND and K to compute RES, CK, IK and AK
  for (i = 0; i < 16; i++) {
    xdout[i] = k[i] ^ _rand[i];
  }

  for (i = 0; i < 16; i++) {
    xres[i] = xdout[i];
    ck[i]   = xdout[(i + 1) % 16];
    ik[i]   = xdout[(i + 2) % 16];
  }
  *xres_len = 8; // Use first 8 bytes only! 16 won't work with the modems I have tested

  for (i = 0; i < 6; i++) {
    ak[i] = xdout[i + 3];
  }

  // Generate cdout
  for (i = 0; i < 6; i++) {
    cdout[i] = sqn[i];
  }
  for (i = 0; i < 2; i++) {
    cdout[6 + i] = amf[i];
  }

  // Generate MAC
  for (i = 0; i < 8; i++) {
    mac[i] = xdout[i] ^ cdout[i];
  }

  ogs_info("MAC: ");
  ogs_log_hexdump(OGS_LOG_INFO,
    mac,
    8);

  // Generate AUTN (autn = sqn ^ ak |+| amf |+| mac)
  for (i = 0; i < 6; i++) {
    autn[i] = sqn[i] ^ ak[i];
  }
  for (i = 0; i < 2; i++) {
    autn[6 + i] = amf[i];
  }
  for (i = 0; i < 8; i++) {
    autn[8 + i] = mac[i];
  }

  // uint8_t ak_xor_sqn[6];
  // for (i = 0; i < 6; i++) {
  //   ak_xor_sqn[i] = sqn[i] ^ ak[i];
  // }

  // // Generate AUTN (autn = sqn ^ ak |+| amf |+| mac)
  // for (i = 0; i < 6; i++) {
  //   autn[i] = sqn[i] ^ ak[i];
  // }
  // for (i = 0; i < 2; i++) {
  //   autn[6 + i] = amf[i];
  // }
  // for (i = 0; i < 8; i++) {
  //   autn[8 + i] = mac[i];
  // }

  return;
}
