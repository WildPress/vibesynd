/* framed @ 0x3b99e: search a singly-linked list rooted at g_11e4c for the node
   whose key (node[+4]) == param_1. Not found -> return -1. Found -> tail-call
   FUN_0003b9ca(param_1, 1) and return its result.

   NEAR-MISS 42/44 bytes (same length), recipe `-3s -os -zp8 -s -zq`. Prologue,
   the param-first load order (`int k`), the top-tested loop, the `param==key`
   operand orientation, the out-of-line found block (`goto found`), and the call
   are ALL byte-perfect. Sole diff at 0x14: target's -1 path shares the epilogue
   (`mov eax,-1; eb14 jmp` -> final `5dc3`), ours emits its own `5dc3 pop;ret`.
   Both 2 bytes, so identical size -- a pure shared-epilogue / tail-merge WALL
   (playbook s3): 9.5b won't cross-jump these two identical `pop ebp;ret` tails
   under any tried source form or flag. */
struct node { struct node *next; int key; };
extern struct node *g_11e4c;
extern int FUN_0003b9ca(int, int);

int FUN_0003b99e(int param_1)
{
    int k = param_1;
    struct node *p = g_11e4c;

    for (;;) {
        if (p == 0)
            return -1;
        if (k == p->key)
            goto found;
        p = p->next;
    }
found:
    return FUN_0003b9ca(k, 1);
}
