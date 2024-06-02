# Sample files

Note: If you need to generate a file with a lot of
data, I suggest visiting https://www.github.com/malloc-nbytes/sic.git/

This directory is for sample input for the various tests.

The name for each file should be:

`<description>-<number>.in`

The first line of the file should be:
`# File for <test-<testname>/<function>>`

Once the file has been written to, use ./lock-file.sh to make it read-only to keep from uneeded modifications that would
break tests. If a file needs to be edited, use ./unlock-file.sh to make it writable again.
