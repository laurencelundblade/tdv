/*
 * t_cose_call_all.c
 *
 * Copyright (c) 2023, Laurence Lundblade. All rights reserved.
 * Created by Laurence Lundblade on 2/28/23.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 *
 */


/* This is here so there is a symbol before main to enable
 * use of nm output to calculate code size of main().
 */
void start_marker(void) {}

/*
 * This is for testing and evaluation of t_cose.
 * It invokes every method in the public header files.
 * It will not successefully run at all. It's purpose is
 * to link in every function in the library.
 */

#include "t_cose/t_cose_key.h"

void call_all_key(void)
{
    struct t_cose_key   key;
    enum t_cose_err_t   err;

    err = t_cose_key_init_symmetric(0, NULL_Q_USEFUL_BUF_C, NULL);

    t_cose_key_free_symmetric(key);
}


#include "t_cose/t_cose_sign_sign.h"

void call_all_sign_sign(void)
{
    struct t_cose_sign_sign_ctx context;
    enum t_cose_err_t           err;

    t_cose_sign_sign_init(&context, 0);
    t_cose_sign_add_signer(&context, NULL);

#ifndef T_COSE_MINIMUM
    t_cose_sign_add_body_header_params(&context, NULL);
    err = t_cose_sign_sign(&context,
                           NULL_Q_USEFUL_BUF_C,
                           NULL_Q_USEFUL_BUF_C,
                           NULL_Q_USEFUL_BUF,
                           NULL);
    err = t_cose_sign_sign_detached(&context,
                                    NULL_Q_USEFUL_BUF_C,
                                    NULL_Q_USEFUL_BUF_C,
                                    NULL_Q_USEFUL_BUF,
                                    NULL);
#endif /* !T_COSE_MINIMUM */

    err = t_cose_sign_encode_start(&context,
                                   NULL);
    err = t_cose_sign_encode_finish(&context,
                                    NULL_Q_USEFUL_BUF_C,
                                    NULL_Q_USEFUL_BUF_C,
                                    NULL);
}


#include "t_cose/t_cose_sign_verify.h"

void call_all_sign_verify(void)
{
    struct t_cose_sign_verify_ctx me;
    enum t_cose_err_t             err;
    void                        *p;


    t_cose_sign_verify_init(&me, 0);
    t_cose_sign_add_verifier(&me, NULL);

#ifndef T_COSE_MINIMUM
    t_cose_sign_add_param_storage(&me, NULL);
    t_cose_sign_set_special_param_decoder(&me,
                                          NULL,
                                          NULL);
#endif /* !T_COSE_MINIMUM */

    err = t_cose_sign_verify(&me,
                             NULL_Q_USEFUL_BUF_C,
                             NULL_Q_USEFUL_BUF_C,
                             NULL,
                             NULL);

#ifndef T_COSE_MINIMUM
    err = t_cose_sign_verify_detached(&me,
                             NULL_Q_USEFUL_BUF_C,
                             NULL_Q_USEFUL_BUF_C,
                             NULL_Q_USEFUL_BUF_C,
                             NULL);
    p = t_cose_sign_verify_get_last(&me);
#endif /* !T_COSE_MINIMUM */
}


#ifndef T_COSE_MINIMUM
#include "t_cose/t_cose_sign1_sign.h"

void call_all_sign1_sign(void)
{
    struct t_cose_sign1_sign_ctx me;
    enum t_cose_err_t            err;
    struct t_cose_key            key;
    size_t                       a_size;

    t_cose_sign1_sign_init(&me, 0, 0);
    t_cose_sign1_set_signing_key(&me, key, NULL_Q_USEFUL_BUF_C);
#ifndef T_COSE_DISABLE_CONTENT_TYPE
    t_cose_sign1_set_content_type_uint(&me, 0);
    t_cose_sign1_set_content_type_tstr(&me, "xxx");
#endif /* !T_COSE_DISABLE_CONTENT_TYPE */
    t_cose_sign1_sign_set_auxiliary_buffer(&me, NULL_Q_USEFUL_BUF);
    a_size = t_cose_sign1_sign_auxiliary_buffer_size(&me);
    err = t_cose_sign1_sign(&me,
                            NULL_Q_USEFUL_BUF_C,
                            NULL_Q_USEFUL_BUF,
                            NULL);
    err = t_cose_sign1_sign_aad(&me,
                                NULL_Q_USEFUL_BUF_C,
                                NULL_Q_USEFUL_BUF_C,
                                NULL_Q_USEFUL_BUF,
                                NULL);
    err = t_cose_sign1_sign_detached(&me,
                                     NULL_Q_USEFUL_BUF_C,
                                     NULL_Q_USEFUL_BUF_C,
                                     NULL_Q_USEFUL_BUF,
                                     NULL);

    err = t_cose_sign1_encode_parameters(&me, NULL);
    err = t_cose_sign1_encode_signature(&me, NULL);
    err = t_cose_sign1_encode_signature_aad(&me,
                                            NULL_Q_USEFUL_BUF_C,
                                            NULL);

}


