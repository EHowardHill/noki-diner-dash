TARGET      	:=  $(notdir $(CURDIR))
BUILD       	:=  build
LIBBUTANO   	:=  ./Butano/butano
PYTHON      	:=  python3
SOURCES     	:=  src ./Butano/common/src
INCLUDES    	:=  include ./Butano/common/include
DATA        	:=
GRAPHICS    	:=  graphics ./Butano/common/graphics
AUDIO       	:=  audio ./Butano/common/audio
AUDIOBACKEND	:=  maxmod
AUDIOTOOL		:=  
DMGAUDIO    	:=  dmg_audio ./Butano/common/dmg_audio
DMGAUDIOBACKEND	:=  default
ROMTITLE    	:=  NOKI
ROMCODE     	:=  SBTP
USERFLAGS   	:=  
USERCXXFLAGS	:=  
USERASFLAGS 	:=  
USERLDFLAGS 	:=  
USERLIBDIRS 	:=  
USERLIBS    	:=  
DEFAULTLIBS 	:=  
STACKTRACE		:=	
USERBUILD   	:=  
EXTTOOL     	:=  

#---------------------------------------------------------------------------------------------------------------------
# Export absolute butano path:
#---------------------------------------------------------------------------------------------------------------------
ifndef LIBBUTANOABS
	export LIBBUTANOABS	:=	$(realpath $(LIBBUTANO))
endif

#---------------------------------------------------------------------------------------------------------------------
# Include main makefile:
#---------------------------------------------------------------------------------------------------------------------
include $(LIBBUTANOABS)/butano.mak