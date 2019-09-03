/* ----------------------------------------------------------------------------------------------- */
/*  Copyright (c) 2014 - 2019 by Axel Kenzo, axelkenzo@mail.ru                                     */
/*                                                                                                 */
/*  Файл ak_random.с                                                                               */
/*  - содержит реализацию генераторов псевдо-случайных чисел                                       */
/* ----------------------------------------------------------------------------------------------- */
#ifdef LIBAKRYPT_HAVE_STDLIB_H
 #include <stdlib.h>
#else
 #error Library cannot be compiled without stdlib.h header
#endif
#ifdef LIBAKRYPT_HAVE_STRING_H
 #include <string.h>
#else
 #error Library cannot be compiled without string.h header
#endif
#ifdef LIBAKRYPT_HAVE_TIME_H
 #include <time.h>
#endif
#ifdef LIBAKRYPT_HAVE_UNISTD_H
 #include <unistd.h>
#endif
#ifdef LIBAKRYPT_HAVE_FCNTL_H
 #include <fcntl.h>
#endif

/* ----------------------------------------------------------------------------------------------- */
 #include <ak_random.h>

/* ----------------------------------------------------------------------------------------------- */
/*! Функция устанавливает значение полей структуры struct random в значения по-умолчанию.

    @param rnd указатель на структуру struct random
    @return В случае успеха возвращается ak_error_ok (ноль). В случае возникновения ошибки
    возвращается ее код.                                                                           */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create( ak_random rnd )
{
  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                      "use a null pointer to a random generator" );
  rnd->oid = NULL;
  rnd->next = NULL;
  rnd->randomize_ptr = NULL;
  rnd->random = NULL;
  rnd->free = NULL;
  memset( &rnd->data, 0, sizeof( rnd->data ));

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param rnd указатель на структуру struct random
    @return В случае успеха возвращается ak_error_ok (ноль). В случае возникновения ошибки
    возвращается ее код.                                                                           */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_destroy( ak_random rnd )
{
  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                      "use a null pointer to a random generator" );
  if( rnd->free != NULL ) rnd->free( rnd );
  rnd->oid = NULL;
  rnd->next = NULL;
  rnd->randomize_ptr = NULL;
  rnd->random = NULL;
  memset( &rnd->data, 0, sizeof( rnd->data ));

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция очищает все внутренние поля, уничтожает генератор псевдо-случайных чисел
    (структуру struct random) и присваивает указателю значение NULL.

    @param rnd указатель на структуру struct random.
    @return В случае успеха возвращается ak_error_ok (ноль). В случае возникновения ошибки
    возвращается ее код.                                                                           */
/* ----------------------------------------------------------------------------------------------- */
 ak_pointer ak_random_context_delete( ak_pointer rnd )
{
  if( rnd != NULL ) {
   ak_random_context_destroy(( ak_random ) rnd );
   free( rnd );
  } else ak_error_message( ak_error_null_pointer, __func__ ,
                                            "use a null pointer to a random generator" );
  return NULL;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Установка происходит путем вызова функции-члена класса random,
    которая и реализует механизм инициализации.

    @param rnd контекст генератора псевдо-случайных чисел.
    @param in указатель на данные, с помощью которых инициализируется генератор.
    @param size размер данных, в байтах.

    @return В случае успеха функция возвращает \ref ak_error_ok. В противном случае
    возвращается код ошибки.                                                                       */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_randomize( ak_random rnd, const ak_pointer in, const ssize_t size )
{
 if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                        "use a null pointer to random generator" );
 if( in == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                           "use a null pointer to initializator" );
 if( size <= 0 ) return ak_error_message( ak_error_wrong_length, __func__,
                                                              "using a buffer with wrong length" );
 if( rnd->randomize_ptr == NULL ) return ak_error_message( ak_error_undefined_function, __func__,
                                           "randomize() function not defined for this generator" );
 return rnd->randomize_ptr( rnd, in, size );
}