#include "t_cose/t_cose_sign1_verify.h"

void call_all_sign1_verify(void)
{
    struct t_cose_sign1_verify_ctx me;
    struct t_cose_key            key;
    size_t                       a_size;
    enum t_cose_err_t            err;
    uint64_t                     cbor_tag;


    t_cose_sign1_verify_init(&me, 0);
    t_cose_sign1_set_verification_key(&me, key);
    t_cose_sign1_verify_set_auxiliary_buffer(&me, NULL_Q_USEFUL_BUF);
    a_size = t_cose_sign1_verify_auxiliary_buffer_size(&me);
    err = t_cose_sign1_verify(&me,
                              NULL_Q_USEFUL_BUF_C,
                              NULL,
                              NULL);

    err = t_cose_sign1_verify_aad(&me,
                                  NULL_Q_USEFUL_BUF_C,
                                  NULL_Q_USEFUL_BUF_C,
                                  NULL,
                                  NULL);

    err = t_cose_sign1_verify_detached(&me,
                                       NULL_Q_USEFUL_BUF_C,
                                       NULL_Q_USEFUL_BUF_C,
                                       NULL_Q_USEFUL_BUF_C,
                                       NULL);

    cbor_tag = t_cose_sign1_get_nth_tag(&me, 0);
}
#endif /* !T_COSE_MINIMUM */



#include "t_cose/t_cose_signature_sign_main.h"

void call_all_signature_sign_main(void)
{
    struct t_cose_signature_sign_main me;
    struct t_cose_key                 key;
    void                               *p;

    t_cose_signature_sign_main_init(&me, 0);
    t_cose_signature_sign_main_set_signing_key(&me,
                                               key,
                                               NULL_Q_USEFUL_BUF_C
                                               );
#ifndef T_COSE_MINIMUM
    t_cose_signature_sign_main_set_crypto_context(&me, NULL);
    t_cose_signature_sign_main_set_header_parameter(&me, NULL);
#endif /* !T_COSE_MINIMUM */
    p = t_cose_signature_sign_from_main(&me);
}


#include "t_cose/t_cose_signature_verify_main.h"

void call_all_signature_verify_main(void)
{
    struct t_cose_signature_verify_main me;
    struct t_cose_key                   key;
    void                               *p;

    t_cose_signature_verify_main_init(&me);
    t_cose_signature_verify_main_set_key(&me, key, NULL_Q_USEFUL_BUF_C);
#ifndef T_COSE_MINIMUM
    t_cose_signature_verify_main_set_crypto_context(&me, NULL);
    t_cose_signature_verify_main_set_special_param_decoder(&me, NULL, NULL);
#endif /* !T_COSE_MINIMUM */
    p = t_cose_signature_verify_from_main(&me);
}


#ifndef T_COSE_MINIMUM
#ifndef T_COSE_DISABLE_EDDSA

#include "t_cose/t_cose_signature_sign_eddsa.h"

void call_all_signature_sign_eddsa(void)
{
    struct t_cose_signature_sign_eddsa me;
    struct t_cose_key                  key;
    size_t                             n;
    void                              *p;

    t_cose_signature_sign_eddsa_init(&me);
    t_cose_signature_sign_eddsa_set_signing_key(&me, key, NULL_Q_USEFUL_BUF_C);
    t_cose_signature_sign_eddsa_set_header_parameter(&me, NULL);
    t_cose_signature_sign_eddsa_set_auxiliary_buffer(&me, NULL_Q_USEFUL_BUF);
    n = t_cose_signature_sign_eddsa_auxiliary_buffer_size(&me);
    p = t_cose_signature_sign_from_eddsa(&me);
}

#include "t_cose/t_cose_signature_verify_eddsa.h"

void call_all_signature_verify_eddsa(void)
{
     struct t_cose_signature_verify_eddsa me;
     struct t_cose_key                    key;
     size_t                               n;
     void                                *p;

    t_cose_signature_verify_eddsa_init(&me, 0);
    t_cose_signature_verify_eddsa_set_key(&me, key, NULL_Q_USEFUL_BUF_C);
    t_cose_signature_verify_eddsa_set_special_param_decoder(&me, NULL, NULL);
    t_cose_signature_verify_eddsa_set_auxiliary_buffer(&me, NULL_Q_USEFUL_BUF);
    n = t_cose_signature_verify_eddsa_auxiliary_buffer_size(&me);
    p = t_cose_signature_verify_from_eddsa(&me);
}
#endif /* !T_COSE_DISABLE_EDDSA */
#endif /* !T_COSE_MINIMUM */



