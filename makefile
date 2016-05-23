all:	out.img BTST2.BIN
	mkdir temp
	mount out.img temp/
	cp BTST2.BIN temp/
	
	umount temp/
	rm -rf temp/
	cp out.img vmmount/

out.img: src/bootsector.asm
	nasm -f bin -o out.img src/bootsector.asm

BTST2.BIN: src/btst2.asm
	nasm -f bin -o BTST2.BIN src/btst2.asm

clean:
	rm -rf temp
	rm -f out.img
	rm -f BTST2.BIN