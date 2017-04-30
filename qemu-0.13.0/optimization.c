/*
 *  (C) 2010 by Computer System Laboratory, IIS, Academia Sinica, Taiwan.
 *      See COPYRIGHT in top-level directory.
 */

#include <stdlib.h>
#include "exec-all.h"
#include "tcg-op.h"
#include "helper.h"
#define GEN_HELPER 1
#include "helper.h"
#include "optimization.h"

#include "qemu-queue.h"

//#define NO_DEBUG
#include "chalosdebug.h"

/* declared in: tcg-target.c
* tcg_target_qemu_prologue(TCGContext *s): optimization_ret_addr = s->code_ptr;
*/
extern uint8_t *optimization_ret_addr;

/*
 * Shadow Stack for pop function
 */
list_t *shadow_hash_list;

/* cpu-defs.h (defined in i386/cpu.h CPU_COMMON)
uint64_t *shack; [ | | | | | .. SHACK_SIZE]
uint64_t *shack_top; // insert before increase
uint64_t *shack_end; // pointed to index SHACK_SIZE 
void *shadow_hash_list; // hash(eip) -> [ | |?| | | | | ]
int shadow_ret_count;
unsigned long *shadow_ret_addr;
*/

#define HASH_INDEX_SIZE 256

static inline void shack_init(CPUState *env)
{
	INF;
	
	env->shack = (uint64_t*) malloc(SHACK_SIZE * sizeof(uint64_t));	
	env->shack_top = env->shack;
	env->shack_end = env->shack + (SHACK_SIZE * sizeof(uint64_t));

	env->shadow_ret_count = 0;
	env->shadow_ret_addr = (unsigned long*) malloc(SHACK_SIZE * sizeof(unsigned long));

	env->shadow_hash_list = calloc(HASH_INDEX_SIZE, sizeof(list_t));
	shadow_hash_list = (list_t*) env->shadow_hash_list;
	RET;
}

/*
 * shack_set_shadow()
 *  Insert a guest eip to host eip pair if it is not yet created.
 * Called by: translate-all.c: cpu_gen_code(CPUState *env, TranslationBlock *tb, int *gen_code_size_ptr)
 * calling using: shack_set_shadow(env, tb->pc, (unsigned long *)tb->tc_ptr)
 */
 void shack_set_shadow(CPUState *env, target_ulong guest_eip, unsigned long* host_eip)
{
	INF;
	DEBUG_XLONG(env);
	DEBUG_XLONG(guest_eip);
	DEBUG_XLONG(host_eip);
	DEBUG_XLONG(*host_eip);

	if(alloc_key_to_hash_list((list_t*)env->shadow_hash_list, guest_eip)) {
		DEBUG("Key existed");

		DEBUG_XLONG(get_key_value((list_t*)env->shadow_hash_list, guest_eip));
	}

	if(put_key_value((list_t*)env->shadow_hash_list, guest_eip, host_eip)) {
		DEBUG("Key Not Existed");
	}

	DEBUG_XLONG(get_key_value((list_t*)env->shadow_hash_list, guest_eip));

	RET;
}

/*
 * helper_shack_flush()
 *  Reset shadow stack.
 */
void helper_shack_flush(CPUState *env)
{
	INF;
	env->shack_top = env->shack;
	env->shadow_ret_count = 0;
	RET;
}

/*
 * push_shack()
 *  Push next guest eip into shadow stack.
 * Called By: translate.c: op case[0xe8 call rel16/32, ff-2 call r/m16/32]
 */