#ifndef T_COSE_DISABLE_MAC0

#include "t_cose/t_cose_mac_compute.h"

void call_all_mac_compute(void)
{
    struct t_cose_mac_calculate_ctx context;
    struct t_cose_key               key;
    enum t_cose_err_t               err;

    t_cose_mac_compute_init(&context, 0, 0);
    t_cose_mac_set_computing_key(&context, key, NULL_Q_USEFUL_BUF_C);

#ifndef T_COSE_MINIMUM
    t_cose_mac_add_body_header_params(&context, NULL);
#endif /* !T_COSE_MINIMUM */

    err = t_cose_mac_compute(&context,
                             NULL_Q_USEFUL_BUF_C,
                             NULL_Q_USEFUL_BUF_C,
                             NULL_Q_USEFUL_BUF,
                             NULL);
#ifndef T_COSE_MINIMUM
    err = t_cose_mac_compute_detached(&context,
                                      NULL_Q_USEFUL_BUF_C,
                                      NULL_Q_USEFUL_BUF_C,
                                      NULL_Q_USEFUL_BUF,
                                      NULL);
#endif /* !T_COSE_MINIMUM */
}

#include "t_cose/t_cose_mac_validate.h"

void call_all_mac_validate(void)
{
    struct t_cose_mac_validate_ctx me;
    struct t_cose_key              key;
    enum t_cose_err_t              err;

    t_cose_mac_validate_init(&me, 0);
    t_cose_mac_set_validate_key(&me, key);
    err = t_cose_mac_validate(&me,
                              NULL_Q_USEFUL_BUF_C,
                              NULL_Q_USEFUL_BUF_C,
                              NULL,
                              NULL);
#ifndef T_COSE_MINIMUM
    err = t_cose_mac_validate_detached(&me,
                                       NULL_Q_USEFUL_BUF_C,
                                       NULL_Q_USEFUL_BUF_C,
                                       NULL_Q_USEFUL_BUF_C,
                                       NULL);
#endif /* !T_COSE_MINIMUM */
}

#endif /* !T_COSE_DISABLE_MAC0 */


#include "t_cose/t_cose_encrypt_enc.h"

void call_all_encrypt_enc(void)
{
    struct t_cose_encrypt_enc me;
    struct t_cose_key         key;
    enum t_cose_err_t         err;


    t_cose_encrypt_enc_init(&me, 0, 0);
    t_cose_encrypt_add_recipient(&me, NULL);
#ifndef T_COSE_MINIMUM
    t_cose_encrypt_enc_body_header_params(&me, NULL);
    t_cose_encrypt_set_cek(&me, key);
    t_cose_encrypt_set_enc_struct_buffer(&me, NULL_Q_USEFUL_BUF);
#endif /* !T_COSE_MINIMUM */

    err = t_cose_encrypt_enc(&me,
                             NULL_Q_USEFUL_BUF_C,
                             NULL_Q_USEFUL_BUF_C,
                             NULL_Q_USEFUL_BUF,
                             NULL);
#ifndef T_COSE_MINIMUM
    err = t_cose_encrypt_enc_detached(&me,
                                      NULL_Q_USEFUL_BUF_C,
                                      NULL_Q_USEFUL_BUF_C,
                                      NULL_Q_USEFUL_BUF,
                                      NULL_Q_USEFUL_BUF,
                                      NULL,
                                      NULL);
#endif /* !T_COSE_MINIMUM */

}


#include "t_cose/t_cose_encrypt_dec.h"

void call_all_encrypt_dec(void)
{
    struct t_cose_encrypt_dec_ctx me;
    struct t_cose_key             key;
    enum t_cose_err_t             err;

    t_cose_encrypt_dec_init(&me, 0);
    t_cose_encrypt_dec_add_recipient(&me, NULL);
#ifndef T_COSE_MINIMUM
    t_cose_encrypt_dec_set_cek(&me, key);
    t_cose_encrypt_add_param_storage(&me, NULL);
    t_cose_decrypt_set_enc_struct_buffer(&me, NULL_Q_USEFUL_BUF);
#endif /* !T_COSE_MINIMUM */

    err = t_cose_encrypt_dec(&me,
                             NULL_Q_USEFUL_BUF_C,
                             NULL_Q_USEFUL_BUF_C,
                             NULL_Q_USEFUL_BUF,
                             NULL,
                             NULL);
#ifndef T_COSE_MINIMUM
    err = t_cose_encrypt_dec_detached(&me,
                                      NULL_Q_USEFUL_BUF_C,
                                      NULL_Q_USEFUL_BUF_C,
                                      NULL_Q_USEFUL_BUF_C,
                                      NULL_Q_USEFUL_BUF,
                                      NULL,
                                      NULL);
#endif /* !T_COSE_MINIMUM */
}


