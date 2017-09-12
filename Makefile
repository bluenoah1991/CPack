ifndef srcdir
	srcdir = .
endif

ifndef blddir
	blddir = build
endif

SOURCE_FILES = $(wildcard $(srcdir)/*.c)

CC ?= gcc

all: build

mkdir:
		-mkdir -p ${blddir}

build: mkdir ${SOURCE_FILES}
		${CC} -g ${SOURCE_FILES} -o ${blddir}/main

test: build
		${blddir}/main

clean:
		rm -rf ${blddir}/*

.PHONY: mkdir, clean, test