void push_shack(CPUState *env, TCGv_ptr cpu_env, target_ulong next_eip)
{
	INF;

	DEBUG_XLONG(env->eip);
	DEBUG_XLONG(next_eip);
	DEBUG_XLONG(env->current_tb);
	DEBUG_UINT(optimization_ret_addr);
	
	unsigned long** host_eip = get_key_location((list_t*)env->shadow_hash_list, next_eip);
	DEBUG_XLONG(host_eip);
	GENHELPER2(print_linereg, tcg_const_tl(__LINE__), tcg_const_tl(host_eip));
	
	TARGET_TRACE
	// _top_addr = env->shack_top
	TCGv top_addr = tcg_temp_local_new();
	tcg_gen_ld_tl(top_addr, cpu_env, offsetof(CPUState, shack_top));
	
	TARGET_TRACE
	// _end_addr = env->shack_end
	TCGv end_addr = tcg_temp_local_new();
	tcg_gen_ld_tl(end_addr, cpu_env, offsetof(CPUState, shack_end));

	TCGv ret_count_local = tcg_temp_local_new();
	tcg_gen_ld_tl(ret_count_local, cpu_env, offsetof(CPUState, shadow_ret_count));

	TARGET_TRACE
	// _top_addr != _end? br .stack_not_full
	int stack_not_full_label = gen_new_label();
	tcg_gen_brcond_tl(TCG_COND_NE, top_addr, end_addr, stack_not_full_label);
	tcg_gen_brcond_tl(TCG_COND_GT, ret_count_local, tcg_const_tl(SHACK_SIZE), stack_not_full_label);
	
	tcg_temp_free(ret_count_local);
	tcg_temp_free(end_addr);

	TARGET_TRACE
	// gen_helper_shack_flush(env)
	TCGv_ptr useless = tcg_temp_new_ptr();
	gen_helper_shack_flush(cpu_env);
	tcg_temp_free_ptr(useless);

	TARGET_TRACE
	// label .stack_not_full:
	gen_set_label(stack_not_full_label);

	TARGET_TRACE
	// ;push
	// _neip = valueof(next_eip)
	TCGv neip = tcg_temp_local_new();
	tcg_gen_movi_tl(neip, next_eip);

	TARGET_TRACE
	// *_top_addr = _neip
	tcg_gen_ld_tl(top_addr, cpu_env, offsetof(CPUState, shack_top));
	tcg_gen_st_tl(neip, top_addr, 0);

	tcg_temp_free(neip);

	TARGET_TRACE
	// _top_addr = _top_addr + sizeof(uint64_t)
	tcg_gen_ld_tl(top_addr, cpu_env, offsetof(CPUState, shack_top));
	tcg_gen_add_tl(top_addr, top_addr, tcg_const_tl(sizeof(uint64_t)));
	tcg_gen_st_tl(top_addr, cpu_env, offsetof(CPUState, shack_top));
	
	tcg_temp_free(top_addr);

	TARGET_TRACE
	// _ret_count = env->shadow_ret_count
	TCGv ret_count = tcg_temp_new();
	tcg_gen_ld_tl(ret_count, cpu_env, offsetof(CPUState, shadow_ret_count));
	GENHELPER2(print_linereg, tcg_const_tl(__LINE__), ret_count);

	TARGET_TRACE
	// _offset = _ret_count * sizeof(unsigned long)
	TCGv offset = tcg_temp_new();
	tcg_gen_mul_tl(offset, ret_count, tcg_const_tl(sizeof(unsigned long)) );

	TARGET_TRACE
	// *env->shadow_ret_count = ++ _ret_count
	tcg_gen_add_tl(ret_count, ret_count, tcg_const_tl(1));
	tcg_gen_st_tl(ret_count, cpu_env, offsetof(CPUState, shadow_ret_count));

	tcg_temp_free(ret_count);

	TARGET_TRACE
	// _ret_addr_base = env->shadow_ret_addr
	TCGv ret_addr_base = tcg_temp_new();
	tcg_gen_ld_tl(ret_addr_base, cpu_env, offsetof(CPUState, shadow_ret_addr));
	GENHELPER2(print_linereg, tcg_const_tl(__LINE__), ret_addr_base);

	TARGET_TRACE
	// _ttop_addr = _ret_addr_base + _offset
	TCGv ttop_addr = tcg_temp_new();
	tcg_gen_add_tl(ttop_addr, ret_addr_base, offset);
	GENHELPER2(print_linereg, tcg_const_tl(__LINE__), ttop_addr);
	
	tcg_temp_free(ret_addr_base);
	tcg_temp_free(offset);

	TARGET_TRACE
	// _next_tip = valueof( host_eip )
	TCGv next_tip = tcg_temp_new();
	tcg_gen_movi_tl(next_tip, host_eip);

	TARGET_TRACE
	// *target_top = _next_tip
	tcg_gen_st_tl(next_tip, ttop_addr, 0);
	
	tcg_temp_free(ttop_addr);
	tcg_temp_free(next_tip);

	RET;
}

/*
 * pop_shack()
 *  Pop next host eip from shadow stack.
 * Called By: translate.c: op case[0xc2 ret imm16, 0xc3 ret]
 */
