CFLAGS=-Wall -O3
src=runs.c

runs: $(src)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(src)

clean:
	rm -f ./runs ./*.o
