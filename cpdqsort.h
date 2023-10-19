/*
    cpdqsort.h - Pattern-defeating quicksort as a C-compilant macro.

    Copyright (c) 2023 Pawel Tarasiuk

    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from
    the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not
       be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/

#ifndef __CPDQSORT_H__
#define __CPDQSORT_H__

#include <string.h>


/* Should the heapsort macro be defined? */
#ifndef CPDQS_NO_EXPORT_HEAPSORT
#define CPDQS_EXPORT_HEAPSORT 1
#endif

/* Partitions below this size are sorted using insertion sort. */
#define CPDQS_ISRT_THRESHOLD 24

/*
When we detect an already sorted partition, attempt an insertion sort
that allows this amount of element moves before giving up.
*/
#define CPDQS_PISRT_LIMIT 8

/* Partitions above this size use Tukey's ninther to select the pivot. */
#define CPDQS_T9THER 128


/* Variables naming scheme */
#define CPDQS_V(x) cpdqs_ ## x

/* Context-based shortcuts */
#define CPDQS_AT(u) ((char *)(CPDQS_V(begin)) + (u) * CPDQS_V(size))
#define CPDQS_SET(d, s) memcpy((d), (s), CPDQS_V(size))
#define CPDQS_SFT(p, n) ((p) + (n) * CPDQS_V(size))
#define CPDQS_LEN(b, e) (((e) - (b)) / CPDQS_V(size))


/* Swaps the elements *a and *b. */
#define CPDQS_SW(a, b) { \
        size_t CPDQS_V(i); \
        char CPDQS_V(t); \
        for(CPDQS_V(i) = 0; CPDQS_V(i) < CPDQS_V(size); ++CPDQS_V(i)) { \
            CPDQS_V(t) = ((char *)(a))[CPDQS_V(i)]; \
            ((char *)(a))[CPDQS_V(i)] = ((char *)(b))[CPDQS_V(i)]; \
            ((char *)(b))[CPDQS_V(i)] = CPDQS_V(t); \
        } \
    }


/* Sorts the elements *a and *b. */
#define CPDQS_SRT2(a, b) { \
        if(CPDQS_V(compar)((b), (a)) < 0) { \
            CPDQS_SW((a), (b)); \
        } \
    }


/* Sorts the elements *a, *b and *c. */
#define CPDQS_SRT3(a, b, c) { \
        CPDQS_SRT2((a), (b)); \
        CPDQS_SRT2((b), (c)); \
        CPDQS_SRT2((a), (b)); \
    }


/* Returns floor(log2(n)), assumes n > 0. */
#if __has_builtin(__builtin_clz)
#define CPDQS_LOG2(dest, n) ((dest) = (31 - (int)(__builtin_clz(n))))
#else
#define CPDQS_LOG2(dest, n) { \
        size_t CPDQS_V(_n) = n; \
        (dest) = 0; \
        while (CPDQS_V(_n) >>= 1) ++(dest); \
    }
#endif


/* Stores the space for tmp and pivot elements for all functions */
void *CPDQS_V(TMP_SPACE_PTR);

#define CPDQS_TMP(dest) { \
        if (CPDQS_V(TMP_SPACE_PTR) == NULL) { \
            CPDQS_V(TMP_SPACE_PTR) = malloc(CPDQS_V(size)); \
        } \
        (dest) = CPDQS_V(TMP_SPACE_PTR); \
    }

#define CPDQS_TMP_END { \
        free(CPDQS_V(TMP_SPACE_PTR)); \
        CPDQS_V(TMP_SPACE_PTR) = NULL; \
    }


