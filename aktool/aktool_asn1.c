/* ----------------------------------------------------------------------------------------------- */
/*  Copyright (c) 2018 - 2021 by Axel Kenzo, axelkenzo@mail.ru                                     */
/*                                                                                                 */
/*  Прикладной модуль, реализующий вывод информации о содержании asn.1 структур                    */
/*                                                                                                 */
/*  aktool_asn1.c                                                                                  */
/* ----------------------------------------------------------------------------------------------- */
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <aktool.h>

/* ----------------------------------------------------------------------------------------------- */
 int aktool_asn1_help( void );
 int aktool_asn1_print( int argc, tchar *argv[] );
 int aktool_asn1_convert( int argc, tchar *argv[],
                                 char *outname, export_format_t format, crypto_content_t content );
 int aktool_asn1_split( int argc, tchar *argv[], export_format_t format, crypto_content_t content );

/* ----------------------------------------------------------------------------------------------- */
 int aktool_asn1( int argc, tchar *argv[] )
{
  char *outname = NULL;
  int next_option = 0, exitcode = EXIT_SUCCESS;
  enum { do_print, do_convert, do_split, do_join } work = do_print;
  export_format_t format = asn1_der_format;
  crypto_content_t content = undefined_content;

  const struct option long_options[] = {
    /* сначала уникальные */
     { "convert",          0, NULL, 255 },
     { "split",            0, NULL, 254 },
     { "join",             0, NULL, 253 },
     { "to",               1, NULL, 250 },
     { "pem",              1, NULL, 249 },
     { "output",           1, NULL, 'o' },

    /* потом общие */
      aktool_common_functions_definition,
     { NULL,               0, NULL,   0  },
  };

 /* разбираем опции командной строки */
  do {
       next_option = getopt_long( argc, argv, "o:", long_options, NULL );
       switch( next_option )
      {
       /* сначала обработка стандартных опций */
        aktool_common_functions_run( aktool_asn1_help );

       /* теперь опции, уникальные для asn1parse */
         case 255 :  work = do_convert;
                     break;
         case 254 :  work = do_split;
                     break;
         case 253 :  work = do_join;
                     break;

       /* определяем формат выходных данных (--to) */
         case 250 :  if(( strncmp( optarg, "der", 3 ) == 0 ) || ( strncmp( optarg, "DER", 3 ) == 0 ))
                       format = asn1_der_format;
                      else
                       if(( strncmp( optarg, "pem", 3 ) == 0 ) || ( strncmp( optarg, "PEM", 3 ) == 0 ))
                         format = asn1_pem_format;
                        else {
                          fprintf( stdout, "error:\t%s is not valid format of output data\n", optarg );
                          return EXIT_FAILURE;
                        }
                     break;

       /* определяем тип pem-контейнера */
         case 249 :  if( strncmp( optarg, "certificate", 7 ) == 0 ) {
                       content = public_key_certificate_content;
                       break;
                     }
                     if( strncmp( optarg, "request", 7 ) == 0 ) {
                       content = public_key_request_content;
                       break;
                     }
                     if( strncmp( optarg, "symkey", 6 ) == 0 ) {
                       content = symmetric_key_content;
                       break;
                     }
                     if( strncmp( optarg, "secretkey", 9 ) == 0 ) {
                       content = secret_key_content;
                       break;
                     }
                     if( strncmp( optarg, "encrypted", 9 ) == 0 ) {
                       content = encrypted_content;
                       break;
                     }
                     if( strncmp( optarg, "plain", 5 ) == 0 ) {
                       content = plain_content;
                       break;
                     }
                     break;

       /* определяем имя выходного файла */
         case 'o' :  outname = optarg;
                     break;


       /* обрабатываем ошибочные параметры */
         default:
                     break;
       }
  } while( next_option != -1 );

 /* если параметры определены некорректно, то выходим  */
  if( argc < 3 ) return aktool_asn1_help();

 /* начинаем работу с криптографическими примитивами */
 /* начинаем работу с криптографическими примитивами */
   if( !aktool_create_libakrypt( )) return EXIT_FAILURE;

  switch( work ) {
   case do_print:
       exitcode = aktool_asn1_print( argc, argv );
       break;
   case do_convert:
       exitcode = aktool_asn1_convert( argc, argv, outname, format, content );
       break;
   case do_split:
       exitcode = aktool_asn1_split( argc, argv, format, content );
       break;

   default:
       break;
  }

 /* завершаем работы с библиотекой */
  aktool_destroy_libakrypt();

  if( exitcode ) {
    fprintf( stdout,
            _("aktool found %d error(s), rerun aktool with \"--audit stderr\" option\n"), exitcode );
    return EXIT_FAILURE;
  }

 return EXIT_SUCCESS;
}