#include "t_cose/t_cose_recipient_enc_keywrap.h"

void call_all_recipient_enc_keywrap(void)
{
    struct t_cose_recipient_enc_keywrap me;
    struct t_cose_key                   key;
    struct t_cose_alg_and_bits          ce_alg;
    

    t_cose_recipient_enc_keywrap_init(&me, 0);

    t_cose_recipient_enc_keywrap_set_key(&me, key, NULL_Q_USEFUL_BUF_C);

#ifndef T_COSE_MINIMUM
    t_cose_recipient_enc_add_params(&me, NULL);
#endif /* !T_COSE_MINIMUM */


    // TODO: why isn't this linked by reference in init?
    t_cose_recipient_create_keywrap_cb_private(NULL,
                                               NULL_Q_USEFUL_BUF_C,
                                               ce_alg,
                                               NULL);
}


#include "t_cose/t_cose_recipient_dec_keywrap.h"

void call_all_recipient_dec_aes_kw(void)
{
    struct t_cose_recipient_dec_keywrap me;
    struct t_cose_key                   key;
    struct t_cose_header_location       loc;
    struct t_cose_alg_and_bits          ce_alg;


    t_cose_recipient_dec_keywrap_init(&me);
    t_cose_recipient_dec_keywrap_set_kek(&me, key, NULL_Q_USEFUL_BUF_C);

    // TODO: why isn't this linked by reference in init?
    t_cose_recipient_dec_keywrap_cb_private(NULL,
                                            loc,
                                            ce_alg,
                                            NULL,
                                            NULL_Q_USEFUL_BUF,
                                            NULL,
                                            NULL,
                                            NULL);
}

#ifndef T_COSE_MINIMUM
#ifndef T_COSE_DISABLE_HPKE

#include "t_cose/t_cose_recipient_enc_hpke.h"

void call_all_recipient_enc_hpke(void)
{
    struct t_cose_recipient_enc_hpke me;
    struct t_cose_key                key;
    struct t_cose_alg_and_bits       ce_alg;


    t_cose_recipient_enc_hpke_init(&me, 0, 0, 0);

    t_cose_recipient_enc_hpke_set_key(&me, key, NULL_Q_USEFUL_BUF_C);


    // TODO: why isn't this linked by reference in init?
    t_cose_recipient_create_hpke_cb_private(NULL,
                                            NULL_Q_USEFUL_BUF_C,
                                            ce_alg,
                                            NULL);
}


#include "t_cose/t_cose_recipient_dec_hpke.h"

void call_all_recipient_dec_hpke(void)
{
    struct t_cose_recipient_dec_hpke    me;
    struct t_cose_key                   key;
    struct t_cose_header_location       loc;
    struct t_cose_alg_and_bits          ce_alg;


    t_cose_recipient_dec_hpke_init(&me);
    t_cose_recipient_dec_hpke_set_skr(&me, key, NULL_Q_USEFUL_BUF_C);

    // TODO: why isn't this linked by reference in init?
    t_cose_recipient_dec_hpke_cb_private(NULL,
                                         loc,
                                         ce_alg,
                                         NULL,
                                         NULL_Q_USEFUL_BUF,
                                         NULL,
                                         NULL,
                                         NULL);
}

#endif /* !T_COSE_DISABLE_HPKE */
#endif /* !T_COSE_MINIMUM */



void call_all_main(void)
{
    call_all_key();

    call_all_sign_sign();

    call_all_sign_verify();

#ifndef T_COSE_MINIMUM
    call_all_sign1_sign();

    call_all_sign1_verify();
#endif /* !T_COSE_MINIMUM */


    call_all_signature_sign_main();

    call_all_signature_verify_main();
#ifndef T_COSE_MINIMUM
#ifndef T_COSE_DISABLE_EDDSA
    call_all_signature_sign_eddsa();
#endif
#endif /* !T_COSE_MINIMUM */


#ifndef T_COSE_DISABLE_MAC0
    call_all_mac_compute();

    call_all_mac_validate();
#endif

    call_all_encrypt_enc();

    call_all_encrypt_dec();

    call_all_recipient_enc_keywrap();

    call_all_recipient_dec_aes_kw();

#ifndef T_COSE_DISABLE_HPKE
    call_all_recipient_enc_hpke();

    call_all_recipient_dec_hpke();
#endif /* ! T_COSE_DISABLE_HPKE */

}

int main(int argc, const char * argv[])
{
    (void)argc;
    (void)argv;

    call_all_main();
}