/* Sorts range using insertion sort. */
#define CPDQS_ISRT(begin, nmemb) { \
        void *CPDQS_V(tmp); \
        size_t CPDQS_V(cur), CPDQS_V(sift), CPDQS_V(sift_1); \
        \
        if((nmemb) > 1) { \
            CPDQS_TMP(CPDQS_V(tmp)); \
            \
            for (CPDQS_V(cur) = 1; CPDQS_V(cur) < (nmemb); ++CPDQS_V(cur)) { \
                CPDQS_V(sift) = CPDQS_V(cur); \
                CPDQS_V(sift_1) = CPDQS_V(cur) - 1; \
                \
                if (CPDQS_V(compar)(CPDQS_AT(CPDQS_V(sift)), CPDQS_AT(CPDQS_V(sift_1))) < 0) { \
                    CPDQS_SET(CPDQS_V(tmp), CPDQS_AT(CPDQS_V(sift))); \
                    \
                    do { \
                        CPDQS_SET(CPDQS_AT(CPDQS_V(sift)--), CPDQS_AT(CPDQS_V(sift_1))); \
                    } while( \
                            CPDQS_V(sift) != 0 && \
                            CPDQS_V(compar)(CPDQS_V(tmp), CPDQS_AT(--CPDQS_V(sift_1))) < 0); \
                    \
                    CPDQS_SET(CPDQS_AT(CPDQS_V(sift)), CPDQS_V(tmp)); \
                } \
            } \
        } \
    }

/*
Sorts range using insertion sort.
Assumes that before the begin there is an element smaller than or equal
to any element in the range.
*/
#define CPDQS_UISRT(begin, nmemb) { \
        void *CPDQS_V(tmp); \
        size_t CPDQS_V(cur), CPDQS_V(sift), CPDQS_V(sift_1); \
        \
        if((nmemb) > 1) { \
            CPDQS_TMP(CPDQS_V(tmp)); \
            \
            for (CPDQS_V(cur) = 1; CPDQS_V(cur) < (nmemb); ++CPDQS_V(cur)) { \
                CPDQS_V(sift) = CPDQS_V(cur); \
                CPDQS_V(sift_1) = CPDQS_V(cur) - 1; \
                \
                if (CPDQS_V(compar)(CPDQS_AT(CPDQS_V(sift)), CPDQS_AT(CPDQS_V(sift_1))) < 0) { \
                    CPDQS_SET(CPDQS_V(tmp), CPDQS_AT(CPDQS_V(sift))); \
                    \
                    do { \
                        CPDQS_SET(CPDQS_AT(CPDQS_V(sift)--), CPDQS_AT(CPDQS_V(sift_1))); \
                    } while(CPDQS_V(compar)(CPDQS_V(tmp), CPDQS_AT(--CPDQS_V(sift_1))) < 0); \
                    \
                    CPDQS_SET(CPDQS_AT(CPDQS_V(sift)), CPDQS_V(tmp)); \
                } \
            } \
        } \
    }


/*
Attempts to use insertion sort on range. Will return false if more than
partial_insertion_sort_limit elements were moved, and abort sorting.
Otherwise it will successfully sort and return true.
*/
#define CPDQS_PISRT(dest, _begin, nmemb) { \
        void *CPDQS_V(tmp); \
        void *CPDQS_V(begin_pop) = CPDQS_V(begin); \
        size_t CPDQS_V(cur), CPDQS_V(sift), CPDQS_V(sift_1), CPDQS_V(limit); \
        CPDQS_V(begin) = (_begin); \
        \
        if((nmemb) > 1) { \
            CPDQS_TMP(CPDQS_V(tmp)); \
            CPDQS_V(limit) = 0; \
            \
            for (CPDQS_V(cur) = 1; CPDQS_V(cur) < (nmemb); ++CPDQS_V(cur)) { \
                CPDQS_V(sift) = CPDQS_V(cur); \
                CPDQS_V(sift_1) = CPDQS_V(cur) - 1; \
                \
                if (CPDQS_V(compar)(CPDQS_AT(CPDQS_V(sift)), CPDQS_AT(CPDQS_V(sift_1))) < 0) { \
                    CPDQS_SET(CPDQS_V(tmp), CPDQS_AT(CPDQS_V(sift))); \
                    \
                    do { \
                        CPDQS_SET(CPDQS_AT(CPDQS_V(sift)--), CPDQS_AT(CPDQS_V(sift_1))); \
                    } while( \
                            CPDQS_V(sift) != 0 && \
                            CPDQS_V(compar)(CPDQS_V(tmp), CPDQS_AT(--CPDQS_V(sift_1))) < 0); \
                    \
                    CPDQS_SET(CPDQS_AT(CPDQS_V(sift)), CPDQS_V(tmp)); \
                    CPDQS_V(limit) += CPDQS_V(cur) - CPDQS_V(sift); \
                } \
                \
                if (CPDQS_V(limit) > CPDQS_PISRT_LIMIT) { \
                    break; \
                } \
            } \
        } \
        (dest) = (CPDQS_V(limit) <= CPDQS_PISRT_LIMIT); \
        CPDQS_V(begin) = CPDQS_V(begin_pop); \
    }


