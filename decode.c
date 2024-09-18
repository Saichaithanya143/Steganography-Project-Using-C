#include<stdio.h>
#include<string.h>
#include"types.h"
#include"decode.h"
#include"encode.h"
#include"common.h"

int flag=0;
int data_flag=0;
StatusDecode open_files_decode(DecodeInfo *decInfo)
{
	//stego image
	decInfo->fptr_stego_image=fopen(decInfo->stego_image_fname,"r");
	// Do Error handling
	if(decInfo->fptr_stego_image==NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

		return d_failure;
	}

	// No failure return e_success
	return d_success;
}

StatusDecode read_and_validate_decode_args(char *argv[], DecodeInfo * decInfo)
{
	/*Check if the argument vactor of 2 of passed file is a '.bmp' or not*/
	if(strstr(argv[2],".bmp")!=NULL)
	{
		/*If the passed file is a '.bmp' store it in a decoding stego file*/
		decInfo->stego_image_fname=argv[2];
		printf("The output file name is \"%s\"\n",decInfo->stego_image_fname);
	}
	else
	{
		/*If the passed file is not a '.bmp' return d_failure */
		return d_failure;
	}

	/*If the argument vector of 3 is not equal to NULL*/
	if(argv[3]!=NULL)
	{
		/*Store the passed file into a decoding file*/
		decInfo->decode_fname=argv[3];
	}
	else
	{
		/*If the argument vector of 3 is equal to NULL assign a one text file for decoding file*/
		decInfo->decode_fname="file";
		flag=1;
	}

	// No failure return d_success
	return d_success;
}
StatusDecode do_decoding(DecodeInfo *decInfo)
{
	/*Call All Functions Separately*/

	/*If any Function is fail return d_failure*/
	if(open_files_decode(decInfo)==d_failure)
	{
		printf("Open file failure\n");
		return d_failure;
	}
	if(skip_header(decInfo->fptr_stego_image)==d_failure)
	{ 
		return d_failure;
	}
	if(decode_magicstring(strlen(MAGIC_STRING),decInfo)==d_failure)
	{
		return d_failure;
	}
	if(decode_extn_size_of_output(decInfo)==d_failure)
	{
		return d_failure;
	}
	if(decode_output_extn(decInfo)==d_failure)
	{
		return d_failure;
	}        
	if(decode_extn_size_of_output(decInfo)==d_failure)
	{
		return d_failure;
	}
	if(decode_output_file_data(decInfo)==d_failure)
	{
		return d_failure;
	}
	// No failure return d_success
	return d_success;       

}

/*Skip the header in stego file*/
StatusDecode skip_header(FILE *fptr_skip_bytes)
{
	fseek(fptr_skip_bytes,54,SEEK_SET);
	printf("The offset position is %ld\n\n",ftell(fptr_skip_bytes));
	// No failure return d_success
	return d_success;
}

/*Decode the Magic String*/
StatusDecode decode_magicstring(int len,DecodeInfo *decInfo)
{
	char buffer[8];

	/*Run a loop upto length of the magic string and decode the data*/
	for(int i=0;i<len;i++)
	{
		/*Read the 8 bytes from stego image into buffer*/ 
		fread(buffer,8,1,decInfo->fptr_stego_image);

		/*Convert the lsb to byte*/
		lsb_to_byte(buffer,decInfo);

		//printf("The magic string value is %X\n",decInfo->decode_magic_string);

		/*Compare the decoded data with magic string*/
		if(decInfo->decode_magic_string!=MAGIC_STRING[i])
		{
			printf("The magic string comparing is failure\n");
			/*If the decoded data is not equal to magic string return d_failure*/
			return d_failure;
		}
		else
		{ 
			printf("The magic string comparing is success\n");
		}  
	}
	// No failure return d_success
	return d_success;         
}

StatusDecode lsb_to_byte(char *data,DecodeInfo *decInfo)
{
	int i;
	unsigned char value;
	int store=0;
	for(i=0;i<8;i++)
	{
		/*Get a bit*/
		store=(data[i]&1);

		/*Set the bits in a proper way*/
		value=value | (store<<i);
	}
	printf("The value is %X\n",value);
	decInfo->decode_magic_string=value;
        if(data_flag==1)
         {
               fwrite(&value,1,1,decInfo->fptr_decode);
          }
}  

int extern_size;
/*Decode extern size and secret file size*/
StatusDecode decode_extn_size_of_output(DecodeInfo *decInfo)
{
	char buffer[32];

	/*Read the 32 bytes from stego image into buffer*/
	fread(buffer,32,1,decInfo->fptr_stego_image);

	int res;
	int i=0;

	for(i=0;i<32;i++)
	{
                /*Get the bit*/
		res=(buffer[i] & 1);
                /*Set the bits in a proper way*/                
		extern_size=extern_size | (res<<i);
	}
	printf("The size of ext is %d\n",extern_size);

	// No failure return d_success
	return d_success;
}

/*Decode the extension of the text file*/
StatusDecode decode_output_extn(DecodeInfo *decInfo)
{       
	// decInfo->fptr_decode=fopen(decInfo->decode_fname,"w");
	int i;
	char buffer[8];
	char str[100];
	char *ptr=str;

	/*Run a loop upto extension size and decode the data*/
	for(i=0;i<extern_size;i++)
	{      
		/*Read the 8 bytes from stego image into buffer*/
		fread(buffer,8,1,decInfo->fptr_stego_image);

		/*convert lsb to byte*/
		lsb_to_byte(buffer,decInfo);
		ptr=ptr+sprintf(ptr,"%c",decInfo->decode_magic_string);

	}
	//  printf("The string is %s\n",str);
	if(flag==1)
	{
		char string[100];
		strcpy(string,decInfo->decode_fname);
		//printf("The string is %s\n",string);
		strcat(string,str);
		//printf("The string is %s\n",string);
		decInfo->decode_fname=string;
		printf("Concatenate string is %s",decInfo->decode_fname);
	}
	//open a text file 
	decInfo->fptr_decode=fopen(decInfo->decode_fname,"w");
        
        // Do Error handling
	if(decInfo->fptr_decode==NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decode_fname);

		return d_failure;
	}

	// No failure return d_success
	return d_success;
}

/*Decode the file data*/
StatusDecode decode_output_file_data(DecodeInfo *decInfo)
{
	int i;
	char ch;
	data_flag=1;
	char buffer[8];

	/*Run a loop upto file size data and decode*/
	for(i=0;i<extern_size;i++)
	{
		/*Read the 8 bytes from stego image into buffer*/
		fread(buffer,8,1,decInfo->fptr_stego_image);

		/*Convert lsb to byte*/
		lsb_to_byte(buffer,decInfo);
	}

	// No failure return e_success
	return d_success;  
}
