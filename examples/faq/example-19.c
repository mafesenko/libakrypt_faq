#include <stdio.h>
#include <libakrypt.h>
#include <libakrypt-base.h>

int main( void ) 
{
    if( ak_libakrypt_create( NULL ) != ak_true ) {
        /* инициализация выполнена не успешно, следовательно, выходим из программы */
        ak_libakrypt_destroy();
        return EXIT_FAILURE;
    }
    
    /* контекст алгоритма аутентифицированного шифрования */
    struct aead a;

    /* Создание контекста алгоритма аутентифицированного шифрования по заданному OID 
    (в данном случае - Магма) */
    if(ak_error_ok != ak_aead_create_oid(&a, ak_true, ak_oid_find_by_name("mgm-magma")))
        return ak_error_get_value();
        
    /* Возможные значения OID для aead шифрования:
    
    "mgm-magma"
    "mgm-kuznechik"
    "xtsmac-magma"
    "ctr-cmac-magma"
    "ctr-cmac-kuznechik"
    "ctr-hmac-magma-streebog256"
    "ctr-hmac-magma-streebog512"
    "ctr-hmac-kuznechik-streebog256"
    "ctr-hmac-kuznechik-streebog512"
    "ctr-nmac-magma"
    "ctr-nmac-kuznechik"
    
    Все OID аналогичны заданию функции напрямую */  
        
    /* константные значения ключей из ГОСТ Р 34.13-2015 */
    ak_uint8 authenticationKey[32] = { /* ключ из приложения А */
       0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe,
       0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88 };

    ak_uint8 encryptionKey[32] = { /* ключ из приложения Б */
       0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0,
       0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };

    /* Присвоение константных значений секретным ключам шифрования и аутентификации */
    if(ak_error_ok != ak_aead_set_keys(&a, /* Контекст алгоритма аутентифицированного шифрования */
    			encryptionKey,     /* Область памяти, в которой хранится значение ключа шифрования */
    			32,                /* Размер ключа шифрования (в октетах) */
    			authenticationKey, /* Область памяти, в которой хранится значение ключа аутентификации (имитозащиты) */
    			32                 /* Размер ключа аутентификации (в октетах) */
    			))
        return ak_error_get_value();


    /* тестовые данные */
    ak_uint8 packet[41 + 67] = {
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
        0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0xEA,
        /* зашифровываем с этого момента */
         0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11,
         0x0A, 0xFF, 0xEE, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,
         0x00, 0x0A, 0xFF, 0xEE, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11,
         0x11, 0x00, 0x0A, 0xFF, 0xEE, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22,
         0xCC, 0xBB, 0xAA };

    /* тестовые синхропосылки */
    ak_uint8 iv[32] = {
      0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11,
      0x59, 0x0a, 0x13, 0x3c, 0x6b, 0xf0, 0xde, 0x92, 0x21, 0x43, 0x65, 0x87, 0xa9, 0xcb, 0xed, 0x0f };

    ak_uint8 tag[64];
    memset(tag, 0, sizeof(tag));
    
    
    /* зашифрование данных с одновременным вычислением имитовставки */
    if (ak_error_ok != ak_aead_encrypt(
    			&a, /* контекст алгоритма аутентифицированного шифрования */
    			packet,      /* указатель на ассоциированные (незашифровываемые) данные */
    			41,          /* длина ассоциированных данных в октетах */
    			packet + 41, /* указатель на зашифровываемые данные */
    			packet + 41, /* указатель на зашифрованные данные */
    			67,          /* размер зашифровываемых данных в октетах */
    			iv,          /* указатель на синхропосылку */
    			a.iv_size,   /* длина синхропосылки в октетах */
    			tag,         /* указатель на область памяти, куда будет помещено значение имитовставки */
    			a.tag_size   /* ожидаемый размер имитовставки в байтах */
    			)) 
        return ak_error_get_value();
    
    ak_aead_destroy(&a);
    ak_libakrypt_destroy();
    return EXIT_SUCCESS;
}