/*
Partitions the range around pivot at the beginning. Elements equal to
the pivot are put in the right-hand partition. Returns the position of
the pivot after partitioning and whether the passed sequence already was
correctly partitioned. Assumes the pivot is a median of at least 3 elements
and that range is at least insertion_sort_threshold long.
*/
#define CPDQS_PAR(pos, already_partitioned, base, nmemb) { \
        char *CPDQS_V(pbegin) = (base); \
        void *CPDQS_V(pivot); \
        char *CPDQS_V(first) = CPDQS_V(pbegin); \
        char *CPDQS_V(last) = CPDQS_SFT(CPDQS_V(pbegin), (nmemb)); \
        \
        CPDQS_TMP(CPDQS_V(pivot)); \
        CPDQS_SET(CPDQS_V(pivot), CPDQS_V(pbegin)); \
        \
        while (CPDQS_V(compar)(CPDQS_V(first) += CPDQS_V(size), CPDQS_V(pivot)) < 0); \
        \
        if (CPDQS_SFT(CPDQS_V(first), -1) == CPDQS_V(pbegin)) { \
            while ( \
                    CPDQS_V(first) < CPDQS_V(last) && \
                    !(CPDQS_V(compar)(CPDQS_V(last) -= CPDQS_V(size), CPDQS_V(pivot)) < 0)); \
        } else { \
            while ( \
                    !(CPDQS_V(compar)(CPDQS_V(last) -= CPDQS_V(size), CPDQS_V(pivot)) < 0)); \
        } \
        \
        (already_partitioned) = (CPDQS_V(first) >= CPDQS_V(last)); \
        \
        while (CPDQS_V(first) < CPDQS_V(last)) { \
            CPDQS_SW(CPDQS_V(first), CPDQS_V(last)); \
            while(CPDQS_V(compar)(CPDQS_V(first) += CPDQS_V(size), CPDQS_V(pivot)) < 0); \
            while(!(CPDQS_V(compar)(CPDQS_V(last) -= CPDQS_V(size), CPDQS_V(pivot)) < 0)); \
        } \
        \
        CPDQS_SET(CPDQS_V(pbegin), CPDQS_SFT(CPDQS_V(first), -1)); \
        CPDQS_SET(CPDQS_SFT(CPDQS_V(first), -1), CPDQS_V(pivot)); \
        \
        (pos) = CPDQS_SFT(CPDQS_V(first), -1); \
    }

/*
Similar function to the one above, except elements equal to the pivot are
put to the left of the pivot and it doesn't check or return if the passed
sequence already was partitioned. Since this is rarely used
(the many equal case), and in that case pdqsort already has O(n) performance,
no block quicksort is applied here for simplicity.
*/
#define CPDQS_PAL(pos, base, nmemb) { \
        char *CPDQS_V(pbegin) = (base); \
        char *CPDQS_V(pend) = CPDQS_SFT(CPDQS_V(pbegin), (nmemb)); \
        void *CPDQS_V(pivot); \
        char *CPDQS_V(first) = CPDQS_V(pbegin); \
        char *CPDQS_V(last) = CPDQS_V(pend); \
        \
        CPDQS_TMP(CPDQS_V(pivot)); \
        CPDQS_SET(CPDQS_V(pivot), CPDQS_V(pbegin)); \
        \
        while (CPDQS_V(compar)(CPDQS_V(pivot), (CPDQS_V(last) -= CPDQS_V(size))) < 0); \
        \
        if (CPDQS_V(last) + CPDQS_V(size) == CPDQS_V(pend)) { \
            while ( \
                    CPDQS_V(first) < CPDQS_V(last) && \
                    !(CPDQS_V(compar)(CPDQS_V(pivot), CPDQS_V(first) += CPDQS_V(size)) < 0)); \
        } else { \
            while ( \
                    !(CPDQS_V(compar)(CPDQS_V(pivot), CPDQS_V(first) += CPDQS_V(size)) < 0)); \
        } \
        \
        while (CPDQS_V(first) < CPDQS_V(last)) { \
            CPDQS_SW(CPDQS_V(first), CPDQS_V(last)); \
            while(CPDQS_V(compar)(CPDQS_V(pivot), (CPDQS_V(last) -= CPDQS_V(size))) < 0); \
            while(!(CPDQS_V(compar)(CPDQS_V(pivot), (CPDQS_V(first) += CPDQS_V(size))) < 0)); \
        } \
        \
        CPDQS_SET(CPDQS_V(pbegin), CPDQS_V(last)); \
        CPDQS_SET(CPDQS_V(last), CPDQS_V(pivot)); \
        \
        (pos) = CPDQS_V(last); \
    }


