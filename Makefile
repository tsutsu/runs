CFLAGS=-Wall -O3
src=runs.c

singles: runs
	cp ./runs ./singles

runs: $(src)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(src)

clean:
	rm -f ./runs ./singles ./*.o