//#define USE_HELPER
void pop_shack(TCGv_ptr cpu_env, TCGv next_eip)
{
	INF;
	DEBUG_XINT(cpu_env);
	DEBUG_XLONG(next_eip);
	DEBUG_UINT(optimization_ret_addr);

#ifdef USE_HELPER

	TCGv_ptr host_eip = tcg_temp_local_new_ptr();
	gen_helper_get_return(host_eip, cpu_env, next_eip);

	int end_label = gen_new_label();
	tcg_gen_brcond_tl(TCG_COND_EQ, host_eip, tcg_const_tl(0), end_label);

	*gen_opc_ptr++ = INDEX_op_jmp;
    *gen_opparam_ptr++ = GET_TCGV_PTR(host_eip);

    tcg_temp_free_ptr(host_eip);
    gen_set_label(end_label);

#else

    TARGET_TRACE
	// _next_eip_local = *_next_eip
	TCGv next_eip_local = tcg_temp_local_new();
	tcg_gen_mov_tl(next_eip_local, next_eip);
	GENHELPER2(print_linereg, tcg_const_tl(__LINE__), next_eip_local);

	TARGET_TRACE
	// _top_addr = env->shack_top
	TCGv top_addr = tcg_temp_local_new();
	tcg_gen_ld_tl(top_addr, cpu_env, offsetof(CPUState, shack_top));

	TARGET_TRACE
	// _base = env->shack
	TCGv base_addr = tcg_temp_local_new();
	tcg_gen_ld_tl(base_addr, cpu_env, offsetof(CPUState, shack));

	TARGET_TRACE
	// _top==_base? jump .end
	int end_label = gen_new_label();
	tcg_gen_brcond_tl(TCG_COND_EQ, top_addr, base_addr, end_label);

	tcg_temp_free(base_addr);

	TARGET_TRACE
	// _ret_count env->shadow_ret_count
	TCGv ret_count = tcg_temp_local_new();
	tcg_gen_ld_tl(ret_count, cpu_env, offsetof(CPUState, shadow_ret_count));

	TARGET_TRACE
	// _ret_count == 0? jmp .end
	tcg_gen_brcond_tl(TCG_COND_EQ, ret_count, tcg_const_tl(0), end_label);
	
	TARGET_TRACE
	// _top_addr = _top_addr - sizeof(uint64_t)
	tcg_gen_sub_tl(top_addr, top_addr, tcg_const_tl(sizeof(uint64_t)));
	tcg_gen_st_tl(top_addr, cpu_env, offsetof(CPUState, shack_top));

	// _ret_count = _ret_count - 1
	tcg_gen_sub_tl(ret_count, ret_count, tcg_const_tl(1));
	GENHELPER2(print_linereg, tcg_const_tl(__LINE__), ret_count);
	
	TARGET_TRACE
	// *env->shadow_ret_count = ret_count
	tcg_gen_st_tl(ret_count, cpu_env, offsetof(CPUState, shadow_ret_count));

	TARGET_TRACE
	// _top_eip = *_top_addr
	TCGv top_eip = tcg_temp_local_new();
	tcg_gen_ld_tl(top_eip, top_addr, 0);
	
	tcg_temp_free(top_addr);

	TARGET_TRACE
	// FIX ME
	// _top_eip != _next_eip? jmp .end
	tcg_gen_brcond_tl(TCG_COND_NE, next_eip_local, top_eip, end_label);
	GENHELPER2(print_linereg, tcg_const_tl(__LINE__), top_eip);
	GENHELPER2(print_linereg, tcg_const_tl(__LINE__), next_eip_local);

	tcg_temp_free(next_eip_local);
	tcg_temp_free(top_eip);

	TARGET_TRACE
	// _offset = _ret_count * sizeof(unsigned long)
	TCGv offset = tcg_temp_new();
	tcg_gen_mul_tl(offset, ret_count, tcg_const_tl(sizeof(unsigned long)) );

	tcg_temp_free(ret_count);

	TARGET_TRACE
	// _ret_addr_base = env->shadow_ret_addr
	TCGv ret_addr_base = tcg_temp_new();
	tcg_gen_ld_tl(ret_addr_base, cpu_env, offsetof(CPUState, shadow_ret_addr));
	GENHELPER2(print_linereg, tcg_const_tl(__LINE__), ret_addr_base);


	TARGET_TRACE
	// _ret_addr = _ret_addr_base + _offset
	TCGv ret_addr = tcg_temp_new();
	tcg_gen_add_tl(ret_addr, ret_addr_base, offset);
	GENHELPER2(print_linereg, tcg_const_tl(__LINE__), ret_addr);
	
	tcg_temp_free(ret_addr_base);
	tcg_temp_free(offset);

	TARGET_TRACE
	// _target_addr = *ret_addr
	TCGv target_addr = tcg_temp_local_new();
	tcg_gen_ld_tl(target_addr, ret_addr, 0);
	tcg_gen_brcond_tl(TCG_COND_EQ, target_addr, tcg_const_tl(0), end_label);

	GENHELPER2(print_linereg, tcg_const_tl(__LINE__), target_addr);
	
	tcg_temp_free(ret_addr);

	// _teip = *_target_addr
	TCGv teip = tcg_temp_local_new();
	tcg_gen_ld_tl(teip, target_addr, 0);
	GENHELPER2(print_linereg, tcg_const_tl(__LINE__), teip);

	tcg_temp_free(target_addr);

	TARGET_TRACE
	// _target_addr == 0 ? jmp .end
	tcg_gen_brcond_tl(TCG_COND_EQ, teip, tcg_const_tl(0), end_label);

	TARGET_TRACE
	// jmp _target_addr
	*gen_opc_ptr++ = INDEX_op_jmp;
	*gen_opparam_ptr++ = GET_TCGV_I32(teip);

	tcg_temp_free(teip);

	// label .end
	gen_set_label(end_label);

#endif

	RET;
}

