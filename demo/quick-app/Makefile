#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

QT_SWITCH_BUILD ?= $(abspath $(CURDIR)/../../build/qtbase-switch)
QTDECLARATIVE_SWITCH_BUILD ?= $(abspath $(CURDIR)/../../build/qtdeclarative-switch)
QTSHADERTOOLS_SWITCH_BUILD ?= $(abspath $(CURDIR)/../../build/qtshadertools-switch)

TARGET		:=	qt6-switch-quick-probe
BUILD		:=	build
SOURCES		:=	.
INCLUDES	:=	.

APP_TITLE	:=	Qt 6 Switch Quick Probe
APP_AUTHOR	:=	Codex
APP_VERSION	:=	0.1.0

ICON		:=	$(DEVKITPRO)/libnx/default_icon.jpg

ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

QT_CORE		:=	$(QT_SWITCH_BUILD)/lib/libQt6Core.a
QT_GUI		:=	$(QT_SWITCH_BUILD)/lib/libQt6Gui.a
QT_OPENGL	:=	$(QT_SWITCH_BUILD)/lib/libQt6OpenGL.a
QT_PLUGIN	:=	$(QT_SWITCH_BUILD)/plugins/platforms/libqswitch.a
QT_RSRC_1	:=	$(QT_SWITCH_BUILD)/src/gui/CMakeFiles/Gui_resources_1.dir/.qt/rcc/qrc_qpdf_init.cpp.obj
QT_RSRC_2	:=	$(QT_SWITCH_BUILD)/src/gui/CMakeFiles/Gui_resources_2.dir/.qt/rcc/qrc_gui_shaders_init.cpp.obj

QT_QML		:=	$(QTDECLARATIVE_SWITCH_BUILD)/lib/libQt6Qml.a
QT_QMLMETA	:=	$(QTDECLARATIVE_SWITCH_BUILD)/lib/libQt6QmlMeta.a
QT_QMLMODELS	:=	$(QTDECLARATIVE_SWITCH_BUILD)/lib/libQt6QmlModels.a
QT_QMLWORKER	:=	$(QTDECLARATIVE_SWITCH_BUILD)/lib/libQt6QmlWorkerScript.a
QT_QUICK	:=	$(QTDECLARATIVE_SWITCH_BUILD)/lib/libQt6Quick.a
QT_SHADERTOOLS	:=	$(QTSHADERTOOLS_SWITCH_BUILD)/lib/libQt6ShaderTools.a
QT_QML_PLUGIN	:=	$(QTDECLARATIVE_SWITCH_BUILD)/qml/QtQml/libqmlplugin.a
QT_QMLMODELS_PLUGIN	:=	$(QTDECLARATIVE_SWITCH_BUILD)/qml/QtQml/Models/libmodelsplugin.a
QT_QMLWORKER_PLUGIN	:=	$(QTDECLARATIVE_SWITCH_BUILD)/qml/QtQml/WorkerScript/libworkerscriptplugin.a
QT_QUICK_PLUGIN	:=	$(QTDECLARATIVE_SWITCH_BUILD)/qml/QtQuick/libqtquick2plugin.a
QT_QML_PLUGIN_INIT	:=	$(QTDECLARATIVE_SWITCH_BUILD)/src/qmlmeta/CMakeFiles/qmlplugin_init.dir/qmlplugin_init.cpp.obj
QT_QMLMODELS_PLUGIN_INIT	:=	$(QTDECLARATIVE_SWITCH_BUILD)/src/qmlmodels/CMakeFiles/modelsplugin_init.dir/modelsplugin_init.cpp.obj
QT_QMLWORKER_PLUGIN_INIT	:=	$(QTDECLARATIVE_SWITCH_BUILD)/src/qmlworkerscript/CMakeFiles/workerscriptplugin_init.dir/workerscriptplugin_init.cpp.obj
QT_QML_RSRC	:=	$(QTDECLARATIVE_SWITCH_BUILD)/src/qmlmeta/CMakeFiles/QmlMeta_resources_1.dir/.qt/rcc/qrc_qmake_QtQml_init.cpp.obj
QT_QMLMODELS_RSRC	:=	$(QTDECLARATIVE_SWITCH_BUILD)/src/qmlmodels/CMakeFiles/QmlModels_resources_1.dir/.qt/rcc/qrc_qmake_QtQml_Models_init.cpp.obj
QT_QMLWORKER_RSRC	:=	$(QTDECLARATIVE_SWITCH_BUILD)/src/qmlworkerscript/CMakeFiles/QmlWorkerScript_resources_1.dir/.qt/rcc/qrc_qmake_QtQml_WorkerScript_init.cpp.obj
QT_QUICK_RSRC	:=	$(QTDECLARATIVE_SWITCH_BUILD)/src/quick/CMakeFiles/Quick_resources_1.dir/.qt/rcc/qrc_qmake_QtQuick_init.cpp.obj
QT_QML_CORE_RSRC	:=	$(QTDECLARATIVE_SWITCH_BUILD)/src/qml/CMakeFiles/Qml_resources_1.dir/.qt/rcc/qrc_qmake_QML_init.cpp.obj
QT_QML_RSRC_OBJ	:=	$(QTDECLARATIVE_SWITCH_BUILD)/src/qmlmeta/CMakeFiles/QmlMeta.dir/.qt/rcc/qrc_qmake_QtQml.cpp.obj
QT_QMLMODELS_RSRC_OBJ	:=	$(QTDECLARATIVE_SWITCH_BUILD)/src/qmlmodels/CMakeFiles/QmlModels.dir/.qt/rcc/qrc_qmake_QtQml_Models.cpp.obj
QT_QMLWORKER_RSRC_OBJ	:=	$(QTDECLARATIVE_SWITCH_BUILD)/src/qmlworkerscript/CMakeFiles/QmlWorkerScript.dir/.qt/rcc/qrc_qmake_QtQml_WorkerScript.cpp.obj
QT_QUICK_RSRC_OBJ	:=	$(QTDECLARATIVE_SWITCH_BUILD)/src/quick/CMakeFiles/Quick.dir/.qt/rcc/qrc_qmake_QtQuick.cpp.obj
QT_QML_CORE_RSRC_OBJ	:=	$(QTDECLARATIVE_SWITCH_BUILD)/src/qml/CMakeFiles/Qml.dir/.qt/rcc/qrc_qmake_QML.cpp.obj

