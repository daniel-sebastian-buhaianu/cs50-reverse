#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "wav.h"

bool check_format(WAVHEADER header);
int get_block_size(WAVHEADER header);

int main(int argc, char *argv[])
{
	// Ensure proper usage
    	if (argc != 3)
	{
		printf("Usage: ./reverse [input.wav] [output.wav]\n");
		return 1;
	}

	// Remember filenames
	char *infile = argv[1];
	char *outfile = argv[2];

	// Open input file for reading
	FILE *inptr = fopen(infile, "r");
	if (inptr == NULL)
	{
		printf("Could not be open %s.\n", infile);
		return 2;
	}

	// Read header
	WAVHEADER header;
	fread(&header, sizeof(WAVHEADER), 1, inptr);

	// Use check_format to ensure WAV format
	if (!check_format(header))
	{
		printf("Input is not a WAV file.\n");
		return 3;
	}

	// Open output file for writing
	FILE *outptr = fopen(outfile, "w");
	if (outptr == NULL)
	{
		printf("Could not create %s.\n", outfile);
		return 4;
	}

	// Write header to file
	fwrite(&header, sizeof(WAVHEADER), 1, outptr);

	// Use get_block_size to calculate size of block
	int BLOCK_SIZE = get_block_size(header);

	// Alocate memory for an array of blocks of audio samples
	int count = 0;
	BYTE (*blocks)[BLOCK_SIZE] = calloc(count + 1, BLOCK_SIZE * sizeof(BYTE));

	// Read each block of BLOCK_SIZE
	while(fread(blocks[count], sizeof(BYTE), BLOCK_SIZE, inptr) == BLOCK_SIZE)
	{
		count++;
		blocks = realloc(blocks, (count + 1) * BLOCK_SIZE * sizeof(BYTE));
	}

	// Write reversed array to outfile
	for (int i = count - 1; i >= 0; i--)
	{
		fwrite(blocks[i], sizeof(BYTE), BLOCK_SIZE, outptr);
	}


	// free memory for blocks
	free(blocks);

	// close infile
	fclose(inptr);

	// close outfile
	fclose(outptr);

	// success
	return 0;

}

bool check_format(WAVHEADER header)
{
	if (strstr(header.format, "WAVE") == NULL)
	{
		return false;
	}
	return true;
}

int get_block_size(WAVHEADER header)
{
	return header.numChannels * header.bitsPerSample / 8;
}
