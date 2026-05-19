PROJECT         =  opendse

DSE_LIB_ROOT    =  ../..

DSE_DEBUG       =? false
DSE_LEGACY      =? false

INC_DIR         =  $(DSE_LIB_ROOT)/include
SRC_DIR         =  $(DSE_LIB_ROOT)/src
DEF_DIR         =  $(DSE_LIB_ROOT)/def
RES_DIR         =  $(DSE_LIB_ROOT)/res

SRC_DIRS        =  $(SRC_DIR) \
                   $(SRC_DIR)/os/unix \
                   $(SRC_DIR)/devices \
                   $(SRC_DIR)/mmio \
                   $(SRC_DIR)/parsers \
                   $(SRC_DIR)/decoders \
                   $(SRC_DIR)/utils

BIN_DIR         =  $(DSE_LIB_ROOT)/out/bin
OBJ_DIR         =  $(DSE_LIB_ROOT)/out/obj

CC              =  gcc
OPENDSE_BACKEND =  alsa

ifeq ($(OPENDSE_BACKEND),alsa)
C_BACKEND       = -DUNIX_ALSA
LD_LIBS         = -lasound
else
C_BACKEND       = -DUNIX_OSS
endif

ifeq ($(OPENDSE_LEGACY),true)
C_LEGACY        = -DUNIX_LEGACY
endif

ifeq ($(OPENDSE_DEBUG),true)
C_FLAGS         =  -O0 -fPIC -DOPENDSE_LIB -DUNIX -DHAVE_VPRINTF $(C_BACKEND) $(C_LEGACY) -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls
else
C_FLAGS         =  -O2 -fPIC -DOPENDSE_LIB -DUNIX -DHAVE_VPRINTF $(C_BACKEND) $(C_LEGACY)
endif

SOURCES         =  $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))

OBJECTS         =  $(OBJ_DIR)/outdev.o \
                   $(OBJ_DIR)/mmio.o \
                   $(OBJ_DIR)/parser.o \
                   $(OBJ_DIR)/riffpars.o \
                   $(OBJ_DIR)/alsawrap.o \
                   $(OBJ_DIR)/dseunix.o \
                   $(OBJ_DIR)/opendse.o

all: prepare $(BIN_DIR)/lib$(PROJECT).so $(BIN_DIR)/lib$(PROJECT).a

prepare:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(C_FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/os/unix/%.c
	$(CC) $(C_FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/decoders/%.c
	$(CC) $(C_FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/devices/%.c
	$(CC) $(C_FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/mmio/%.c
	$(CC) $(C_FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/parsers/%.c
	$(CC) $(C_FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/utils/%.c
	$(CC) $(C_FLAGS) -I$(INC_DIR) -c $< -o $@

$(BIN_DIR)/lib$(PROJECT).so: $(OBJECTS)
	$(CC) $(LD_ARCH) -shared $(LD_LIBS) -o $@ $(OBJECTS) $(LD_FLAGS)

$(BIN_DIR)/lib$(PROJECT).a: $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

install: all
	mkdir -p $(PREFIX)/lib
	cp $(BIN_DIR)/lib$(PROJECT).so $(PREFIX)
	cp $(BIN_DIR)/lib$(PROJECT).a $(PREFIX)

	mkdir -p $(PREFIX)/include/$(PROJECT)
	cp $(INC_DIR)/*.h $(PREFIX)/include/$(PROJECT)/

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR)

rebuild:
	clean all

.PHONY: 
	all install clean rebuild
