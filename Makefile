# Makefile
HX_SOURCEMOD = ../sourcemod
#
# geoip.ext.so
#
HX_INCLUDE = -I. \
	-I$(HX_SOURCEMOD)/public \
	-I$(HX_SOURCEMOD)/public/amtl \
	-I$(HX_SOURCEMOD)/public/amtl/amtl \
	-I$(HX_SOURCEMOD)/sourcepawn/include
#
HX_QWERTY = -D_LINUX \
	-Dstricmp=strcasecmp \
	-D_stricmp=strcasecmp \
	-D_strnicmp=strncasecmp \
	-Dstrnicmp=strncasecmp \
	-D_snprintf=snprintf \
	-D_vsnprintf=vsnprintf \
	-D_alloca=alloca \
	-Dstrcmpi=strcasecmp \
	-Wall \
	-Werror \
	-Wno-switch \
	-Wno-unused \
	-Wno-reorder \
	-msse \
	-DSOURCEMOD_BUILD \
	-DHAVE_STDINT_H \
	-m32 \
	-DNDEBUG \
	-O3 \
	-funroll-loops \
	-pipe \
	-fno-strict-aliasing \
	-fvisibility=hidden \
	-DCOMPILER_GCC \
	-mfpmath=sse

CPP_FLAGS = -Wno-non-virtual-dtor \
	-fvisibility-inlines-hidden \
	-fno-exceptions \
	-fno-rtti
#
HX_SO = Release/geoip_util.o \
	Release/smsdk_ext.o \
	Release/extension.o
#
all:
	mkdir -p Release
	ln -sf $(HX_SOURCEMOD)/public/smsdk_ext.cpp
#
	gcc $(HX_INCLUDE) $(HX_QWERTY) $(CPP_FLAGS) -o Release/geoip_util.o -c geoip_util.cpp
	gcc $(HX_INCLUDE) $(HX_QWERTY) $(CPP_FLAGS) -o Release/smsdk_ext.o -c smsdk_ext.cpp
	gcc $(HX_INCLUDE) $(HX_QWERTY) $(CPP_FLAGS) -o Release/extension.o -c extension.cpp
#
	gcc $(HX_SO) data-pool.c maxminddb.c -static-libgcc -shared -m32 -lm -ldl -o Release/geoip.ext.so
#
	rm -rf Release/*.o
