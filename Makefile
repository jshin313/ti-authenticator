# ----------------------------
# Set NAME to the program name
# Set ICON to the png icon file name
# Set DESCRIPTION to display within a compatible shell
# Set COMPRESSED to "YES" to create a compressed program
# ----------------------------

NAME        = OTP
COMPRESSED  ?= NO
ICON        ?= icon.png
DESCRIPTION ?= "OTP for the TI-84+ CE"
MAIN_ARGS   ?= NO
ARCHIVED    ?= NO

# ----------------------------

# ----------------------------
# Compile Options
# ----------------------------

OPT_MODE     ?= -Oz
EXTRA_CFLAGS ?= -Wall -Wextra

# ----------------------------
# Debug Options
# ----------------------------

OUTPUT_MAP   ?= NO

include $(CEDEV)/include/.makefile