/* ----------------------------------------------------------------------------------------------- */
/*! Выработка последовательности псведо-случайных данных происходит путем
    вызова функции-члена класса random.

    @param rnd контекст генератора псевдо-случайных чисел.
    @param out указатель на область памяти, в которую помещаются псевдо-случайные данные.
    @param size размер помещаемых данных, в байтах.

    @return В случае успеха функция возвращает \ref ak_error_ok. В противном случае
    возвращается код ошибки.                                                                       */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_random( ak_random rnd, const ak_pointer out, const ssize_t size )
{
 if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                        "use a null pointer to random generator" );
 if( out == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                             "use a null pointer to output data" );
 if( size <= 0 ) return ak_error_message( ak_error_wrong_length, __func__,
                                                              "using a buffer with wrong length" );
 if( rnd->random == NULL ) return ak_error_message( ak_error_undefined_function, __func__,
                                                "this generator has undefined random() function" );
 return rnd->random( rnd, out, size );
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param rnd указатель на контекст генератора псевдо-случайных чисел
    @param oid OID генератора.

    @return В случае успеха возвращается ak_error_ok (ноль). В случае возникновения ошибки
    возвращается ее код.                                                                           */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_oid( ak_random rnd, ak_oid oid )
{
  int error = ak_error_ok;

 /* выполняем проверку */
  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                             "using null pointer to random generator context" );
  if( oid == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                 "using null pointer to random generator OID" );
 /* проверяем, что OID от того, что нужно */
  if( oid->engine != random_generator )
    return ak_error_message( ak_error_oid_engine, __func__ , "using oid with wrong engine" );
 /* проверяем, что OID от алгоритма, а не от каких-то там параметров  */
  if( oid->mode != algorithm )
    return ak_error_message( ak_error_oid_mode, __func__ , "using oid with wrong mode" );
 /* проверяем, что производящая функция определена */
  if( oid->func.create == NULL )
    return ak_error_message( ak_error_undefined_function, __func__ ,
                                                       "using oid with undefined constructor" );
 /* инициализируем контекст */
  if(( error = (( ak_function_random * )oid->func.create )( rnd )) != ak_error_ok )
      return ak_error_message( error, __func__, "invalid creation of random generator context");

 return error;
}

/* ----------------------------------------------------------------------------------------------- */
  static ak_uint64 shift_value = 0; // Внутренняя статическая переменная (счетчик вызовов)

/* ----------------------------------------------------------------------------------------------- */
/*! Функция использует для генерации случайного значения текущее время, номер процесса и
    номер пользователя.
    Несмотря на кажущуюся случайность вырабатываемого значения, функция не должна использоваться для
    генерации значений, для которых требуется криптографическая случайность. Это связано с
    достаточно прогнозируемым изменением значений функции при многократных повторных вызовах.

    Основная задача данной функции - инициализаци программного генератора
    каким-либо знаением, в случае, когда пользователь не инициализирует программный генератор
    самостоятельно.

   \return Функция возвращает случайное число размером 8 байт (64 бита).                                                     */
/* ----------------------------------------------------------------------------------------------- */
 ak_uint64 ak_random_value( void )
{
  ak_uint64 vtme = 17, clk = 23, value = 1;
#ifndef _WIN32
  ak_uint64 pval = ( ak_uint64 ) getpid();
  ak_uint64 uval = ( ak_uint64 ) getuid();
#else
  ak_uint64 pval = _getpid();
  ak_uint64 uval = 67;
#endif

#ifdef LIBAKRYPT_HAVE_TIME_H
  vtme = ( ak_uint64) time( NULL );
  clk = ( ak_uint64 ) clock();
#endif

  value = ( shift_value += 11 )*125643267795740073ULL + pval;
  value = ( value * 506098983240188723ULL ) + 71331*uval + vtme;
 return value ^ clk;
}

/* ----------------------------------------------------------------------------------------------- */
/*                                 реализация класса rng_lcg                                       */
/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_lcg_next( ak_random rnd )
{
  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                      "use a null pointer to a random generator" );
  rnd->data.val *= 125643267795740073ULL;
  rnd->data.val += 506098983240188723ULL;

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_lcg_randomize_ptr( ak_random rnd, const ak_pointer ptr, const ssize_t size )
{
  ssize_t idx = 0;
  ak_uint8 *value = ptr;

  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                      "use a null pointer to a random generator" );
  if( ptr == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                          "use a null pointer to initial vector" );
  if( size <= 0 ) return ak_error_message( ak_error_wrong_length, __func__ ,
                                                          "use initial vector with wrong length" );
 /* сначала начальное значение, потом цикл по всем элементам массива */
  rnd->data.val = value[idx];
  do {
        rnd->next( rnd );
        rnd->data.val += value[idx];
  } while( ++idx < size );

 return rnd->next( rnd );
}

