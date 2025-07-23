CC = gcc
CFLAGS = -Iinclude -Ithirdparty/raylib-5.5/win/include
LDFLAGS = -Lthirdparty/raylib-5.5/win/lib -lraylib -lopengl32 -lgdi32 -lwinmm -lole32 -lws2_32 -lcomdlg32 -luuid -static-libgcc -static-libstdc++

OBJ = build/screenshit.o build/capture.o build/imageops.o build/clipboard.o

windows: $(OBJ)
	$(CC) -o screenshit.exe src/main.c $(OBJ) $(LDFLAGS)

build/screenshit.o: src/screenshit.c src/screenshit.h
	$(CC) $(CFLAGS) -c $< -o $@

build/capture.o: src/capture.c src/capture.h
	$(CC) $(CFLAGS) -c $< -o $@

build/imageops.o: src/imageops.c src/imageops.h
	$(CC) $(CFLAGS) -c $< -o $@

build/clipboard.o: src/clipboard.c src/clipboard.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f build/*.o screenshit.exe
