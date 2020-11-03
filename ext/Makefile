ext_app-objs = ext_app.o
CC = gcc
SED = sed
RM = rm -f

# Makefile.buildより
CFLAGS = -m64 -mno-red-zone -mno-sse -nostdinc -g -O \
-ffreestanding -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -fno-PIE \
-fno-strict-aliasing -Wall -DUSE_PAE -I./

extapp: ext_app.c
	gcc -I./ ext_app.c -g -c -o ext_app.o

# %.o: %.c
# 	gcc -I./ $*.c -g -c -o $*.o

# オブジェクトファイルまでを生成する
.c.o :
	$(CC) $(CFLAGS) -MD -c -o $@ $< && \
$(SED) -n -e 's/.*: *//' -e '/^ *$$/d' -e :a -e '/\\$$/{N' -e ba -e '}' \
-e 's/$$/ :/' -e H -e '$${x' -e p -e q -e '}' < $(@:.o=).d >> $(@:.o=).d || \
{ $(RM) $@; false; }

install/%.o: %.o %.d
	cp $*.o ../process/$*.o
	cp $*.d ../process/$*.d
	cp bv_calls.h ../process

clean:
	$(RM) -f *.o