/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_lcg_random( ak_random rnd, const ak_pointer ptr, const ssize_t size )
{
  ssize_t idx = 0;
  ak_uint8 *value = ptr;

  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                      "use a null pointer to a random generator" );
  if( ptr == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                                    "use a null pointer to data" );
  if( size <= 0 ) return ak_error_message( ak_error_wrong_length, __func__ ,
                                                           "use a data vector with wrong length" );
  lab_start:
    value[idx] = (ak_uint8) ( rnd->data.val >> 16 );
    rnd->next( rnd );
    if( ++idx < size ) goto lab_start;

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Генератор вырабатывает последовательность внутренних состояний, удовлетворяющую
    линейному сравнению \f$ x_{n+1} \equiv a\cdot x_n + c \pmod{2^{64}}, \f$
    в котором константы a и c удовлетворяют равенствам
    \f$ a = 125643267795740073 \f$ и \f$ b = 506098983240188723. \f$

    Далее, последовательность внутренних состояний преобразуется в последовательность
    байт по следующему правилу
    \f$ \gamma_n = \displaystyle\frac{x_n - \hat x_n}{2^{24}} \pmod{256}, \f$
    где \f$\hat x_n \equiv x_n \pmod{2^{24}}. \f$

    @param generator Контекст создаваемого генератора.
    \return В случае успеха, функция возвращает \ref ak_error_ok. В противном случае
            возвращается код ошибки.                                                               */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_lcg( ak_random generator )
{
  int error = ak_error_ok;
  ak_uint64 qword = ak_random_value(); /* вырабатываем случайное число */

  if(( error = ak_random_context_create( generator )) != ak_error_ok )
    return ak_error_message( error, __func__ , "wrong initialization of random generator" );

  generator->oid = ak_oid_context_find_by_name("lcg");
  generator->next = ak_random_lcg_next;
  generator->randomize_ptr = ak_random_lcg_randomize_ptr;
  generator->random = ak_random_lcg_random;

 /* для корректной работы присваиваем какое-то случайное начальное значение */
  ak_random_lcg_randomize_ptr( generator, &qword, sizeof( ak_uint64 ));
 return error;
}

/* ----------------------------------------------------------------------------------------------- */
/*                              реализация класса rng_xorshift32                                   */
/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_xorshift32_next( ak_random rnd )
{
  ak_uint32 x = 0;
  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                      "use a null pointer to a random generator" );
  x = rnd->data.value;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  rnd->data.value = x;

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_xorshift32_randomize_ptr( ak_random rnd,
                                                        const ak_pointer ptr, const ssize_t size )
{
  ssize_t idx = 0;
  ak_uint8 *value = ptr;

  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                      "use a null pointer to a random generator" );
  if( ptr == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                          "use a null pointer to initial vector" );
  if( size <= 0 ) return ak_error_message( ak_error_wrong_length, __func__ ,
                                                          "use initial vector with wrong length" );
  /* сначала начальное значение, потом цикл по всем элементам массива */
  rnd->data.value = ak_max( value[idx], 1 );
  do {
       rnd->next( rnd );
       rnd->data.value += value[idx];
  } while( ++idx < size );
  if( rnd->data.value == 0 ) rnd->data.value = 0x42134ea1;

 return rnd->next( rnd );
}

