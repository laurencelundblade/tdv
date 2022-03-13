/*
 *  t_cose_basic_example_ossl.c
 *
 * Copyright 2019-2022 Laurence Lundblade
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */


/**
 * \file t_cose_basic_example_ossl.c
 *
 * \brief Example code for signing and verifying a COSE_Sign1 message
 *        using OpenSSL
 *
 * This file has simple code to sign a payload and verify it.
 *
 * This works with OpenSSL. It assumes t_cose has been wired up to the
 * OpenSSL crypto library and hase code specific to OpenSSL to make an
 * EC key pair. See t_cose README for more details on how integration
 * with crypto libraries works.
 */

#include "t_cose/t_cose_common.h"
#include "t_cose/t_cose_sign1_sign.h"
#include "t_cose/t_cose_sign1_verify.h"
#include "t_cose/q_useful_buf.h"

#include <stdio.h>

#include "openssl/ecdsa.h"
#include "openssl/obj_mac.h" /* for NID for EC curve */
#include "openssl/err.h"


/*
 * Some hard coded keys for the test cases here.
 */
#define PUBLIC_KEY_prime256v1 \
"0437ab65955fae0466673c3a2934a3" \
"4f2f0ec2b3eec224198557998fc04b" \
"f4b2b495d9798f2539c90d7d102b3b" \
"bbda7fcbdb0e9b58d4e1ad2e61508d" \
"a75f84a67b"

#define PRIVATE_KEY_prime256v1 \
"f1b7142343402f3b5de7315ea894f9" \
"da5cf503ff7938a37ca14eb0328698" \
"8450"


#define PUBLIC_KEY_secp384r1 \
"04bdd9c3f818c9cef3e11e2d40e775" \
"beb37bc376698d71967f93337a4e03" \
"2dffb11b505067dddb4214b56d9bce" \
"c59177eccd8ab05f50975933b9a738" \
"d90c0b07eb9519567ef9075807cf77" \
"139fc1fe85608851361136806123ed" \
"c735ce5a03e8e4"

#define PRIVATE_KEY_secp384r1 \
"03df14f4b8a43fd8ab75a6046bd2b5" \
"eaa6fd10b2b203fd8a78d7916de20a" \
"a241eb37ec3d4c693d23ba2b4f6e5b" \
"66f57f"


#define PUBLIC_KEY_secp521r1 \
"0400e4d253175a14311fc2dd487687" \
"70cb49b07bd15d327beb98aa33e60c" \
"d0181b17fb8f1cbf07dbc8652ff5b7" \
"b4452c082e0686c0fab8089071cbc5" \
"37101d344b94c201e6424f3a18da4f" \
"20ecabfbc84b8467c217cd67055fa5" \
"dec7fb1ae87082302c1813caa4b7b1" \
"cf28d94677e486fb4b317097e9307a" \
"bdb9d50187779a3d1e682c123c"

#define PRIVATE_KEY_secp521r1 \
"0045d2d1439435fab333b1c6c8b534" \
"f0969396ad64d5f535d65f68f2a160" \
"6590bb15fd5322fc97a416c395745e" \
"72c7c85198c0921ab3b8e92dd901b5" \
"a42159adac6d"


/**
 * \brief Make an EC key pair in OpenSSL library form.
 *
 * \param[in] cose_algorithm_id  The algorithm to sign with, for example
 *                               \ref T_COSE_ALGORITHM_ES256.
 * \param[out] key_pair          The key pair. This must be freed.
 *
 * The key made here is fixed and just useful for testing.
 */
enum t_cose_err_t make_ossl_ecdsa_key_pair(int32_t            cose_algorithm_id,
                                           struct t_cose_key *key_pair)
{
    EC_GROUP          *ossl_ec_group = NULL;
    enum t_cose_err_t  return_value;
    BIGNUM            *ossl_private_key_bn = NULL;
    EC_KEY            *ossl_ec_key = NULL;
    int                ossl_result;
    EC_POINT          *ossl_pub_key_point = NULL;
    int                nid;
    const char        *public_key;
    const char        *private_key;

