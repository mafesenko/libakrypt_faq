/* ----------------------------------------------------------------------------------------------- */
/*  Copyright (c) 2014 - 2019 by Axel Kenzo, axelkenzo@mail.ru                                     */
/*                                                                                                 */
/*  Файл ak_gf2n.h                                                                                 */
/*  - содержит описание функций умножения элементов конечных полей характеристики 2.               */
/* ----------------------------------------------------------------------------------------------- */
#ifndef    __AK_GF2N_H__
#define    __AK_GF2N_H__

/* ----------------------------------------------------------------------------------------------- */
 #include <libakrypt.h>

/* ----------------------------------------------------------------------------------------------- */
/*! \brief Умножение двух элементов поля \f$ \mathbb F_{2^{64}}\f$. */
 void ak_gf64_mul_uint64( ak_pointer z, ak_pointer x, ak_pointer y );
/*! \brief Умножение двух элементов поля \f$ \mathbb F_{2^{128}}\f$. */
 void ak_gf128_mul_uint64( ak_pointer z, ak_pointer x, ak_pointer y );

#ifdef LIBAKRYPT_HAVE_BUILTIN_CLMULEPI64
/*! \brief Умножение двух элементов поля \f$ \mathbb F_{2^{64}}\f$. */
 void ak_gf64_mul_pcmulqdq( ak_pointer z, ak_pointer x, ak_pointer y );
/*! \brief Умножение двух элементов поля \f$ \mathbb F_{2^{128}}\f$. */
 void ak_gf128_mul_pcmulqdq( ak_pointer z, ak_pointer a, ak_pointer b );

 #define ak_gf64_mul ak_gf64_mul_pcmulqdq
 #define ak_gf128_mul ak_gf128_mul_pcmulqdq
#else
 #define ak_gf64_mul ak_gf64_mul_uint64
 #define ak_gf128_mul ak_gf128_mul_uint64
#endif

/*! \brief Функция тестирования корректности реализации операций умножения в полях характеристики 2. */
 bool_t ak_gfn_multiplication_test( void );


/* ----------------------------------------------------------------------------------------------- */
#endif

/* ----------------------------------------------------------------------------------------------- */
/*                                                                                      ak_gf2n.h  */
/* ----------------------------------------------------------------------------------------------- */
