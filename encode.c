#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_image, 18, SEEK_SET);

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);
	printf("width = %u\n", width);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);
	printf("height = %u\n", height);

	// Return image capacity
	return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
	// Src Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
	// Do Error handling
	if (encInfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

		return e_failure;
	}

	// Secret file
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
	// Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}

	// Stego Image file
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	// Do Error handling
	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}

	// No failure return e_success
	return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{       
	/*Check if the argument vactor of 2 of passed file is a '.bmp' or not*/
	if(strstr(argv[2],".bmp")!=NULL)
	{
		/*If the passed file is a '.bmp' store it in a encoding src file*/
		encInfo->src_image_fname=argv[2];

		/*Store the argument vector of 3 of secret file into encoding secret file*/
		encInfo->secret_fname=argv[3];

		char *ptr=strstr(encInfo->secret_fname,".");

		/*Store the Extension of secret file*/
		strcpy(encInfo->extn_secret_file,ptr);
		printf("The address of exitension text file  after stored %s\n",encInfo->extn_secret_file);
	}
	else
	{
		/*If the passed file is not a '.bmp' return e_failure */
		return e_failure;
	}

	/*If the argument vector of 4 is not equal to NULL*/
	if(argv[4]!=NULL)
	{
		/*Check the passed file is a '.bmp' or not*/
		if(strstr(argv[4],".bmp")!=NULL)
		{
			/*If the passed file is a '.bmp' store it in a encoding output file*/
			encInfo->stego_image_fname=argv[4];
			printf("The output file name is \"%s\"\n",encInfo->stego_image_fname);
		}
		else
		{
			/*If the passed file is not a '.bmp' return e_failure*/
			return e_failure;
		}
	}

	/*If the argument vector of 4 is equal to NULL*/
	else
	{
		/*Assign one .bmp file into stego file*/
		encInfo->stego_image_fname="stego.bmp";
	}

	// No failure return e_success
	return e_success;

}

