# One Time Passwords (OTP) on the TI-84+ CE
TOTP for your graphing calculator.

### Description
One-Time Passwords are a popular way to use 2FA (2-factor authentication). Since, the codes change and can't be intercepted as easily as text-based MMS codes, OTP offers a pretty decent 2FA solution when used in conjunction with passwords. Apps like Google Authenticator offer OTP for phone platforms; similarly, this implementation allows you to get OTP codes from your TI-84+ CE graphing calculator.

I implemented OTP based on the following documents:
* [RFC 4226 (HOTP: An HMAC-Based One-Time Password Algorithm)](https://tools.ietf.org/html/rfc4226)
* [RFC 6238 (TOTP: Time-Based One-Time Password Algorithm)](https://tools.ietf.org/html/rfc6238)

They're actually pretty short compared to other standards; only like 16 pages long.

Unforunately, my implemenation doesn't fully meet the following specification because it it uses uint32_t to keep track of the epoch time.
```
The implementation of this algorithm MUST support a time value T
   larger than a 32-bit integer when it is beyond the year 2038.
```
I wasn't able to use a 64 bit integer since the LLVM toolchain currently doesn't support certain operations on 64 bit integers (e.g. lladd, llshl).

The unsigned int allows for dates past 2038, but it doesn't allow for negative values (i.e. dates before 1970) which shouldn't be too much of a problem for this use case.

This implementation was made primarily as an interesting little programming exercise. You probably shouldn't actually use it for real security stuff. 

### Build and Run
#### Regular Computer
```console
$ make 
$ ./otp
98673
```
#### TI-84+ CE
* Currently the code uses `JBSWY3DPEHPK3PXP` as the secret key. Change this if you want in [src/otp.c](src/otp.c)
* Download and build the LLVM version of the CE toolchain from [here](https://github.com/CE-Programming/toolchain) or use the online version on [TI-Planet](https://tiplanet.org/pb).
* Transfer the src folder to where you installed the CE toolchain and use `make` if you downloaded the toolchain or press `Build` if using the online version
* Make sure to set the calculator's current time to the correct UTC time by using `[MODE][Set CLOCK]`
* Transfer the 8xp produced to your calculator and run it using `[PGRM][OTP]` (you might have to use `[2nd][Asm]` before running the program depending on the OS version you're running on.

#### Demo
You can scan the barcode below or type the secret key in manually.
![QR Code](qr.png)
```
JBSWY3DPEHPK3PXP
```

### Credits
See the LICENSE folder
* Thanks to [SopaXorzTaker](https://www.cemetech.net/forum/viewtopic.php?t=14959&start=0) for the idea
* I used the [teeny-sha1 implementation](https://github.com/CTrabant/teeny-sha1/) for sha1.
* For the base32 decode function I used the one by [Google](https://github.com/google/google-authenticator-libpam/tree/master/src)
* This project was built using the [CE Toolchain](https://github.com/CE-Programming/toolchain)

### TODO
* Allow users to input the secret key
* Make a UI
* Allow for multiple codes like Google Authenticator
* Use USB maybe somehow
* Allow users to use the counter based OTP
