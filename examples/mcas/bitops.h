#ifndef BITOPS_H
#define BITOPS_H

/*@

lemma void bitand_bitor_lemma(uintptr_t x, uintptr_t y);
    requires true == ((x & y) == 0);
    ensures y == ((x | y) & y) &*& x == ((x | y) & ~y);

lemma void bitand_bitor_1_2_lemma(void *x);
    requires true == (((uintptr_t)x & 1) == 0);
    ensures  true == ((((uintptr_t)x | 2) & 1) == 0);

@*/

#endif