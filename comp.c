#include <stdio.h>
#include <string.h>

//comp()
void Compress(unsigned char *szOut, const char *szMessage) {
	unsigned long long nBuffer = 0; //This is enough to store 8 uncompressed characters or 9 compressed. We will only use 8 tho.
	char nBitsInBuffer = 0;
	while (*szMessage != 0) {
		nBuffer |= (unsigned long long)(*szMessage++ & 0x7F) << nBitsInBuffer;
		nBitsInBuffer += 7;
		if (nBitsInBuffer == 7 * 8) { //We have 8 chars in the buffer, dump them
			while (nBitsInBuffer > 0) {
				*szOut++ = nBuffer & 0xFF;
				nBuffer >>= 8;
				nBitsInBuffer -= 8;
			}
			//The following should be redundant, but just to be safe
			nBitsInBuffer = 0;
			nBuffer = 0;
		}
	}
	//Write out whatever is left in the buffer
	while (nBitsInBuffer > 0) {
		*szOut++ = nBuffer & 0xFF;
		nBuffer >>= 8;
		nBitsInBuffer -= 8;
	}
}

//uncomp()
//nCompressedLen is the number of bytes in the compressed buffer.
//NOTE: the compressed buffer does not have a NULL terminating character
void Uncompress(char *szOut, const unsigned char *szCompressed, unsigned nCompressedLen) {
	unsigned long long nBuffer = 0; //This is enough to store 8 uncompressed characters or 9 compressed. We will only use 8 tho.
	char nBitsInBuffer = 0;
	while (nCompressedLen) {
		while (nCompressedLen && nBitsInBuffer < 7 * 8) {
			nBuffer |= (unsigned long long)*szCompressed++ << nBitsInBuffer;
			nBitsInBuffer += 8;
			--nCompressedLen;
		}
		while (nBitsInBuffer > 0) {
			*szOut++ = nBuffer & 0x7F;
			nBuffer >>= 7;
			nBitsInBuffer -= 7;
		}
		//The following should be redundant, but just to be safe
		nBitsInBuffer = 0;
		nBuffer = 0;
	}
}

int main() {
	//char szMessage[] = "\x53\x7F\x63\x4B";
	char szMessage[] = "hello world. this is a compressed long string";
	scanf("%[^\n]%*c",szMessage);
	static const unsigned nCompressedSize = sizeof(szMessage) * 7 / 8; //This does not include the NULL terminating character from the string
	unsigned char pCompressedBytes[nCompressedSize];
	char szUncompressed[sizeof(szMessage)];
	printf("     Message: %s\n", szMessage);
	Compress(pCompressedBytes, szMessage);
	printf("  Compressed: ");
	for (int nByte = 0; nByte < nCompressedSize; ++nByte) {
		printf("%02X ", pCompressedBytes[nByte]);
	}
	printf("\n");
	Uncompress(szUncompressed, pCompressedBytes, nCompressedSize);
	szUncompressed[sizeof(szMessage) - 1] = 0; //We need to terminate the string. The NULL terminator is not stored in the compressed bytes
	printf("Uncompressed: %s\n", szUncompressed);
	//Now just verify that we ended up with the same message we started with
	if (strcmp(szMessage, szUncompressed) == 0) {
		printf("Compression works\n");
	} else {
		printf("Compression failed\n");
	}
	return 0;
}