/* ----------------------------------------------------------------------------------------------- */
 int aktool_asn1_print( int argc, tchar *argv[] )
{
  int idx = 0, error = ak_error_ok, ecount = 0;

  for( idx = 2; idx < argc; idx++ ) {
     if( ak_file_or_directory( argv[idx] ) == DT_REG ) {
       if(( error = ak_libakrypt_print_asn1( argv[idx], stdout )) != ak_error_ok ) {
         fprintf( stdout, _("file %s is wrong\n"), argv[idx] );
         ecount++;
       }
     }
  }

 return ecount;
}

/* ----------------------------------------------------------------------------------------------- */
 int aktool_asn1_convert( int argc, tchar *argv[],
                                  char *outname, export_format_t format, crypto_content_t content )
{
  size_t sl = 0;
  int idx = 0, error = ak_error_ok, ecount = 0;

  for( idx = 2; idx < argc; idx++ ) {
     if( ak_file_or_directory( argv[idx] ) == DT_REG ) {
        char name[FILENAME_MAX];

       /* 1. вырабатываем имя выходного файла */
        memset( name, 0, sizeof( name ));
        if( outname ) strncpy( name, outname, sizeof(name)-1 );
         else {
               strncpy( name, argv[idx], sizeof(name)-5 );
               sl = strlen( name );
               if( format == asn1_der_format ) memcpy( name+sl, ".der", 4 );
                else memcpy( name+sl, ".pem", 4 );
              }

       /* 2. если формат pem и тип не определен, надо бы потестировать */

       /* 3. конвертируем данные */
        if(( error = ak_libakrypt_convert_asn1( argv[idx], name, format, content )) != ak_error_ok )
        {
          fprintf( stdout, _("convertation of %s is wrong\n"), argv[idx] );
          ecount++;
        } else {
            if(( error = ak_libakrypt_print_asn1( name, stdout )) == ak_error_ok )
              fprintf( stdout, _("convertation of %s to %s is Ok\n"), argv[idx], name );
             else {
               fprintf( stdout, _("convertation of %s is wrong\n"), argv[idx] );
               ecount++;
             }
          }
     }
  }

 return ecount;
}

/* ----------------------------------------------------------------------------------------------- */
 int aktool_asn1_split( int argc, tchar *argv[], export_format_t format, crypto_content_t content )
{
  int idx = 0, error = ak_error_ok, ecount = 0;

  for( idx = 2; idx < argc; idx++ ) {
     if( ak_file_or_directory( argv[idx] ) == DT_REG ) {
       if(( error = ak_libakrypt_split_asn1( argv[idx], format, content )) != ak_error_ok ) {
         fprintf( stdout, _("file %s is wrong\n"), argv[idx] );
         ecount++;
       }
     }
  }

 return ecount;
}


/* ----------------------------------------------------------------------------------------------- */
 int aktool_asn1_help( void )
{
  printf(
   _("aktool asn1parse [options] [files] - decode and print ASN.1 data\n"
     "usage:\n"
     "  aktool a file - print ASN.1 data stored in DER or PEM format\n\n"
     "available options:\n"
     "     --convert           print and convert file to specified format\n"
     " -o, --output <file>     set the name of output file\n"
     "     --pem <content>     use the specified informational string of pem content\n"
     "                         [ enabled values: certificate, request, symkey, secretkey, encrypted, plain ]\n"
     "     --split             split ASN.1 tree into separate leaves\n"
     "     --to <format>       set the format of output file [ enabled values : der, pem ]\n"
  ));

 return aktool_print_common_options();
}