/* Heap sort - for guaranteed in-place O(n log n). */
#define CPDQS_HSRTM(base, nmemb) { \
        size_t CPDQS_V(node), CPDQS_V(left), CPDQS_V(right); \
        size_t CPDQS_V(cur); \
        void *CPDQS_V(begin_pop) = CPDQS_V(begin); \
        CPDQS_V(begin) = (base); \
        \
        if ((nmemb) > 1) { \
            for (CPDQS_V(cur) = ((nmemb) >> 1); CPDQS_V(cur)--;) { \
                CPDQS_V(node) = CPDQS_V(cur); \
                while (1) { \
                    CPDQS_V(left)  = (CPDQS_V(node) << 1) | 1; \
                    CPDQS_V(right) = CPDQS_V(left) + 1; \
                    if (CPDQS_V(left) >= (nmemb)) { \
                        break; \
                    } \
                    if ( \
                            CPDQS_V(left) == (nmemb) - 1 || \
                            CPDQS_V(compar)(CPDQS_AT(CPDQS_V(right)), CPDQS_AT(CPDQS_V(left))) < 0) { \
                        if (CPDQS_V(compar)(CPDQS_AT(CPDQS_V(node)), CPDQS_AT(CPDQS_V(left))) < 0) { \
                            CPDQS_SW(CPDQS_AT(CPDQS_V(node)), CPDQS_AT(CPDQS_V(left))); \
                            CPDQS_V(node) = CPDQS_V(left); \
                        } else { \
                            break; \
                        } \
                    } else { \
                        if(CPDQS_V(compar)(CPDQS_AT(CPDQS_V(node)), CPDQS_AT(CPDQS_V(right))) < 0) { \
                            CPDQS_SW(CPDQS_AT(CPDQS_V(node)), CPDQS_AT(CPDQS_V(right))); \
                            CPDQS_V(node) = CPDQS_V(right); \
                        } else { \
                            break; \
                        } \
                    } \
                } \
            } \
            for (CPDQS_V(cur) = (nmemb); --CPDQS_V(cur) >= 1;) { \
                CPDQS_SW(CPDQS_AT(CPDQS_V(cur)), CPDQS_V(begin)); \
                CPDQS_V(node) = 0; \
                while (1) { \
                    CPDQS_V(left)  = (CPDQS_V(node) << 1) | 1; \
                    CPDQS_V(right) = CPDQS_V(left) + 1; \
                    if (CPDQS_V(left) >= CPDQS_V(cur)) { \
                        break; \
                    } \
                    if ( \
                            CPDQS_V(left) == CPDQS_V(cur) - 1 || \
                            CPDQS_V(compar)(CPDQS_AT(CPDQS_V(right)), CPDQS_AT(CPDQS_V(left))) < 0) { \
                        if (CPDQS_V(compar)(CPDQS_AT(CPDQS_V(node)), CPDQS_AT(CPDQS_V(left))) < 0) { \
                            CPDQS_SW(CPDQS_AT(CPDQS_V(node)), CPDQS_AT(CPDQS_V(left))); \
                            CPDQS_V(node) = CPDQS_V(left); \
                        } else { \
                            break; \
                        } \
                    } else { \
                        if(CPDQS_V(compar)(CPDQS_AT(CPDQS_V(node)), CPDQS_AT(CPDQS_V(right))) < 0) { \
                            CPDQS_SW(CPDQS_AT(CPDQS_V(node)), CPDQS_AT(CPDQS_V(right))); \
                            CPDQS_V(node) = CPDQS_V(right); \
                        } else { \
                            break; \
                        } \
                    } \
                } \
            } \
        } \
        CPDQS_V(begin) = CPDQS_V(begin_pop); \
    }


