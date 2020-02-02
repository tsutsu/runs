# `runs` — the opposite of `uniq`

`runs` is a simple, stateless command-line utility which filters an
input stream for only lines which share their first N bytes with a line
adjacent to them (ignoring empty lines.) Lines which do not share a common
prefix of length N with an adjacent line will be dropped.

Or, to put that another way, `runs` considers the first N bytes of each line
the "bucket key" for the line, and emits only "buckets" with more than one member.

`runs` is O(N) in time, and O(1) in memory usage. This makes it safe to use
in on datasets with large "buckets", where a dynamic language's `group_by` or
`chunk_by` stream combinator might generate excessive garbage or even choke on
the input.

`runs` is useful as a preprocessing step when analyzing real-world hash-table
or trie keysets for their collision or nesting-depth properties. A large
keyspace dump, with mostly "trivial" (1:1 pigeonholed) keys, can be filtered
down to only the non-trivial keys.


## Example usage

```sh
$ cat ./test.list
a1
a2
a2
b1
c1
d1
d2
d3
e1

$ runs 1 < ./test.list
a1
a2
a2
d1
d2
d3
```

## Also, there's `singles`

...which is "the opposite of the opposite of `uniq`." :grin: `singles` will
filter the input stream for only those lines that form single-member buckets,
i.e. which have no adjacent lines with the same prefix.

I'm not sure if there's a use for `singles`, but I threw it in. Let me know if
it's useful for you!

`runs` and `singles` are actually just two copies of the same binary; which
behavior you get from them depends on the executable's name (sort of like
`xz` and `unxz`.) You can hard link one to the other if you like.
