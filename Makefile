PREF_AKI_OBJ = ./aki_obj/
PREF_AKI_SRC = ./aki_src/
PREF_BUF_OBJ = ./buffer_process_obj/
PREF_BUF_SRC = ./buffer_process_src/
CC = g++
TARGET_1 = aki.out
FLAGS = -D _DEBUG -ggdb3 \
    -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat        \
    -Wmissing-declarations -Wcast-qual -Wchar-subscripts  \
    -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
    -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual \
    -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo \
    -Wstrict-overflow=2 \
    -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef \
    -Wunreachable-code -Wunused -Wvariadic-macros \
    -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs \
    -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow \
    -fno-omit-frame-pointer -Wlarger-than=8192 \
    -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

LINK_FLAGS = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

$(TARGET_1): $(PREF_AKI_OBJ)main.o $(PREF_AKI_OBJ)akinator.o $(PREF_AKI_OBJ)akinator_helpers.o $(PREF_BUF_OBJ)buffer_process.o
	@ $(CC) $(LINK_FLAGS) $(PREF_AKI_OBJ)main.o $(PREF_AKI_OBJ)akinator.o $(PREF_AKI_OBJ)akinator_helpers.o $(PREF_BUF_OBJ)buffer_process.o -o $(TARGET_1) -L./lib -lstack -I./lib

$(PREF_AKI_OBJ)main.o: $(PREF_AKI_SRC)main.cpp
	@ $(CC) -c $(PREF_AKI_SRC)main.cpp -o $(PREF_AKI_OBJ)main.o $(FLAGS)

$(PREF_AKI_OBJ)akinator.o: $(PREF_AKI_SRC)akinator.cpp
	@ $(CC) -c $(PREF_AKI_SRC)akinator.cpp -o $(PREF_AKI_OBJ)akinator.o $(FLAGS)

$(PREF_AKI_OBJ)akinator_helpers.o: $(PREF_AKI_SRC)akinator_helpers.cpp
	@ $(CC) -c $(PREF_AKI_SRC)akinator_helpers.cpp -o $(PREF_AKI_OBJ)akinator_helpers.o $(FLAGS)

$(PREF_BUF_OBJ)buffer_process.o: $(PREF_BUF_SRC)buffer_process.cpp
	@ $(CC) -c $(PREF_BUF_SRC)buffer_process.cpp -o $(PREF_BUF_OBJ)buffer_process.o $(FLAGS)

clean:
	rm $(TARGET_1) $(PREF_AKI_OBJ)*.o $(PREF_BUF_OBJ)*.o
