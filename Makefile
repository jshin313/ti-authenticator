# ----------------------------
# Set NAME to the program name
# Set ICON to the png icon file name
# Set DESCRIPTION to display within a compatible shell
# Set COMPRESSED to "YES" to create a compressed program
# ----------------------------

NAME ?= OTP
COMPRESSED  ?= NO
ICON        ?= icon.png
DESCRIPTION ?= "OTP for the TI-84+ CE"

# ----------------------------

include $(CEDEV)/include/.makefile
