#include "ft_otp.h"

# define MODE_TOTP		1
# define MODE_ENCRYPT	2

int mode = 0;

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
	if (read(fd, hex_key, 65) != 64) {
		close(fd);
		dprintf(2, "incorrect hex_key size\n");
		free(hex_key);
		return NULL;
	}
	close(fd);

	// Validate hex characters
	for (int i = 0; i < 64; i++) {
		if (!strchr("0123456789abcdefABCDEF", hex_key[i])) {
			dprintf(2, "invalid hex_key\n");
			free(hex_key);
			return NULL;
		}
	}

	// Convert to raw bytes (still big endian)
	unsigned char *key = malloc(32 * sizeof(unsigned char));
	if (!key) {
		free(hex_key);
		return NULL;
	}

	for (int i = 0; i < 32; i++) {
		char	byte[3];
		byte[0] = hex_key[2*i];
		byte[1] = hex_key[2*i + 1];
		byte[2] = '\0';
		key[i] = strtol(byte, NULL, 16);
	}

	free(hex_key);

	return key;
}

int	parse_option(int argc, char **argv)
{
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			for (int j = 1; argv[i][j]; j++) {
				if (argv[i][j] == 'g')
					mode = MODE_ENCRYPT;
				else if (argv[i][j] == 'k')
					mode = MODE_TOTP;
				else
					return 1;
			}
		}
	}
	if (!mode)
		return 1;
	return 0;
}



int	main(int argc, char **argv)
{
	if (argc < 2)
		return 1;

	if (parse_option(argc, argv))
		return 1;

	if (mode == MODE_TOTP) {
		// key must be in raw bytes
		unsigned char	*key = parse_key(argv[1]);
		if (!key)
			return 1;
		int	otp = generate_hotp(key, time(NULL) / 30, 6);
		printf("otp = %06d\n", otp);
		free(key); // freaky
	}
	else if (mode == MODE_ENCRYPT) {
		// encrypt

	}

	return 0;
}