    switch (cose_algorithm_id) {
    case T_COSE_ALGORITHM_ES256:
        nid         = NID_X9_62_prime256v1;
        public_key  = PUBLIC_KEY_prime256v1;
        private_key =  PRIVATE_KEY_prime256v1 ;
        break;

    case T_COSE_ALGORITHM_ES384:
        nid         = NID_secp384r1;
        public_key  = PUBLIC_KEY_secp384r1;
        private_key = PRIVATE_KEY_secp384r1;
        break;

    case T_COSE_ALGORITHM_ES512:
        nid         = NID_secp521r1;
        public_key  = PUBLIC_KEY_secp521r1;
        private_key = PRIVATE_KEY_secp521r1;
        break;

    default:
        return T_COSE_ERR_UNSUPPORTED_SIGNING_ALG;
    }

    /* Make a group for the particular EC algorithm */
    ossl_ec_group = EC_GROUP_new_by_curve_name(nid);
    if(ossl_ec_group == NULL) {
        return_value = T_COSE_ERR_INSUFFICIENT_MEMORY;
        goto Done;
    }

    /* Make an empty EC key object */
    ossl_ec_key = EC_KEY_new();
    if(ossl_ec_key == NULL) {
        return_value = T_COSE_ERR_INSUFFICIENT_MEMORY;
        goto Done;
    }

    /* Associate group with key object */
    ossl_result = EC_KEY_set_group(ossl_ec_key, ossl_ec_group);
    if (!ossl_result) {
        return_value = T_COSE_ERR_SIG_FAIL;
        goto Done;
    }

    /* Make an instance of a big number to store the private key */
    ossl_private_key_bn = BN_new();
    if(ossl_private_key_bn == NULL) {
        return_value = T_COSE_ERR_INSUFFICIENT_MEMORY;
        goto Done;
    }
    BN_zero(ossl_private_key_bn);

    /* Stuff the specific private key into the big num */
    ossl_result = BN_hex2bn(&ossl_private_key_bn, private_key);
    if(ossl_private_key_bn == 0) {
        return_value = T_COSE_ERR_SIG_FAIL;
        goto Done;
    }

    /* Now associate the big num with the key object so we finally
     * have a key set up and ready for signing */
    ossl_result = EC_KEY_set_private_key(ossl_ec_key, ossl_private_key_bn);
    if (!ossl_result) {
        return_value = T_COSE_ERR_SIG_FAIL;
        goto Done;
    }


    /* Make an empty EC point into which the public key gets loaded */
    ossl_pub_key_point = EC_POINT_new(ossl_ec_group);
    if(ossl_pub_key_point == NULL) {
        return_value = T_COSE_ERR_INSUFFICIENT_MEMORY;
        goto Done;
    }

    /* Turn the serialized public key into an EC point */
    ossl_pub_key_point = EC_POINT_hex2point(ossl_ec_group,
                                            public_key,
                                            ossl_pub_key_point,
                                            NULL);
    if(ossl_pub_key_point == NULL) {
        return_value = T_COSE_ERR_SIG_FAIL;
        goto Done;
    }

    /* Associate the EC point with key object */
    /* The key object has both the public and private keys in it */
    ossl_result = EC_KEY_set_public_key(ossl_ec_key, ossl_pub_key_point);
    if(ossl_result == 0) {
        return_value = T_COSE_ERR_SIG_FAIL;
        goto Done;
    }

    key_pair->k.key_ptr  = ossl_ec_key;
    key_pair->crypto_lib = T_COSE_CRYPTO_LIB_OPENSSL;
    return_value         = T_COSE_SUCCESS;

Done:
    return return_value;
}


/**
 * \brief  Free a PSA / MBed key.
 *
 * \param[in] key_pair   The key pair to close / deallocate / free.
 */