void* helper_get_return(CPUState* env, target_ulong guest_eip) {
	INF;

	DEBUG_XLONG(guest_eip);
	DEBUG_XLONG(env->shack_top);
	DEBUG_XLONG(env->shack);
	DEBUG_XLONG(env->shadow_ret_count);

	if(env->shack_top == env->shack || env->shadow_ret_count == 0) {
		RETURN(0);
	} 

	env->shack_top--;
	env->shadow_ret_count = env->shadow_ret_count - 1;

	target_ulong top_addr = *(env->shack_top);
	DEBUG_XLONG(top_addr);
	DEBUG_XLONG(guest_eip);
	
	if(top_addr != guest_eip) {
		RETURN(0);
	}

	unsigned long* ret_addr = env->shadow_ret_addr[env->shadow_ret_count];
	DEBUG_XLONG(ret_addr);

	if(ret_addr == 0) {
		RETURN(0);
	}

	RETURN(*ret_addr);
}

/*
 * Indirect Branch Target Cache
 */
__thread int update_ibtc;

/*
 * helper_lookup_ibtc()
 *  Look up IBTC. Return next host eip if cache hit or
 *  back-to-dispatcher stub address if cache miss.
 */
void *helper_lookup_ibtc(target_ulong guest_eip)
{
	INF;
	DEBUG_UINT(*optimization_ret_addr);
	DEBUG_XLONG(guest_eip);

	TranslationBlock* tb = get_ibtc_cache_by_epi(&ibtc, guest_eip);

	if(tb != NULL) {
		DEBUG("Found eip");
		RETURN((unsigned long*)tb->tc_ptr);
	} else {
		// maybe old? update entry on next cpu_exec
		update_ibtc = 1;
		RETURN(optimization_ret_addr);
	}
}

/*
 * update_ibtc_entry()
 *  Populate eip and tb pair in IBTC entry.
 * called by: cpu-exec.c:cpu_exec
 */
void update_ibtc_entry(TranslationBlock *tb)
{
	INF;
	DEBUG_XLONG(tb->pc);
	DEBUG_XLONG(tb->cs_base);
	DEBUG_XINT(tb->cflags);
	DEBUG_XLONG(tb->page_addr[0]);
	DEBUG_XLONG(tb->page_addr[1]);
	DEBUG_UINT(optimization_ret_addr);

	update_ibtc = 0;

	set_ibtc_cache(&ibtc, tb->pc, tb);

	RET;
}

/*
 * ibtc_init()
 *  Create and initialize indirect branch target cache.
 */
static inline void ibtc_init(CPUState *env)
{
	INF;
	update_ibtc = 1;
	RET;
}

/*
 * init_optimizations()
 *  Initialize optimization subsystem.
 * called by: linux-user/main.c
 */
int init_optimizations(CPUState *env)
{
	INF;
    shack_init(env);
    ibtc_init(env);

    RETURN(0);
}

/*
* REMIND: target-i386/translate.c: gen_ibtc_stub(DisasContext *s, TCGv ibtc_guest_eip)
*/

/*
 * vim: ts=8 sts=4 sw=4 expandtab
 */