/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_xorshift32_random( ak_random rnd, const ak_pointer ptr, const ssize_t size )
{
  ak_uint32 *arr = (ak_uint32 *) ptr;
  ssize_t i = 0, blocks = ( size >> 2 ), tail = size - ( blocks << 2 );

  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                      "use a null pointer to a random generator" );
  if( ptr == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                                    "use a null pointer to data" );
  if( size <= 0 ) return ak_error_message( ak_error_wrong_length, __func__ ,
                                                           "use a data vector with wrong length" );
 /* сначала заполняем блоками по 8 байт */
  for( i = 0; i < blocks; i++ ) {
     arr[i] = rnd->data.value;
     rnd->next( rnd );
  }
 /* потом остаток из младших разрядов */
  if( tail > 0 ) {
    memcpy( (ak_uint8 *)arr + blocks, &rnd->data.value, ( size_t )tail );
    rnd->next( rnd );
  }

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Генератор вырабатывает последовательность внутренних состояний, удовлетворяющую ...

    @param generator Контекст создаваемого генератора.
    \return В случае успеха, функция возвращает \ref ak_error_ok. В противном случае
            возвращается код ошибки.                                                               */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_xorshift32( ak_random generator )
{
  int error = ak_error_ok;
  ak_uint64 qword = ak_random_value(); /* вырабатываем случайное число */

  if(( error = ak_random_context_create( generator )) != ak_error_ok )
    return ak_error_message( error, __func__ , "wrong initialization of random generator" );

  generator->oid = ak_oid_context_find_by_name("xorshift32");
  generator->next = ak_random_xorshift32_next;
  generator->randomize_ptr = ak_random_xorshift32_randomize_ptr;
  generator->random = ak_random_xorshift32_random;

 /* для корректной работы присваиваем какое-то случайное начальное значение */
  ak_random_xorshift32_randomize_ptr( generator, &qword, sizeof( ak_uint64 ));
 return error;
}

/* ----------------------------------------------------------------------------------------------- */
/*                                 реализация класса rng_file                                      */
/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_context_random_file( ak_random rnd, const ak_pointer ptr, const ssize_t size )
{
  ak_uint8 *value = ptr;
  ssize_t result = 0, count = size;

  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                     "use a null pointer to a random generator" );
  if( ptr == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                                   "use a null pointer to data" );
  if( size <= 0 ) return ak_error_message( ak_error_zero_length, __func__ ,
                                                                 "use a data with wrong length" );
  /* считываем несколько байт */
  slabel: result = read( rnd->data.fd, value,
  #ifdef _MSC_VER
    (unsigned int)
  #else
    (size_t)
  #endif
    count );

  /* если конец файла, то переходим в начало */
  if( result == 0 ) {
    lseek( rnd->data.fd, 0, SEEK_SET );
    goto slabel;
  }
  /* если мы считали меньше, чем надо */
  if( result < count ) {
    value += result;
    count -= result;
    goto slabel;
  }
  /* если ошибка чтения, то возбуждаем ошибку */
  if( result == -1 ) return ak_error_message( ak_error_read_data, __func__ ,
                                                                 "wrong reading data from file" );
 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_free_file( ak_random rnd )
{
  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                     "use a null pointer to a random generator" );
  if( close( rnd->data.fd ) == -1 )
    ak_error_message( ak_error_close_file, __func__ , "wrong closing a file with random data" );

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Данный генератор связывается с заданным файлом и возвращает содержащиеся в нем значения
    в качестве случайных чисел. Если данные в файле заканчиваются, то считывание начинается
    с начала файла.

    Основное назначение данного генератора - считывание данных из файловых устройств,
    таких как /dev/randon или /dev/urandom.

    @param generator Контекст создаваемого генератора.
    @param filename Имя файла.
    \return В случае успеха, функция возвращает \ref ak_error_ok. В противном случае
            возвращается код ошибки.                                                               */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_file( ak_random rnd, const char *filename )
{
  int error = ak_error_ok;
  if(( error = ak_random_context_create( rnd )) != ak_error_ok )
    return ak_error_message( error, __func__ , "wrong initialization of random generator" );

 /* теперь мы открываем заданный пользователем файл */
  if(( rnd->data.fd = open( filename, O_RDONLY | O_BINARY )) == -1 ) {
    ak_error_message_fmt( ak_error_open_file, __func__ ,
                                  "wrong opening a file \"%s\" with random data", filename );
    ak_random_context_destroy( rnd );
    return ak_error_open_file;
  }

  // для данного генератора oid не определен
  rnd->next = NULL;
  rnd->randomize_ptr = NULL;
  rnd->random = ak_random_context_random_file;
  rnd->free = ak_random_context_free_file;

 return error;
}

