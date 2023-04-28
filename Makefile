# Путь к компилятору
COMPILER=g++

# Флаги компиляции
FLAGS=-O1 -mavx2 -no-pie -Wno-unused-parameter -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_

# Папка с .o
BIN_DIR=binary

# Папка с исходниками и заголовками
SRC_DIR=source


all: $(BIN_DIR) table.exe conv.exe


# Завершает сборку table
table.exe: $(addprefix $(BIN_DIR)/, table.o hash_table.o text_parser.o) 
	$(COMPILER) $^ -o $@


# Завершает сборку converter
conv.exe: $(addprefix $(BIN_DIR)/, converter.o hash_table.o text_parser.o) 
	$(COMPILER) $^ -o $@


# Предварительная сборка table.cpp
$(BIN_DIR)/table.o: $(addprefix $(SRC_DIR)/, table.cpp text_parser.hpp hash_table.hpp assert.hpp)
	$(COMPILER) $(FLAGS) -c $< -o $@


# Предварительная сборка converter.cpp
$(BIN_DIR)/converter.o: $(addprefix $(SRC_DIR)/, converter.cpp text_parser.hpp hash_table.hpp assert.hpp)
	$(COMPILER) $(FLAGS) -c $< -o $@


# Предварительная сборка hash_table.cpp
$(BIN_DIR)/hash_table.o: $(addprefix $(SRC_DIR)/, hash_table.cpp hash_table.hpp assert.hpp)
	$(COMPILER) $(FLAGS) -c $< -o $@


# Предварительная сборка text_parser.cpp
$(BIN_DIR)/text_parser.o: $(addprefix $(SRC_DIR)/, text_parser.cpp text_parser.hpp hash_table.hpp assert.hpp)
	$(COMPILER) $(FLAGS) -c $< -o $@


# Создание папки для .o, если она еще не существует
$(BIN_DIR):
	mkdir $@
