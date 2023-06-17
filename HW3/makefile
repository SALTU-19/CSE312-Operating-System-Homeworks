makeFileSystemObj = makeFileSystemObj/makeFileSystem.o makeFileSystemObj/main.o
fileSystemOperObj = fileSystemOperObj/fileSystemOper.o fileSystemOperObj/main.o
makeFileSystemBinary = makeFileSystem
fileSystemOperBinary = fileSystemOper
fileSystemOperFlags =   -g -lm -Ifilesystemoperfolder/include
makeFileSystemFlags =   -g -lm -Imakefilesystemfolder/include


.PHONY: all clean

all: $(makeFileSystemBinary) $(fileSystemOperBinary)


$(fileSystemOperBinary): $(fileSystemOperObj)
	gcc -o $@ $^ $(fileSystemOperFlags) -pthread -lrt

$(makeFileSystemBinary): $(makeFileSystemObj)
	gcc -o $@ $^ $(makeFileSystemFlags) -pthread -lrt
 

makeFileSystemObj/%.o: makefilesystemfolder/src/%.c
	mkdir -p $(@D)
	gcc $(makeFileSystemFlags) -c -o $@ $<

fileSystemOperObj/%.o: filesystemoperfolder/src/%.c
	mkdir -p $(@D)
	gcc $(fileSystemOperFlags) -c -o $@ $<


clean:
	rm -rf makeFileSystemObj fileSystemOperObj $(makeFileSystemBinary) $(fileSystemOperBinary)
