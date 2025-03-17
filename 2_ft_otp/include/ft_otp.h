#ifndef FT_OTP_H
# define FT_OTP_H

# include <stdio.h>
# include <string.h>
# include <math.h>
# include <unistd.h>
# include <fcntl.h>
# include <time.h>
# include <openssl/hmac.h>

# define MODE_TOTP		1
# define MODE_ENCRYPT	2

int	generate_hotp(unsigned char *key, int counter, int digits);

#endif