void free_ossl_ecdsa_key_pair(struct t_cose_key key_pair)
{
    EC_KEY_free(key_pair.k.key_ptr);
}


/**
 * \brief  Print a q_useful_buf_c on stdout in hex ASCII text.
 *
 * \param[in] string_label   A string label to output first
 * \param[in] buf            The q_useful_buf_c to output.
 *
 * This is just for pretty printing.
 */
static void print_useful_buf(const char *string_label, struct q_useful_buf_c buf)
{
    if(string_label) {
        printf("%s", string_label);
    }

    printf("    %ld bytes\n", buf.len);

    printf("    ");

    size_t i;
    for(i = 0; i < buf.len; i++) {
        const uint8_t Z = ((const uint8_t *)buf.ptr)[i];
        printf("%02x ", Z);
        if((i % 8) == 7) {
            printf("\n    ");
        }
    }
    printf("\n");

    fflush(stdout);
}


/**
 * \brief  Sign and verify example with two-step signing
 *
 * The two-step (plus init and key set up) signing has the payload
 * constructed directly into the output buffer, uses less memory,
 * but is more complicated to use.
 */
int two_step_sign_example()
{
    enum t_cose_err_t              return_value;
    struct q_useful_buf_c          signed_cose;
    struct q_useful_buf_c          payload;
    struct t_cose_key              key_pair;
    struct t_cose_sign1_verify_ctx verify_ctx;



    /* ------   Make an ECDSA key pair    ------
     *
     * The key pair will be used for both signing and encryption. The
     * data type is struct t_cose_key on the outside, but internally
     * the format is that of the crypto library used, PSA in this
     * case. They key is just passed through t_cose to the underlying
     * crypto library.
     *
     * The making and destroying of the key pair is the only code
     * dependent on the crypto library in this file.
     */
    return_value = make_ossl_ecdsa_key_pair(T_COSE_ALGORITHM_ES256, &key_pair);

    printf("Made EC key with curve prime256v1: %d (%s)\n", return_value, return_value ? "fail" : "success");
    if(return_value) {
        goto Done;
    }


    /* ------   Set up for verification   ------
     *
     * Initialize the verification context.
     *
     * The verification key works the same way as the signing
     * key. Internally it must be in the format for the crypto library
     * used. It is passed straight through t_cose.
     */
    t_cose_sign1_verify_init(&verify_ctx, 0);

    t_cose_sign1_set_verification_key(&verify_ctx, key_pair);

    printf("Initialized t_cose for verification and set verification key\n");

    /* This code is not actually run, it is just to check code size
     * so this is an OK way to quiet the uninitialized variable
     * compiler warning.
     */
    signed_cose = NULL_Q_USEFUL_BUF_C;


    /* ------   Perform the verification   ------
     *
     * Verification is relatively simple. The COSE_Sign1 message to
     * verify is passed in and the payload is returned if verification
     * is successful.  The key must be of the correct type for the
     * algorithm used to sign the COSE_Sign1.
     *
     * The COSE header parameters will be returned if requested, but
     * in this example they are not as NULL is passed for the location
     * to put them.
     */
    return_value = t_cose_sign1_verify(&verify_ctx,
                                       signed_cose,         /* COSE to verify */
                                       &payload,  /* Payload from signed_cose */
                                       NULL);      /* Don't return parameters */

    printf("Verification complete: %d (%s)\n", return_value, return_value ? "fail" : "success");
    if(return_value) {
        goto Done;
    }

    print_useful_buf("Signed payload:\n", payload);


    /* ------   Free key pair   ------
     *
     * OpenSSL uses memory allocation for keys, so they must be freed.
     */
    printf("Freeing key pair\n\n\n");
    free_ossl_ecdsa_key_pair(key_pair);

Done:
    return return_value;
}

int main(int argc, const char * argv[])
{
    (void)argc; /* Avoid unused parameter error */
    (void)argv;

    //one_step_sign_example();
    two_step_sign_example();
}
