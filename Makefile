PORT?=/dev/ttyUSB0
PROJECT=matrix_plus_rotary.cpp
ARDUINO_MODEL=nano
MCU=atmega328p
UPLOAD_RATE=57600
ARDUINO_DIR=${HOME}/.local/arduino-1.6.7/
AVR_TOOLS_PATH=${ARDUINO_DIR}/hardware/tools/avr/bin/
USER_LIBDIR=${CURDIR}/libs/
USER_LIBS=Adafruit-GFX-Library Adafruit_NeoPixel Adafruit_NeoMatrix RotaryEncoder
ARDUINO=167

CFLAGS=-Wno-warning

include Makefile.arduino

