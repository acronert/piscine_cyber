#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <openssl/sha.h>

void compute_sha1(const char *message, unsigned char output[SHA_DIGEST_LENGTH]) {
	SHA1((unsigned char*)message, strlen(message), output);
}

int main(int argc, char **argv) {
	unsigned char	hash[SHA_DIGEST_LENGTH];

	if (argc != 2)
		return 1;

	long int	counter = time(NULL) / 30;
	printf("counter = %ld\n", counter);

	compute_sha1(argv[1], hash);

	printf("SHA-1 hash: ");
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
		printf("%x", hash[i]);
	}
	printf("\n");
	
	printf("hash = %s\n", hash);

	long int otp = strtol(hash, NULL, 16);
	printf("otp = %d\n", otp);

	return 0;
}

// compile with -lcrypto


// get the key
// get the counter (time(NULL) / 30)
// convert the counter to big endian
