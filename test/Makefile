#

CC=gcc

# Directory containing library sources
SRC_DIR = ../src

# List of all library sources that might need to be compiled into test executables
C_SOURCES:=
C_SOURCES+=$(SRC_DIR)/saeclib_circular_buffer.c
C_SOURCES+=$(SRC_DIR)/saeclib_collection.c
C_SOURCES+=./unity.c

# build directory for library .o files
BUILD_SRC_DIR = build

# List of all test sources that need to be run
TEST_SOURCES:=
TEST_SOURCES+=saeclib_circular_buffer_test.c
TEST_SOURCES+=saeclib_u8_circular_buffer_test.c
TEST_SOURCES+=saeclib_collection_test.c

# build directory for executables that run tests
BUILD_TEST_DIR = build_test

# compiler flags
C_INCLUDES:=
C_INCLUDES+=-I../src

CFLAGS = -O0 -Werror -Wall -g $(C_INCLUDES) -std=gnu99

# .o files for all of the individual application code source files
SOURCE_OBJECTS = $(addprefix $(BUILD_SRC_DIR)/,$(notdir $(C_SOURCES:.c=.o)))

# Each test src file generates one executable. All of these executables reside in the build_test
# directory
TESTS = $(addprefix $(BUILD_TEST_DIR)/,$(basename $(TEST_SOURCES)))

all: $(TESTS) $(SOURCE_OBJECTS)
	@for test in $(TESTS) ; do $$test || true; echo; echo ; done

$(BUILD_SRC_DIR)/%.o: $(SRC_DIR)/%.c Makefile | $(BUILD_SRC_DIR)
	@$(CC) -c $(CFLAGS) $< -o $@

# hack to keep unity.c in test/ folder
$(BUILD_SRC_DIR)/%.o: ./%.c Makefile | $(BUILD_SRC_DIR)
	@$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_TEST_DIR)/%: %.c Makefile $(SOURCE_OBJECTS) | $(BUILD_TEST_DIR)
	@$(CC) $(SOURCE_OBJECTS) $(CFLAGS) $(LDFLAGS) $< -o $@

$(BUILD_SRC_DIR):
	@mkdir $@

$(BUILD_TEST_DIR):
	@mkdir $@