inline int alloc_key_to_hash_list(list_t* hash, unsigned long key) 
{ 
	unsigned int hashedNum = key % HASH_INDEX_SIZE;	
	list_t* hash_header = hash + hashedNum;
	
	shadow_pair_t* newsp = (shadow_pair_t*) calloc(1, sizeof(shadow_pair_t));
	list_t* newl = &newsp->l;
	newsp->guest_eip = key;
	newsp->shadow_slot = NULL;

	if(hash_header->next == NULL) {
		hash_header->next = newl;
		newl->prev = hash_header;

		return OK;
	} 
	else {
		list_t* lptr = NULL;
		shadow_pair_t* sp = NULL;

		for(lptr = hash_header->next; lptr != NULL; lptr = lptr->next) {
			sp = (shadow_pair_t*) lptr;
			
			if(sp->guest_eip == key) {
				return KEY_EXIST;
			}

			if(sp->guest_eip >= key) {
				newl->prev = lptr->prev;
				newl->next = lptr;
				lptr->prev->next = newl;
				lptr->prev = newl;

				return OK;
			}
		}
	}

	return ALLOC_ERR;
}

inline int put_key_value(list_t* hash, unsigned long key, unsigned long* value) 
{
	unsigned int hashedNum = key % HASH_INDEX_SIZE;	
	list_t* hash_header = hash + hashedNum;
	
	if(hash_header->next != NULL) {
		list_t* lptr = NULL;
		shadow_pair_t* sp = NULL;

		for(lptr = hash_header->next; lptr != NULL; lptr = lptr->next) {
			sp = (shadow_pair_t*) lptr;

			if(sp->guest_eip == key) {
				sp->shadow_slot = value;
				return OK;
			}

			if(sp->guest_eip > key) {
				return KEY_NOT_EXIST;
			} 
		}
	}

	return KEY_NOT_EXIST;
}

inline unsigned long** get_key_location(list_t* hash, unsigned long key) 
{
	alloc_key_to_hash_list(hash, key);

	unsigned int hashedNum = key % HASH_INDEX_SIZE;	
	list_t* hash_header = hash + hashedNum;
	
	list_t* lptr = NULL;
	shadow_pair_t* sp = NULL;
	
	if(hash_header->next != NULL) {
		list_t* lptr = NULL;
		shadow_pair_t* sp = NULL;

		for(lptr = hash_header->next; lptr != NULL; lptr = lptr->next) {
			sp = (shadow_pair_t*) lptr;
			
			if(sp->guest_eip == key) {
				return &sp->shadow_slot;
			}

			if(sp->guest_eip > key) {
				return NULL;
			} 
		}
	}

	return NULL;
}

inline unsigned long* get_key_value(list_t* hash, unsigned long key) 
{
	unsigned int hashedNum = key % HASH_INDEX_SIZE;	
	list_t* hash_header = hash + hashedNum;
	
	list_t* lptr = NULL;
	shadow_pair_t* sp = NULL;
	
	if(hash_header->next != NULL) {
		list_t* lptr = NULL;
		shadow_pair_t* sp = NULL;

		for(lptr = hash_header->next; lptr != NULL; lptr = lptr->next) {
			sp = (shadow_pair_t*) lptr;
			
			if(sp->guest_eip == key) {
				return sp->shadow_slot;
			}

			if(sp->guest_eip > key) {
				return NULL;
			} 
		}
	}

	return NULL;
}

inline TranslationBlock* get_ibtc_cache_by_epi(ibtc_table_t* target, target_ulong key) 
{
	unsigned int index = key % IBTC_CACHE_SIZE;
	struct jmp_pair* jp = &(target->htable[index]);

	if(jp->guest_eip == key) {
		return jp->tb;
	} else {
		return NULL;
	}
}

inline TranslationBlock* set_ibtc_cache(ibtc_table_t* target, target_ulong key, TranslationBlock* tb) {
	unsigned int index = key % IBTC_CACHE_SIZE;

	TranslationBlock* replaced_tb = target->htable[index].tb;

	struct jmp_pair* jp = &(target->htable[index]);
	jp->guest_eip = tb->pc;
	jp->tb = tb;

	return replaced_tb;
}

void helper_print_line(int line_num) 
{
	DEBUG_UINT(line_num);
}

void helper_print_linereg(int line_num, target_ulong reg) {
	DEBUG_LINE_XLONG(line_num, reg);
}