#if defined(__unix__) || defined(__APPLE__)
/* ----------------------------------------------------------------------------------------------- */
/*! @param generator Контекст создаваемого генератора.
    \return В случае успеха, функция возвращает \ref ak_error_ok. В противном случае
            возвращается код ошибки.                                                               */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_random( ak_random generator )
{
 int result = ak_random_context_create_file( generator, "/dev/random" );
  if( result == ak_error_ok ) generator->oid = ak_oid_context_find_by_name("dev-random");
 return result;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param generator Контекст создаваемого генератора.
    \return В случае успеха, функция возвращает \ref ak_error_ok. В противном случае
            возвращается код ошибки.                                                               */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_urandom( ak_random generator )
{
 int result = ak_random_context_create_file( generator, "/dev/urandom" );
  if( result == ak_error_ok ) generator->oid = ak_oid_context_find_by_name("dev-urandom");
 return result;
}
#endif

/* ----------------------------------------------------------------------------------------------- */
/*                                 реализация класса rng_winrtl                                    */
/* ----------------------------------------------------------------------------------------------- */
#ifdef _WIN32
 static int ak_random_context_random_winrtl( ak_random rnd, const ak_pointer ptr, const ssize_t size )
{
  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                      "use a null pointer to a random generator" );
  if( ptr == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                                    "use a null pointer to data" );
  if( size <= 0 ) return ak_error_message( ak_error_wrong_length, __func__ ,
                                                           "use a data vector with wrong length" );

  if( !CryptGenRandom( rnd->data.handle, (DWORD) size, ptr ))
    return ak_error_message( ak_error_undefined_value, __func__,
                                                    "wrong generation of pseudo random sequence" );
 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_context_free_winrtl( ak_random rnd )
{
  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                                "freeing a null pointer to data" );
  if( !CryptReleaseContext( rnd->data.handle, 0 )) {
    return ak_error_message_fmt( ak_error_close_file,
            __func__ , "wrong closing a system crypto provider with error: %x", GetLastError( ));
  }

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_winrtl( ak_random generator )
{
  HCRYPTPROV handle = 0;

  int error = ak_error_ok;
  if(( error = ak_random_context_create( generator )) != ak_error_ok )
    return ak_error_message( error, __func__ , "wrong initialization of random generator" );

  /* теперь мы открываем криптопровайдер для доступа к генерации случайных значений
     в начале мы пытаемся создать новый ключ */
  if( !CryptAcquireContext( &handle, NULL, NULL,
                                         PROV_RSA_FULL, CRYPT_NEWKEYSET )) {
    /* здесь нам не удалось создать ключ, поэтому мы пытаемся его открыть */
    if( GetLastError() == NTE_EXISTS ) {
      if( !CryptAcquireContext( &handle, NULL, NULL,
                                            PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT )) {
        ak_error_message_fmt( error = ak_error_open_file, __func__,
              "wrong open default key for system crypto provider with error: %x", GetLastError( ));
       ak_random_context_destroy( generator );
       return error;
      }
    } else {
       ak_error_message_fmt( error = ak_error_access_file, __func__,
                    "wrong creation of default key for system crypto provider with error: %x",
                                                                                  GetLastError( ));
       ak_random_context_destroy( generator );
       return error;
     }
  }

  generator->data.handle = handle;
  generator->oid = ak_oid_context_find_by_name("winrtl");
  generator->next = NULL;
  generator->randomize_ptr = NULL;
  generator->random = ak_random_context_random_winrtl;

 /* эта функция должна закрыть открытый ранее криптопровайдер */
  generator->free = ak_random_context_free_winrtl;
 return error;
}

#endif

/* ----------------------------------------------------------------------------------------------- */
/*!  \example test-random01.c                                                                      */
/*!  \example test-random02.c                                                                      */
/*!  \example test-random03.c                                                                      */
/* ----------------------------------------------------------------------------------------------- */
/*                                                                                    ak_random.c  */
/* ----------------------------------------------------------------------------------------------- */
