/*
 *  (C) 2010 by Computer System Laboratory, IIS, Academia Sinica, Taiwan.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef __OPTIMIZATION_H
#define __OPTIMIZATION_H

/* Comment the next line to disable optimizations. */
#define ENABLE_OPTIMIZATION

/*
 * Link list facilities
 */
struct list_head {
    struct list_head *next, *prev;
};
typedef struct list_head list_t;

static const int OK = 0;
static const int KEY_EXIST = 1;
static const int KEY_NOT_EXIST = 2;
static const int ALLOC_ERR = 3;
inline int alloc_key_to_shadow_hash(list_t* hash, unsigned long key);
inline int put_key_value(list_t* hash, unsigned long key, unsigned long* value);
inline unsigned long* get_key_value(list_t* hash, unsigned long key);
inline unsigned long** get_key_location(list_t* hash, unsigned long key);

/*
 * Shadow Stack
 */

#if TCG_TARGET_REG_BITS == 32
#define tcg_gen_st_ptr          tcg_gen_st_i32
#define tcg_gen_brcond_ptr      tcg_gen_brcond_i32
#define tcg_temp_free_ptr       tcg_temp_free_i32
#define tcg_temp_local_new_ptr  tcg_temp_local_new_i32
#else
#define tcg_gen_st_ptr          tcg_gen_st_i64
#define tcg_gen_brcond_ptr      tcg_gen_brcond_i64
#define tcg_temp_free_ptr       tcg_temp_free_i64
#define tcg_temp_local_new_ptr  tcg_temp_local_new_i64
#endif

#if TARGET_LONG_BITS == 32
#define TCGv TCGv_i32
#else
#define TCGv TCGv_i64
#endif

#define MAX_CALL_SLOT   (16 * 1024)
#define SHACK_SIZE      (16 * 1024)

struct shadow_pair
{
    struct list_head l;
    target_ulong guest_eip;
    unsigned long *shadow_slot;
};
typedef struct shadow_pair shadow_pair_t;

void shack_set_shadow(CPUState *env, target_ulong guest_eip, unsigned long *host_eip);
inline void insert_unresolved_eip(CPUState *env, target_ulong next_eip, unsigned long *slot);
unsigned long lookup_shadow_ret_addr(CPUState *env, target_ulong pc);
void push_shack(CPUState *env, TCGv_ptr cpu_env, target_ulong next_eip);
void pop_shack(TCGv_ptr cpu_env, TCGv next_eip);

/*
 * Indirect Branch Target Cache
 */
#define IBTC_CACHE_BITS     (16)
#define IBTC_CACHE_SIZE     (1U << IBTC_CACHE_BITS)
#define IBTC_CACHE_MASK     (IBTC_CACHE_SIZE - 1)

/*
* ibtc index
*/
struct jmp_pair
{
    target_ulong guest_eip;
    TranslationBlock *tb;
};

/*
* ibtc hashtable<jmp_pair<epi,tb>>
*/
struct ibtc_table
{
    struct jmp_pair htable[IBTC_CACHE_SIZE];
};
typedef struct ibtc_table ibtc_table_t;

static ibtc_table_t ibtc;

int init_optimizations(CPUState *env);
void update_ibtc_entry(TranslationBlock *tb);
inline TranslationBlock* get_ibtc_cache_by_epi(ibtc_table_t* target, target_ulong key);
inline TranslationBlock* set_ibtc_cache(ibtc_table_t* target, target_ulong key, TranslationBlock* tb);

#endif

/*
 * vim: ts=8 sts=4 sw=4 expandtab
 */