QT_PUBLIC_INCLUDES := \
	-I$(QT_SWITCH_BUILD)/include \
	-I$(QT_SWITCH_BUILD)/include/QtCore \
	-I$(QT_SWITCH_BUILD)/include/QtGui \
	-I$(QT_SWITCH_BUILD)/include/QtOpenGL \
	-I$(QTDECLARATIVE_SWITCH_BUILD)/include \
	-I$(QTDECLARATIVE_SWITCH_BUILD)/include/QtQml \
	-I$(QTDECLARATIVE_SWITCH_BUILD)/include/QtQmlMeta \
	-I$(QTDECLARATIVE_SWITCH_BUILD)/include/QtQmlModels \
	-I$(QTDECLARATIVE_SWITCH_BUILD)/include/QtQmlWorkerScript \
	-I$(QTDECLARATIVE_SWITCH_BUILD)/include/QtQuick \
	-I$(QT_SWITCH_BUILD)/include/QtCore/6.8.3 \
	-I$(QT_SWITCH_BUILD)/include/QtCore/6.8.3/QtCore \
	-I$(QT_SWITCH_BUILD)/include/QtGui/6.8.3 \
	-I$(QT_SWITCH_BUILD)/include/QtGui/6.8.3/QtGui \
	-I$(QT_SWITCH_BUILD)/include/QtOpenGL/6.8.3 \
	-I$(QT_SWITCH_BUILD)/include/QtOpenGL/6.8.3/QtOpenGL \
	-I$(QTDECLARATIVE_SWITCH_BUILD)/include/QtQml/6.8.3 \
	-I$(QTDECLARATIVE_SWITCH_BUILD)/include/QtQml/6.8.3/QtQml \
	-I$(QTDECLARATIVE_SWITCH_BUILD)/include/QtQmlModels/6.8.3 \
	-I$(QTDECLARATIVE_SWITCH_BUILD)/include/QtQmlModels/6.8.3/QtQmlModels \
	-I$(QTDECLARATIVE_SWITCH_BUILD)/include/QtQuick/6.8.3 \
	-I$(QTDECLARATIVE_SWITCH_BUILD)/include/QtQuick/6.8.3/QtQuick

CFLAGS	:=	-g -Wall -O2 -ffunction-sections $(ARCH) $(DEFINES)
CFLAGS	+=	$(INCLUDE) $(QT_PUBLIC_INCLUDES) -D__SWITCH__

CXXFLAGS	:=	$(CFLAGS) -fno-exceptions
ASFLAGS	:=	-g $(ARCH)
LDFLAGS	:=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:=	$(QT_QUICK_PLUGIN) $(QT_QMLWORKER_PLUGIN) $(QT_QMLMODELS_PLUGIN) $(QT_QML_PLUGIN) \
			$(QT_QML_PLUGIN_INIT) $(QT_QMLMODELS_PLUGIN_INIT) $(QT_QMLWORKER_PLUGIN_INIT) \
			$(QT_QUICK_RSRC) $(QT_QMLWORKER_RSRC) $(QT_QMLMODELS_RSRC) $(QT_QML_RSRC) $(QT_QML_CORE_RSRC) \
			$(QT_QUICK_RSRC_OBJ) $(QT_QMLWORKER_RSRC_OBJ) $(QT_QMLMODELS_RSRC_OBJ) $(QT_QML_RSRC_OBJ) $(QT_QML_CORE_RSRC_OBJ) \
			$(QT_QUICK) $(QT_QMLWORKER) $(QT_QMLMODELS) $(QT_QMLMETA) $(QT_QML) \
			$(QT_SHADERTOOLS) $(QT_RSRC_1) $(QT_RSRC_2) $(QT_PLUGIN) \
			$(QT_OPENGL) $(QT_GUI) $(QT_CORE) \
			-lpcre2-16 -lzstd -lGLESv2 -lEGL -lpng -lharfbuzz -lfreetype -lbz2 -lz -lnx
LIBDIRS	:=	$(PORTLIBS) $(LIBNX)

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)
export BUILD	:=	$(BUILD)
export QT_SWITCH_BUILD
export QTDECLARATIVE_SWITCH_BUILD
export QTSHADERTOOLS_SWITCH_BUILD

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

.PHONY: all nro clean check-quick

all: nro

check-quick:
	@test -f "$(QT_QUICK)" || { echo "Missing $(QT_QUICK). Run scripts/build-qtdeclarative-switch.sh first."; exit 1; }
	@test -f "$(QT_QML)" || { echo "Missing $(QT_QML). Run scripts/build-qtdeclarative-switch.sh first."; exit 1; }
	@test -f "$(QT_QUICK_PLUGIN)" || { echo "Missing $(QT_QUICK_PLUGIN). Build qtquick2plugin first."; exit 1; }
	@test -f "$(QT_QML_RSRC)" || { echo "Missing $(QT_QML_RSRC). Build qmlplugin first."; exit 1; }
	@test -f "$(QT_QML_CORE_RSRC)" || { echo "Missing $(QT_QML_CORE_RSRC). Build qtqmlcoreplugin first."; exit 1; }

nro: check-quick $(BUILD)

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
