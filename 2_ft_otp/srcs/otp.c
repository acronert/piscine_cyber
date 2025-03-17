# include "ft_otp.h"

unsigned char* hmac_sha1(const unsigned char *key, int key_len,
	const unsigned char *data, int data_len,
	unsigned char *result, unsigned int *result_len) {
	return HMAC(EVP_sha1(), key, key_len, data, data_len, result, result_len);
}

int	generate_hotp(unsigned char *key, int counter, int digits)
{
	unsigned char	hash[20]; // SHA1 returns a  20 bytes hash
	unsigned int	result_len;
	unsigned char	counter_bytes[8];

	// tranform counter into a unsigned char*, big endian !
	for (int i = 7; i >= 0; i--) {
		counter_bytes[i] = counter & 0xFF;
		counter >>= 8;
	}

	// Compute HMAC-SHA1
	hmac_sha1(key, 32, counter_bytes, 8, hash, &result_len);

	// Dynamic truncation with bitwise magic
	int	offset = hash[19] & 0x0f; // get the last 4bits as offset
	int bin_code =	(hash[offset] & 0x7f) << 24 |
					(hash[offset + 1] & 0xff) << 16 |
					(hash[offset + 2] & 0xff) << 8 |
					(hash[offset + 3] & 0xff);

	// Generate the OTP with the right number of digitd
	int	otp = bin_code % (int)pow(10, digits);

	return otp;
}