#if CPDQS_EXPORT_HEAPSORT + 0 == 1
    /* Let's pretend it's a function */
    #define heapsort(base, nmemb, _size, _compar) { \
            size_t CPDQS_V(size) = (_size); \
            void *CPDQS_V(begin) = NULL; \
            int (* CPDQS_V(compar))(void const *, void const *) = (_compar); \
            CPDQS_HSRTM((base), (nmemb)); \
        }
#endif

/* pdqsort main logic */
#define CPDQS_PDQSRTL(base, nmemb) { \
        struct CPDQS_V(pdqsort_loop_lifo) { \
            struct CPDQS_V(pdqsort_loop_lifo) *prev; \
            char *begin; \
            char *end; \
            unsigned int bad_allowed; \
            int is_leftmost; \
        } *CPDQS_V(top), *CPDQS_V(ncall); \
        char *CPDQS_V(begin), *CPDQS_V(end); \
        unsigned int CPDQS_V(bad_allowed); \
        int CPDQS_V(is_leftmost); \
        size_t CPDQS_V(tlen), CPDQS_V(s2); \
        int CPDQS_V(already_partitioned); \
        char *CPDQS_V(pivot_pos); \
        size_t CPDQS_V(l_size), CPDQS_V(r_size); \
        int CPDQS_V(highly_unbalanced); \
        int CPDQS_V(pisrt_ok); \
        \
        CPDQS_V(top) = malloc( \
            sizeof(struct CPDQS_V(pdqsort_loop_lifo))); \
        CPDQS_V(top)->prev = NULL; \
        CPDQS_V(top)->begin = (char *)(base); \
        CPDQS_V(top)->end = CPDQS_SFT(CPDQS_V(top)->begin, nmemb); \
        CPDQS_LOG2(CPDQS_V(top)->bad_allowed, CPDQS_V(size)); \
        CPDQS_V(top)->is_leftmost = 1; \
        \
        while(CPDQS_V(top) != NULL) { \
            CPDQS_V(begin) = CPDQS_V(top)->begin; \
            CPDQS_V(end) = CPDQS_V(top)->end; \
            CPDQS_V(bad_allowed) = CPDQS_V(top)->bad_allowed; \
            CPDQS_V(is_leftmost) = CPDQS_V(top)->is_leftmost; \
            CPDQS_V(ncall) = CPDQS_V(top)->prev; \
            free(CPDQS_V(top)); \
            CPDQS_V(top) = CPDQS_V(ncall); \
            \
            CPDQS_V(tlen) = CPDQS_LEN(CPDQS_V(begin), CPDQS_V(end)); \
            \
            if (CPDQS_V(tlen) < CPDQS_ISRT_THRESHOLD) { \
                if (CPDQS_V(is_leftmost)) { \
                    CPDQS_ISRT(CPDQS_V(begin), CPDQS_V(tlen)); \
                } else { \
                    CPDQS_UISRT(CPDQS_V(begin), CPDQS_V(tlen)); \
                } \
                continue; \
            } \
            \
            CPDQS_V(s2) = CPDQS_V(tlen) / 2; \
            if (CPDQS_V(tlen) > CPDQS_T9THER) { \
                CPDQS_SRT3( \
                    CPDQS_V(begin), \
                    CPDQS_SFT(CPDQS_V(begin), CPDQS_V(s2)), \
                    CPDQS_SFT(CPDQS_V(end), -1)); \
                CPDQS_SRT3( \
                    CPDQS_SFT(CPDQS_V(begin), 1), \
                    CPDQS_SFT(CPDQS_V(begin), CPDQS_V(s2) - 1), \
                    CPDQS_SFT(CPDQS_V(end), -2)); \
                CPDQS_SRT3( \
                    CPDQS_SFT(CPDQS_V(begin), 2), \
                    CPDQS_SFT(CPDQS_V(begin), CPDQS_V(s2) + 1), \
                    CPDQS_SFT(CPDQS_V(end), -3)); \
                CPDQS_SRT3( \
                    CPDQS_SFT(CPDQS_V(begin), CPDQS_V(s2) - 1), \
                    CPDQS_SFT(CPDQS_V(begin), CPDQS_V(s2)), \
                    CPDQS_SFT(CPDQS_V(begin), CPDQS_V(s2) + 1)); \
                CPDQS_SW( \
                    CPDQS_V(begin), \
                    CPDQS_SFT(CPDQS_V(begin), CPDQS_V(s2))); \
            } else { \
                CPDQS_SRT3( \
                    CPDQS_SFT(CPDQS_V(begin), CPDQS_V(s2)), \
                    CPDQS_V(begin), \
                    CPDQS_SFT(CPDQS_V(end), -1)); \
            } \
            \
            if ( \
                    !CPDQS_V(is_leftmost) && \
                    !(CPDQS_V(compar)(CPDQS_SFT(CPDQS_V(begin), -1), CPDQS_V(begin)) < 0)) { \
                CPDQS_PAL(CPDQS_V(begin), CPDQS_V(begin), CPDQS_V(tlen)); \
                CPDQS_V(begin) = CPDQS_SFT(CPDQS_V(begin), 1); \
                continue; \
            } \
            \
            CPDQS_PAR( \
                CPDQS_V(pivot_pos), CPDQS_V(already_partitioned), \
                CPDQS_V(begin), CPDQS_V(tlen)); \
            \
            CPDQS_V(l_size) = CPDQS_LEN(CPDQS_V(begin), CPDQS_V(pivot_pos)); \
            CPDQS_V(r_size) = CPDQS_LEN(CPDQS_V(pivot_pos), CPDQS_V(end)) - 1; \
            CPDQS_V(highly_unbalanced) = ( \
                CPDQS_V(l_size) < CPDQS_V(tlen) / 8 || CPDQS_V(r_size) < CPDQS_V(tlen) / 8); \
            \
            if (CPDQS_V(highly_unbalanced)) { \
                if (--CPDQS_V(bad_allowed) == 0) { \
                    CPDQS_HSRTM(CPDQS_V(begin), CPDQS_V(tlen)); \
                    continue; \
                } \
                \
                if (CPDQS_V(l_size) >= CPDQS_ISRT_THRESHOLD) { \
                    CPDQS_SW( \
                        CPDQS_V(begin), \
                        CPDQS_SFT(CPDQS_V(begin), CPDQS_V(l_size) / 4)); \
                    CPDQS_SW( \
                        CPDQS_SFT(CPDQS_V(pivot_pos), -1), \
                        CPDQS_SFT(CPDQS_V(pivot_pos), -(CPDQS_V(l_size) / 4))); \
                    \
                    if (CPDQS_V(l_size) > CPDQS_T9THER) { \
                        CPDQS_SW( \
                            CPDQS_SFT(CPDQS_V(begin), 1), \
                            CPDQS_SFT(CPDQS_V(begin), CPDQS_V(l_size) / 4 + 1)); \
                        CPDQS_SW( \
                            CPDQS_SFT(CPDQS_V(begin), 2), \
                            CPDQS_SFT(CPDQS_V(begin), CPDQS_V(l_size) / 4 + 2)); \
                        CPDQS_SW( \
                            CPDQS_SFT(CPDQS_V(pivot_pos), -2), \
                            CPDQS_SFT(CPDQS_V(pivot_pos), -(CPDQS_V(l_size) / 4 + 1))); \
                        CPDQS_SW( \
                            CPDQS_SFT(CPDQS_V(pivot_pos), -3), \
                            CPDQS_SFT(CPDQS_V(pivot_pos), -(CPDQS_V(l_size) / 4 + 2))); \
                    } \
                } \
                \
                if (CPDQS_V(r_size) >= CPDQS_ISRT_THRESHOLD) { \
                    CPDQS_SW( \
                        CPDQS_SFT(CPDQS_V(pivot_pos), 1), \
                        CPDQS_SFT(CPDQS_V(pivot_pos), 1 + CPDQS_V(r_size) / 4)); \
                    CPDQS_SW( \
                        CPDQS_SFT(CPDQS_V(end), -1), \
                        CPDQS_SFT(CPDQS_V(end), -(CPDQS_V(r_size) / 4))); \
                    \
                    if (CPDQS_V(r_size) > CPDQS_T9THER) { \
                        CPDQS_SW( \
                            CPDQS_SFT(CPDQS_V(pivot_pos), 2), \
                            CPDQS_SFT(CPDQS_V(pivot_pos), 2 + CPDQS_V(r_size) / 4)); \
                        CPDQS_SW( \
                            CPDQS_SFT(CPDQS_V(pivot_pos), 3), \
                            CPDQS_SFT(CPDQS_V(pivot_pos), 3 + CPDQS_V(r_size) / 4)); \
                        CPDQS_SW( \
                            CPDQS_SFT(CPDQS_V(end), -2), \
                            CPDQS_SFT(CPDQS_V(end), -(1 + CPDQS_V(r_size) / 4))); \
                        CPDQS_SW( \
                            CPDQS_SFT(CPDQS_V(end), -3), \
                            CPDQS_SFT(CPDQS_V(end), -(2 + CPDQS_V(r_size) / 4))); \
                    } \
                } \
            } else { \
                if (CPDQS_V(already_partitioned)) { \
                    CPDQS_PISRT( \
                        CPDQS_V(pisrt_ok), \
                        CPDQS_V(begin), \
                        CPDQS_LEN(CPDQS_V(begin), CPDQS_V(pivot_pos))); \
                    if (CPDQS_V(pisrt_ok)) { \
                        CPDQS_PISRT( \
                            CPDQS_V(pisrt_ok), \
                            CPDQS_SFT(CPDQS_V(pivot_pos), 1), \
                            CPDQS_LEN(CPDQS_V(pivot_pos), CPDQS_V(end)) - 1); \
                        if (CPDQS_V(pisrt_ok)) { \
                            continue; \
                        } \
                    } \
                } \
            } \
            \
            CPDQS_V(ncall) = malloc( \
                sizeof(struct CPDQS_V(pdqsort_loop_lifo))); \
            CPDQS_V(ncall)->prev = CPDQS_V(top); \
            CPDQS_V(ncall)->begin = CPDQS_V(begin); \
            CPDQS_V(ncall)->end = CPDQS_V(pivot_pos); \
            CPDQS_V(ncall)->bad_allowed = CPDQS_V(bad_allowed); \
            CPDQS_V(ncall)->is_leftmost = CPDQS_V(is_leftmost); \
            CPDQS_V(top) = CPDQS_V(ncall); \
            \
            CPDQS_V(ncall) = malloc( \
                sizeof(struct CPDQS_V(pdqsort_loop_lifo))); \
            CPDQS_V(ncall)->prev = CPDQS_V(top); \
            CPDQS_V(ncall)->begin = CPDQS_SFT(CPDQS_V(pivot_pos), 1); \
            CPDQS_V(ncall)->end = CPDQS_V(end); \
            CPDQS_V(ncall)->bad_allowed = CPDQS_V(bad_allowed); \
            CPDQS_V(ncall)->is_leftmost = 0; \
            CPDQS_V(top) = CPDQS_V(ncall); \
        } \
    }


/* pdqsort context-dependent macro */
#define CPDQS_PDQSRTM(base, nmemb) { \
        if (nmemb > 1) { \
            CPDQS_PDQSRTL((base), (nmemb)); \
            CPDQS_TMP_END; \
        } \
    }


/* Let's pretend it's a function */
#define pdqsort(base, nmemb, _size, _compar) { \
        size_t CPDQS_V(size) = (_size); \
        int (* CPDQS_V(compar))(void const *, void const *) = (_compar); \
        CPDQS_PDQSRTM((base), (nmemb)); \
    }


#endif  /* __CPDQSORT_H__ */
