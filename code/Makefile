TARGET_NAME ?= main
TARGET_BIN  ?= $(TARGET_NAME).bin

AS := riscv-none-embed-gcc
CC := riscv-none-embed-gcc
CXX := riscv-none-embed-g++
OBJCOPY := riscv-none-embed-objcopy
MKDIR_P ?= mkdir -p
UPLOAD := vendor/chprog.py
FLASH := vendor/rvprog.py -f

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src ./vendor/Core ./vendor/Debug ./vendor/Peripheral ./vendor/Startup ./vendor/User

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.S)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

FLAGS ?= -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g
ASFLAGS ?= $(FLAGS) -x assembler $(INC_FLAGS) -MMD -MP
CPPFLAGS ?=  $(FLAGS) $(INC_FLAGS) -std=gnu99 -MMD -MP
LDFLAGS ?= $(FLAGS) -T ./vendor/Ld/Link.ld -nostartfiles -Xlinker --gc-sections -Wl,-Map,"$(BUILD_DIR)/CH32V203.map" --specs=nano.specs --specs=nosys.specs

all: $(BUILD_DIR)/$(TARGET_BIN)

%.bin: %.elf
	$(OBJCOPY) -Obinary $< $@

%.hex: %.elf
	$(OBJCOPY) -Oihex  $< $@

%.elf: $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.S.o: %.S
	$(MKDIR_P) $(@D)
	$(CC) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)

flash: $(BUILD_DIR)/$(TARGET_BIN)
	$(FLASH) $<

upload: $(BUILD_DIR)/$(TARGET_BIN)
	$(UPLOAD) $<

.PHONY: clean flash upload all
-include $(DEPS)
