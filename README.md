# `runs` — the opposite of `uniq`

`runs` is a simple, stateless command-line utility which filters an
input stream for only non-unique lines (i.e. lines that are "the same" as
at least one line above or below them.) In this sense, it does the opposite
of what POSIX `uniq(1)` does: `uniq(1)` gives you the stream without the
duplicates, while `runs(1)` gives you *only* the duplicates.

This behavior isn't of much use by itself (as far as I know!), but `runs` can
also take a flag `--prefixlen` (`-p` for short) to specify that lines should
be compared for "same-ness" by only their first N bytes. This turns `runs` into
a sort of "bucketing" or "chunking" stream, working much like the
`chunk_by` stream combinator found in some dynamic languages. Unlike these
stream combinators, `runs` will toss out any "bucket" with only one member.

`runs` is O(N) in time, and O(1) in memory usage; it doesn't buffer the entire
"bucket" before emitting it. This makes it safe to use as a filter on datasets
with potentially huge "buckets", where a dynamic language's `group_by` or
`chunk_by` stream combinator might generate excessive garbage or even choke on
the input.

`runs` (with the `-p` flag) is useful as a preprocessing step when analyzing
real-world hash-table or trie keysets for their collision or nesting-depth
properties. A large keyspace dump, with mostly "trivial" (1:1 pigeonholed) keys,
can be filtered down to only the non-trivial keys.

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

## Usage and options

`runs` takes two options:

* `--prefixlen=LEN`, `-p LEN`: as explained above, rather than comparing
  entire lines for equality, a `LEN`-byte prefix of the line will be compared
  instead. `runs` will exit with an error if it encounters a line shorter than
  the specified prefix length.

* `--invert`, `-i`: emits orphans (lines that aren't part of a run) rather than
  runs. This behavior isn't *quite* the same as that of the POSIX `uniq(1)`
  program; where `uniq(1)` would emit the first member of a multi-member bucket,
  `runs -i` will emit no members from such buckets.
