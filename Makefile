windows: screenshit.c capture.h capture.o stb_image_write.h
	gcc -o screenshit.exe screenshit.c capture.o -lgdi32

capture.o: capture.c
	gcc -c capture.c

