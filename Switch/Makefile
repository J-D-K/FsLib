.PHONY: all FsLib TestingApp clean

all:	FsLib TestingApp

FsLib:
	$(MAKE) -C FsLib

TestingApp: FsLib
	$(MAKE) -C TestingApp

clean:
	$(MAKE) -C FsLib clean
	$(MAKE) -C TestingApp clean
