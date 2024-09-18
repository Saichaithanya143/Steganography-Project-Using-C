/*
NAME                 : LINGALA SAI CHAITHANYA
REG.NO               : 24008_082
DATE OF SUBMISSDION  : 16-08-2024
 */

#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc,char *argv[])
{
	EncodeInfo encInfo;
	DecodeInfo decInfo;

	if(argc>=2)
	{
		/*Check the operation is type is encode or decode*/
		int ret=check_operation_type(argv);

		/*If the operation type is encode start the encoding*/
		if(ret==e_encode)
		{
			printf("Selected encoding\n");

			/*For Execution of Encoding we pass atleast 4 command line arguments*/
			if(argc>=4 && argc<=5)
			{

				/*Check the passed Command line arguments  for encoding are correct or wrong*/
				if(read_and_validate_encode_args(argv,&encInfo)==e_success)
				{

					/*If the passed command line arguments are correct then start encoding the data*/
					if(do_encoding(&encInfo)==e_success)
					{
						printf("Encoding Successful\n");
					}

					/*If the passed command line arguments are wrong then the encoding will be failed*/
					else
					{
						printf("Encoding Failure\n");
					}
				}
				else
				{
					printf("read and validate for encoding is failure\n");
				}
			}
			else
			{
				printf("Please provide atleast 4 command line arguments for Encoding\n");
			}
		}

		/*If the operation type is decode start the decoding*/
		else if(ret==e_decode)
		{
			printf("Selected decoding\n");

			/*For Execution of Decoding we pass atleast 3 command line arguments*/
			if(argc>=3 && argc<=4) 
			{

				/*Check the passed Command line arguments  for decoding are correct or wrong*/
				if(read_and_validate_decode_args(argv,&decInfo)==e_success)
				{

					/*If the passed command line arguments are correct then start decoding the data*/
					if(do_decoding(&decInfo)==e_success)
					{
						printf("Decoding Successful\n");
					}

					/*If the passed command line arguments are wrong then the decoding will be failed*/
					else
					{
						printf("Decoding Failure\n");
					}
				}
				else
				{
					printf("read and validate for decoding is failure\n");

				}
			}
			else
			{
				printf("Please provide atleast 3 command line arguments for decoding\n");
			}
		}

		/*IF the operation type is neither encode nor decode then operation leads to unsupported format*/
		else
		{
			printf("Unsupported Format\n");
		}
	}
	else
	{
		printf("ERROR: ./a.out : INVALID ARGUMENTS\n");
		printf("USAGE : \n");
		printf("To encode please pass like: ./a.out -e beautiful.bmp secret.txt/encode.h/decode.h\n");
		printf("To decode please pass like: ./a.out -d output_file.bmp\n");
	}
}
/*Check Opeartion type*/
OperationType check_operation_type(char *argv[])
{
	/*Check if the argument vector of 1 is either a encode or a decode or a unsupported format*/

	/*If the argument vector of 1 is "-e" then the operation is selected for encoding*/
	if((strcmp(argv[1],"-e")==0))
	{
		return e_encode;
	}

	/*If the argument vector of 1 is "-d" then the operation is selected for decoding*/
	else if(strcmp(argv[1],"-d")==0)
	{
		return e_decode;
	}

	/*If the argument vector of 1 is neither "-e" nor "-d" then the operation leads to unsupported format*/
	else
	{
		return e_unsupported;
	}
}



