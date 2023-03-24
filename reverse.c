#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "wav.h"

bool check_format(WAVHEADER header);
int get_block_size(WAVHEADER header);
void reverse(int n, int m, BYTE samples[n][m]);

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

	// calculate number of audio samples
	int s_count = header.subchunk2Size / BLOCK_SIZE;

	// calculate length of each audio sample
	int s_length = header.bitsPerSample / 8;

	// allocate memory for samples
	BYTE (*samples)[s_length] = calloc(s_count, s_length * sizeof(BYTE));
	if (samples == NULL)
	{
		printf("Not enough memory to store audio samples.\n");
		fclose(outptr);
		fclose(inptr);
		return 5;
	}

	// iterate over infile's scanlines
	for (int i = 0; i < s_count; i++)
	{
		// read row into samples array
		fread(samples[i], sizeof(BYTE), s_length, inptr);
	}

	// reverse audio samples
	reverse(s_count, s_length, samples);

	// write new audio bits to outfile
	for (int i = 0; i < s_count; i++)
	{
		fwrite(samples[i], sizeof(BYTE), s_length, outptr);
	}

	// free memory for samples
	free(samples);

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

void reverse(int s_count, int s_length, BYTE samples[s_count][s_length])
{
	BYTE reverse_samples[s_count][s_length];

	// create a reversed copy of samples array and store it in reverse_samples
	for (int i = 0; i < s_count; i++)
	{
		for (int j = 0; j < s_length; j++)
		{
			reverse_samples[i][j] = samples[s_count - i - 1][s_length - j - 1];
		}
	}

	// copy reverse_samples[i][j] to samples[i][j]
	for (int i = 0; i < s_count; i++)
	{
		for (int j = 0; j < s_length; j++)
		{
			samples[i][j] = reverse_samples[i][j];
		}
	}
}