Status do_encoding(EncodeInfo *encInfo)
{

	/*Call All Functions Separately*/

	/*If any Function is fail return e_failure*/
	if(open_files(encInfo)==e_failure)
	{
		printf("Open file failure\n");
		return e_failure;
	}

	if(check_capacity(encInfo)==e_failure)
	{
		printf("Capacity not available\n");
		return e_failure;    
	}

	if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
	{
		printf("The bmp header is not copied successfully\n");
		return e_failure;
	}

	if(encode_magic_string(MAGIC_STRING,encInfo)==e_failure)
	{
		printf("The magic string is not encoded successfully\n");
		return e_failure;
	}

	if(encode_extern_size_data_to_image(encInfo,strlen(encInfo->extn_secret_file), encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
	{  
		printf("Encoding for Extention Size is Failed\n");
		return e_failure;
	}

	if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_failure)
	{
		printf("Extention encoding is failed\n");
		return e_failure;
	}

	if(encode_extern_size_data_to_image(encInfo,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
	{
		printf("Secret File Size encoding size is failed\n");
		return e_failure;
	}

	if(encode_secret_file_data(encInfo)==e_failure)
	{ 
		printf("The Secret File data is not encoded Successfully\n");
		return e_failure;
	}

	if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
	{
		printf("The ramaining data is not copied successfully\n");
		return e_failure;

	}

	// No failure return e_success
	return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
	printf("The size is for secret data is %ld\n",encInfo->size_secret_file);

	/*Check the Capacity of src file is greater than or equal or not*/
	if((get_image_size_for_bmp(encInfo->fptr_src_image))>=54+(strlen(MAGIC_STRING)+4+strlen(encInfo->extn_secret_file)+4+(get_file_size(encInfo->fptr_secret))*8))
	{
		/*If the Capacity of src file is greater than or equal to the given data return e_success*/
		printf("Capacity is Success\n");
		return e_success;
	}

	else
	{
		/*If the Capacity of src file is not greater than or equal to the given data return e_failure*/
		printf("Capacity is Failure\n");
		return e_failure;

	}
}

uint get_file_size(FILE *fptr)
{       
	/*Set the offset position of secret file at 0 and find the size of the secret file*/
	fseek(fptr,0,SEEK_END);
	printf("The offset position of fptr is->%ld\n",ftell(fptr));
	int store= ftell(fptr);
        printf("The store is %d\n",store);
	rewind(fptr);
	return store;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	/*Copy the header of the src image into a destination image*/
	char arr[54];
	fseek(fptr_src_image,0,SEEK_SET);
	fseek(fptr_dest_image,0,SEEK_SET);
	fread(arr,54,1,fptr_src_image);
	fwrite(arr,54,1,fptr_dest_image);
	printf("The value of source image header is %ld\n",ftell(fptr_src_image));
	printf("The value of destination image header is %ld\n",ftell(fptr_dest_image));

	if(ftell(fptr_src_image) ==54 && ftell(fptr_dest_image)==54)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}

}

/*Encode the Magic String*/
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	/*Find the length of the Magic string*/
	int size=strlen(magic_string);
	printf("The size of the magic string %d\n",size);

	/*Encode Magic String Data into stego image*/
	if(encode_data_to_image(MAGIC_STRING,size,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
	{
		printf("Magic String is successfully executed\n");
		// No failure return e_success
		return e_success;
	}
	else
	{       /*Encoding the Magic String is failed return e_failure*/
		return e_failure;
	}
}
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	printf("THe data is %s\n",data);
	printf("the data size is %d\n",size);
	char buffer[8];
	int i;
	for(i=0;i<size;i++)
	{
		//Read the 8 bytes from src image and store it in a buffer*/
		fread(buffer,8,1,fptr_src_image);

		/*Encode every byte into lsb*/
		encode_byte_to_lsb(data[i],buffer);

		/*Write the Encoded bytes into stego image*/
		fwrite(buffer,8,1,fptr_stego_image);
	}
	printf("data_to_image is successful\n");

	// No failure return e_success
	return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
	printf("The ASCII HEXA DECIMAL value is %x\n",data);
	int get;
	int i;
	int clear;
	int set;

	for(i=0;i<8;i++)
	{
		/*Get the byte from the data*/
		get=(data >> i) &1;

		/*Clear the byte from image buffer*/
		clear=image_buffer[i] & ~1;

		/*Set the byte*/
		set=clear | get;

		/*Store it in a image buffer*/
		image_buffer[i]=set;
		printf("The value after conversion %X\n",image_buffer[i]);
	}
}

/*Encode the secret file data size and secret file extension size*/
Status encode_extern_size_data_to_image(EncodeInfo *encInfo,int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{ 
	char buffer[32];

	/*Read the 32 bytes from the src image*/
	fread(buffer,32,1,fptr_src_image);

	/*Encode the size into lsb*/
	encode_extern_size(size,buffer);

	/*After Encoding write the 32 bytes into stego image*/
	fwrite(buffer,32,1,fptr_stego_image);
	printf("Encoding size is Successful\n");

	// No failure return e_success
	return e_success;
}

Status encode_extern_size(int size,char *image_buffer)
{
	printf("The ASCII HEXA DECIMAL value is %x\n",size);
	int get,clear,set;
	for(int i=0;i<32;i++)
	{ 
		/*Get the byte from the data*/
		get=(size >> i) & 1;

		/*Clear the byte from image buffer*/
		clear=image_buffer[i] & ~ 1;

		/*Set the byte*/
		set=clear | get;

		/*Store it in a image buffer*/
		image_buffer[i]=set;
		printf("The value after conversion %X\n",image_buffer[i]);
	}
}

/*Encode Secret File Extension*/
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{ 
	/*Find the length of the Extension of secret file*/
	int size=strlen(file_extn);
	printf("The size of the secret file extention is %d\n",size);

	/*Encode the Extension of secret file data into stego image*/
	if(encode_data_to_image(encInfo->extn_secret_file,size,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
	{
		printf("The Extention data is successfully\n");
		// No failure return e_success
		return e_success;
	}
	else
	{       
		/*Encoding of Extension of secret file is failed return e_failure*/
		return e_failure;
	}

}

/*Encoding of secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	/*Find the size of the secret file data*/
	int size=get_file_size(encInfo->fptr_secret);
	printf("The size of the secret file data is %d\n",size);

	/*Intialize one string*/
	char secret_data[size];
	secret_data[size] = '\0';

	/*Set offset position of the secret file data at 0*/
	fseek(encInfo->fptr_secret,0,SEEK_SET);

	/*Read all the data of secret file into that string*/
	fread(secret_data,size,1,encInfo->fptr_secret);
	printf("The secret file data is %s\n",secret_data);

	/*Encode the secret file data into stego image*/
	if(encode_data_to_image(secret_data,size,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
	{
		printf("The Secret File data is encoded successfully\n");
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

/*Copy the reamaining src image data into stego image*/
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	char ch;
	while(!feof(fptr_src))
	{
		ch=fgetc(fptr_src);
		fputc(ch,fptr_dest);
	} 
	fclose(fptr_src);
	fclose(fptr_dest);  
	return e_success;  
}
