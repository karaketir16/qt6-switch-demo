ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules
QT_SWITCH_BUILD ?= $(abspath $(TOPDIR)/../../build/qtbase-switch)
OPENSSL_SWITCH_BUILD ?= $(abspath $(TOPDIR)/../../build/openssl-switch/install)

TARGET := qt6-switch-network-test
BUILD := build
APP_TITLE := Qt 6 Switch Network Test
APP_AUTHOR := Codex
APP_VERSION := 0.1.0
ICON := $(DEVKITPRO)/libnx/default_icon.jpg
ARCH := -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE
QT_INCLUDES := -I$(QT_SWITCH_BUILD)/include -I$(QT_SWITCH_BUILD)/include/QtCore \
    -I$(QT_SWITCH_BUILD)/include/QtGui -I$(QT_SWITCH_BUILD)/include/QtWidgets \
    -I$(QT_SWITCH_BUILD)/include/QtNetwork -I$(DEVKITPRO)/libnx/include \
    -I$(DEVKITPRO)/portlibs/switch/include -I$(OPENSSL_SWITCH_BUILD)/include
QT_CORE := $(QT_SWITCH_BUILD)/lib/libQt6Core.a
QT_NETWORK := $(QT_SWITCH_BUILD)/lib/libQt6Network.a
QT_GUI := $(QT_SWITCH_BUILD)/lib/libQt6Gui.a
QT_WIDGETS := $(QT_SWITCH_BUILD)/lib/libQt6Widgets.a
QT_PLUGIN := $(QT_SWITCH_BUILD)/plugins/platforms/libqswitch.a
QT_TLS_PLUGIN := $(QT_SWITCH_BUILD)/plugins/tls/libqopensslbackend.a
CXXFLAGS := -g -Wall -O2 -ffunction-sections $(ARCH) $(QT_INCLUDES) -D__SWITCH__ -fno-exceptions
LDFLAGS := -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH)
LIBS := $(QT_PLUGIN) $(QT_TLS_PLUGIN) $(QT_WIDGETS) $(QT_GUI) $(QT_NETWORK) $(QT_CORE) \
    -lpcre2-16 -lzstd -lGLESv2 -lEGL -lpng -lharfbuzz -lfreetype -lbz2 \
    -lcurl -lmbedtls -lmbedx509 -lmbedcrypto -L$(OPENSSL_SWITCH_BUILD)/lib \
    -lssl -lcrypto -lz -lnx
LIBDIRS := $(PORTLIBS) $(LIBNX)

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export TOPDIR BUILD QT_SWITCH_BUILD
export VPATH := $(CURDIR)
export DEPSDIR := $(CURDIR)/$(BUILD)
export LD := $(CXX)
export OFILES := main.o
export INCLUDE := -I$(CURDIR)/$(BUILD) -I$(CURDIR) $(QT_INCLUDES) $(foreach dir,$(LIBDIRS),-I$(dir)/include)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)
export APP_ICON := $(ICON)
export NROFLAGS := --icon=$(APP_ICON) --nacp=$(CURDIR)/$(TARGET).nacp
.PHONY: all clean
all: $(BUILD)
$(BUILD):
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $@ -f $(CURDIR)/Makefile QT_SWITCH_BUILD="$(QT_SWITCH_BUILD)"
clean:
	@rm -rf $(BUILD) $(TARGET).elf $(TARGET).nro $(TARGET).nacp
else
.PHONY: all
all: $(OUTPUT).nro
$(OUTPUT).nro: $(OUTPUT).elf $(TOPDIR)/$(TARGET).nacp
$(OUTPUT).elf: $(OFILES)
	@$(LD) $(LDFLAGS) -o $@ $^ $(LIBPATHS) $(LIBS)
-include $(OFILES:.o=.d)
endif
