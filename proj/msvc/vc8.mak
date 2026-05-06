PROJECT         = opendse

DSE_LIB_ROOT    = ..\..

INC_DIR         = $(DSE_LIB_ROOT)\include
SRC_DIR         = $(DSE_LIB_ROOT)\src
DEF_DIR         = $(DSE_LIB_ROOT)\def
RES_DIR         = $(DSE_LIB_ROOT)\res


BIN_DIR         = $(DSE_LIB_ROOT)\..\out\library\bin
OBJ_DIR         = $(DSE_LIB_ROOT)\..\out\library\obj

C_FLAGS         = -MT -W3 -EHsc -Zi -Od

!ifdef DEBUG
    C_FLAGS     = -Z7 -DDEBUG
    LD_FLAGS    = -debug -pdb:none
!endif


LD_FLAGS        = -dll -out:$(BIN_DIR)\opendse.dll -def:$(DEF_DIR)\opendse.def

CC_FLAGS        = $(C_FLAGS) -I$(INC_DIR) -DCROC_STATIC_BUILD -DOPENDSE_LIB -DMSVC_GE_800 -DWIN32 -DWINDOWS
CC_FLAGS_DLL    = $(C_FLAGS) -I$(INC_DIR)
CC_LIBS         = user32.lib

CC              = cl -nologo
LINKER          = link.exe -nologo

CROCON_LIBS     = $(BIN_DIR)\opendse.lib

OBJECTS         = $(OBJ_DIR)\dsewin32.obj \
                    $(OBJ_DIR)\dsescrn.obj \
                    $(OBJ_DIR)\dseclrs.obj \
                    $(OBJ_DIR)\window.obj \
                    $(OBJ_DIR)\opendse.obj

all: prepare $(BIN_DIR)\$(PROJECT).dll

$(BIN_DIR)\$(PROJECT).dll: $(OBJECTS)
    $(LINKER) $(LD_FLAGS) /OUT:$@ $**

{$(SRC_DIR)}.c{$(OBJ_DIR)}.obj:
    $(CC) $(CC_FLAGS) -c $< -Fo$@

{$(SRC_DIR)\decoders}.c{$(OBJ_DIR)}.obj:
    $(CC) $(CC_FLAGS) -c $< -Fo$@

{$(SRC_DIR)\os\win32}.c{$(OBJ_DIR)}.obj:
    $(CC) $(CC_FLAGS) -c $< -Fo$@
    
prepare:
    @if not exist $(DSE_LIB_ROOT)\..\out mkdir $(DSE_LIB_ROOT)\..\out
    @if not exist $(DSE_LIB_ROOT)\..\out\library mkdir $(DSE_LIB_ROOT)\..\out\library
    @if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
    @if not exist $(BIN_DIR) mkdir $(BIN_DIR)

clean:
    -del $(OBJ_DIR)\*.obj
    -del $(BIN_DIR)\crocon.lib
