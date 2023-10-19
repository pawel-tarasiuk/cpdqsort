# cpdqsort

Translation of [Pattern-defeating quicksort (pdqsort) by Orson Peters](https://github.com/orlp/pdqsort) to a C-compilant macro.

Inspired by [Quicksort as a C macro](https://github.com/svpv/qsort). This implementation, however, has the pattern-defeating capabilities as well, making it of *O(n log n)* complexity pesimistically, in-place in terms of memory, and reasonably fast.

## Usage

Include the header and use `pdqsort` the same way as you would use [`qsort`](https://en.cppreference.com/w/c/algorithm/qsort) otherwise.

All the preprocessor symbols other than `pdqsort` and `heapsort` have names starting with `CPDQS_`. All the variables have names starting with `cpdqs_`, and the code itself produces no warnings with `-Wshadow` GCC flag. As such, it should not generate conflicts in the usual scenarios.

You can also set `CPDQS_EXPORT_HEAPSORT` to `0` or remove that definition if you prefer the header not to define `heapsort`. Or compile the code with `-DCPDQS_NO_EXPORT_HEAPSORT` if that's preferred to making changes to the file.
