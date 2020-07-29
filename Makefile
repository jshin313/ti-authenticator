CC = gcc
CFLAGS += -DREGULAR

binary = otp

hmac:
	$(CC) -o $(binary) src/otp.c src/hmac.c src/base32.c src/teeny-sha1.c $(CFLAGS)

.PHONY : clean
clean :
	-rm $(binary)
