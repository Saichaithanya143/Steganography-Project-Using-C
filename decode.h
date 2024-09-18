#define DECODE_H

#include "types.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* data */
    /* Decode Text File Info */
    char *decode_fname;
    FILE *fptr_decode;
    char extn_decode_file[MAX_FILE_SUFFIX];
    char decode_data[MAX_SECRET_BUF_SIZE];

    unsigned char decode_magic_string;
    int decode_size;
   /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} DecodeInfo;

/* Read and validate Decode args from argv */
StatusDecode read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
StatusDecode do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
StatusDecode open_files_decode(DecodeInfo *decInfo);

/*skip header*/
StatusDecode skip_header(FILE *fptr_skip_bytes);

/*decode magic string*/
StatusDecode decode_magicstring(int len,DecodeInfo *decInfo);

/*decode byte from lsb*/
StatusDecode lsb_to_byte(char *data,DecodeInfo *decInfo);

/*DECODE OUTPUT EXTENSION SIZE*/
StatusDecode decode_extn_size_of_output(DecodeInfo *decInfo);

//decode output extension
StatusDecode decode_output_extn(DecodeInfo *decInfo);

/*decode output file data*/
StatusDecode decode_output_file_data(DecodeInfo *decInfo);
