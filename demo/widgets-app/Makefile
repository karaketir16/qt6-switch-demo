#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

QT_SWITCH_BUILD ?= /Volumes/T7/sdk/qt6-switch-build/qtbase-widgets-test

TARGET		:=	qt6-switch-widgets-probe
BUILD		:=	build
SOURCES		:=	.
INCLUDES	:=	.

APP_TITLE	:=	Qt 6 Switch Widgets Probe
APP_AUTHOR	:=	Codex
APP_VERSION	:=	0.1.0

ICON		:=	$(DEVKITPRO)/libnx/default_icon.jpg

ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

QT_CORE		:=	$(QT_SWITCH_BUILD)/lib/libQt6Core.a
QT_GUI		:=	$(QT_SWITCH_BUILD)/lib/libQt6Gui.a
QT_WIDGETS	:=	$(QT_SWITCH_BUILD)/lib/libQt6Widgets.a
QT_PLUGIN	:=	$(QT_SWITCH_BUILD)/plugins/platforms/libqswitch.a
QT_RSRC_1	:=	$(QT_SWITCH_BUILD)/src/gui/CMakeFiles/Gui_resources_1.dir/.qt/rcc/qrc_qpdf_init.cpp.obj
QT_RSRC_2	:=	$(QT_SWITCH_BUILD)/src/gui/CMakeFiles/Gui_resources_2.dir/.qt/rcc/qrc_gui_shaders_init.cpp.obj

QT_PUBLIC_INCLUDES := \
	-I$(QT_SWITCH_BUILD)/include \
	-I$(QT_SWITCH_BUILD)/include/QtCore \
	-I$(QT_SWITCH_BUILD)/include/QtGui \
	-I$(QT_SWITCH_BUILD)/include/QtWidgets \
	-I$(QT_SWITCH_BUILD)/include/QtCore/6.8.3 \
	-I$(QT_SWITCH_BUILD)/include/QtCore/6.8.3/QtCore \
	-I$(QT_SWITCH_BUILD)/include/QtGui/6.8.3 \
	-I$(QT_SWITCH_BUILD)/include/QtGui/6.8.3/QtGui \
	-I$(QT_SWITCH_BUILD)/include/QtWidgets/6.8.3 \
	-I$(QT_SWITCH_BUILD)/include/QtWidgets/6.8.3/QtWidgets

CFLAGS	:=	-g -Wall -O2 -ffunction-sections $(ARCH) $(DEFINES)
CFLAGS	+=	$(INCLUDE) $(QT_PUBLIC_INCLUDES) -D__SWITCH__

CXXFLAGS	:=	$(CFLAGS) -fno-exceptions
ASFLAGS	:=	-g $(ARCH)
LDFLAGS	:=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:=	$(QT_RSRC_1) $(QT_RSRC_2) $(QT_PLUGIN) $(QT_WIDGETS) $(QT_GUI) $(QT_CORE) \
			-lpcre2-16 -lzstd -lGLESv2 -lEGL -lpng -lharfbuzz -lfreetype -lbz2 -lz -lnx
LIBDIRS	:=	$(PORTLIBS) $(LIBNX)

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)
export BUILD	:=	$(BUILD)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES	:=	$(OFILES_SRC)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export APP_ICON	:=	$(ICON)
export NROFLAGS	:=	--icon=$(APP_ICON) --nacp=$(CURDIR)/$(TARGET).nacp
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: all nro clean

all: nro

nro: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).nro $(TARGET).nacp $(TARGET).elf

else

.PHONY: all

DEPENDS	:=	$(OFILES:.o=.d)

all: $(OUTPUT).nro

$(OUTPUT).nro: $(OUTPUT).elf $(TOPDIR)/$(TARGET).nacp

$(OUTPUT).elf: $(OFILES)

-include $(DEPENDS)

endif
