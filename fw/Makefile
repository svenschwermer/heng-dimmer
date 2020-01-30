target ?= heng-dimmer.elf
src = main.c encoder.c
obj = $(src:%c=%o)

CC = avr-gcc
CFLAGS = -mmcu=attiny13a -Os -DF_CPU=1200000ul -std=gnu11 -Wall -Werror
LDFLAGS = -mmcu=attiny13a -Os

$(target): $(obj)
	$(CC) $(LDFLAGS) -o $@ $^
	avr-objdump -P mem-usage $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(obj) $(target)

flash: $(target)
	avrdude -p t13 -c jtag3isp -e -U flash:w:$<

#fuse:
#	avrdude -p t13 -c jtag3updi -U fuse2:w:0x1:m

dis: $(target)
	avr-objdump -S $< | less

.PHONY: clean flash fuse dis
