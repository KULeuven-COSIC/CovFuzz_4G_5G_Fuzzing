/*
 * XOR algorithm
 */

#if !defined(OGS_CRYPT_INSIDE) && !defined(OGS_CRYPT_COMPILATION)
#error "This header cannot be included directly."
#endif

#ifndef XOR_H
#define XOR_H

#ifdef __cplusplus
extern "C" {
#endif

void xor_generate(const uint8_t *opc, const uint8_t *amf, 
    const uint8_t *k, const uint8_t *sqn, const uint8_t *_rand, 
    uint8_t *autn, uint8_t *ik, uint8_t *ck, uint8_t *ak,
    uint8_t *res, size_t *res_len);

// void xor_opc(const uint8_t *k, const uint8_t *op,  uint8_t *opc);

#ifdef __cplusplus
}
#endif

#endif /* XOR_H */
