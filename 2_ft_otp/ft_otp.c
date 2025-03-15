#include <stdio.h>
#include <string.h>
// #include <stdlib.h>
// #include <openssl/sha.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include <openssl/hmac.h>

unsigned char* hmac_sha1(const unsigned char *key, int key_len,
	const unsigned char *data, int data_len,
	unsigned char *result, unsigned int *result_len) {
	return HMAC(EVP_sha1(), key, key_len, data, data_len, result, result_len);
}

int	generate_hotp(unsigned char *key, int counter, int digits)
{
	unsigned char		hash[20]; // SHA1 returns a  20 bytes hash
	unsigned int	result_len;
	unsigned char		counter_bytes[8];

	// tranform counter into a unsigned char*, big endian
	for (int i = 7; i >= 0; i--) {
		counter_bytes[i] = counter & 0xFF;
		counter >>= 8;
	}

	// Compute HMAC-SHA1
	hmac_sha1(key, strlen(key), counter_bytes, 8, hash, &result_len);

	// Dynamic truncation
	int	offset = hash[19] & 0x0f; // get the last 4bits as offset
	int bin_code =	(hash[offset] & 0x7f) << 24 |
					(hash[offset + 1] & 0xff) << 16 |
					(hash[offset + 2] & 0xff) << 8 |
					(hash[offset + 3] & 0xff);

	// Generate the OTP with the right number of digitd
	int	otp = bin_code % (int)pow(10, digits);

	return otp;
}



int	generate_otp(char *k)
{
	int	counter = time(NULL) / 30;
	return generate_hotp(k, counter, 6);
}

unsigned char *parse_key(char *filename)
{
	unsigned char *hex_key = malloc(65 * sizeof(unsigned char));
	if (!hex_key)
		return NULL;

	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		dprintf(2, "couldnt open %s\n", filename);
		return NULL;
	}
	int read_return = read(fd, hex_key, 65);
	printf("read_return = %d\n", read_return);

	close(fd);

	if (strlen(hex_key) != 64) {
		dprintf(2, "incorrect hex_key size\n");
		free(hex_key);
		return NULL;
	}
	// Validate hex characters
	for (int i = 0; i < 64; i++) {
		if (!strchr("0123456789abcdefABCDEF", hex_key[i])) {
			dprintf(2, "invalid hex_key\n");
			free(hex_key);
			return NULL;
		}
	}

	// Convert to binary key
	unsigned char *key = malloc(32 * sizeof(unsigned char));
	if (!key) {
		free(hex_key);
		return NULL;
	}


	return key;
}

int	main(int argc, char** argv)
{
	if (argc < 2)
		return 1;
	unsigned char	*key = parse_key(argv[1]);
	if (!key)
		return 1;
	int	otp = generate_otp(key);
	printf("otp = %d\n", otp);
	return 0;
}


// void compute_sha1(const char *message, unsigned char output[SHA_DIGEST_LENGTH]) {
// 	SHA1((unsigned char*)message, strlen(message), output);
// }

// int main(int argc, char **argv) {
// 	unsigned char	hash[SHA_DIGEST_LENGTH];

// 	if (argc != 2)
// 		return 1;

// 	long int	counter = time(NULL) / 30;
// 	printf("counter = %ld\n", counter);

// 	compute_sha1(argv[1], hash);

// 	printf("SHA-1 hash: ");
// 	for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
// 		printf("%x", hash[i]);
// 	}
// 	printf("\n");
	
// 	printf("hash = %s\n", hash);

// 	long int otp = strtol(hash, NULL, 16);
// 	printf("otp = %d\n", otp);

// 	return 0;
// }

// compile with -lcrypto


// get the key
// get the counter (time(NULL) / 30)
// convert the counter to big endian
