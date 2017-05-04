
# Report of VM HW2

> 0456147, 0556100

## Data Structure

### Shadow Stack for Guest Return Address

- Use `env->shack` to store return addresses, which have 16 * 1024 indecies and 8 bytes in size per index

````C
env->shack = (uint64_t*) malloc(SHACK_SIZE * sizeof(uint64_t));	
````

- Two pointers pointed to next input of stack `env->top` and end of stack `env->end`
- `env-top` will be changed by time; while `env-end` is fixed.

````C
env->shack_top = env->shack;
env->shack_end = env->shack + (SHACK_SIZE * sizeof(uint64_t));
````

- Sequences of pushing a value into `env-shack`

````python
# Stack should be checked if it isnt full before perform push

push_guest():
	stack_top = value
	stack_top = stack_top + 1
````

- Sequences of popping a value from `env-shack`

````python
# Stack should be checked if it isnt empty before perform pop

pop_guest():
	stack_top = stack_top - 1
	value = stack_top
````


### Shadow Stack for Target Return Address

- Just another stack in array form, same indecies number as `env->shack`
- Value stored in this stack is a pointer to the field located in hash list, which holding the address to code cache

````C
env->shadow_ret_count = 0;
env->shadow_ret_addr = (unsigned long*) malloc(SHACK_SIZE * sizeof(unsigned long));
````

- Sequences of pushing a value into `env-shadow_ret_addr`

````python
# shadow_ret_count should be checked if it isnt SHACK_SIZE before perform push

push_target():
	shadow_ret_addr[shadow_ret_count] = value
	shadow_ret_count = shadow_ret_count + 1
````

- Sequences of popping a value from `env-shack`

````python
# shadow_ret_count should be checked if it isnt 0 before perform pop

pop_target():
	shadow_ret_count = shadow_ret_count - 1
	value = shadow_ret_addr[shadow_ret_count]
````

### Hash List for Storing Guest Address and Target Address Pairs

- The hash list mentioned above
- It contains a array contains 256 index size, each index maintaining a ordered double linked-list which have same hash value
- To acquire the hash value according to key:

````python
hash(key):
	hash_index_size = 256
	hash = key % hash_index_size
```` 

#### Allocate a key into hash list

````C
int alloc_key_to_hash_list(list_t* hash, unsigned long key)
````

- Function `alloc_key_to_hash_list` will add a new key-value pair with empty value if key not exists
- Pointer of hash list and key as input, status as return
- Return 0 if allocate successfully, 1 if key inputed exists. 

#### Set value 
````C
int put_key_value(list_t* hash, unsigned long key, unsigned long* value)
````

- Function `put_key_value` will add a new key-value pair with value if key exists
- Return 0 if allocate successfully, 2 if key inputed not exists. 

#### Get Key Location
````C
unsigned long** get_key_location(list_t* hash, unsigned long key);
````

- Get the memory reference of value field of key-value pair

### Indirect Branch Target Cache

- Structure as shown below:

````C
struct jmp_pair
{
    target_ulong guest_eip;
    TranslationBlock *tb;
}

struct ibtc_table
{
    struct jmp_pair htable[IBTC_CACHE_SIZE]; // 1 << 16
}
typedef struct ibtc_table ibtc_table_t;

ibtc_table_t ibtc;
```` 

- Hash Table will used by `helper_get_return` function to perform faster target address lookup
- To acquire the hash value according to key:

````python
hash(key):
	IBTC_CACHE_SIZE = 1 << 16
	hash = key % IBTC_CACHE_SIZE
```` 

- When the slot to insert is not empry, it will simply replace the slot with new value


## Implementation

### push_shack

- Implementation of `void push_shack(CPUState *env, TCGv_ptr cpu_env, target_ulong next_eip)`
````asm
; Before calling TCG API, lookup memory reference of key-value pair in hash list
	host_eip = get_key_location(hash_list, next_eip)

; TCG seqences
	_top_addr = env->shack_top
	_end_addr = env->shack_end
	(_top_addr != _end)? br .stack_not_full
	
	gen_helper_shack_flush(env)
.stack_not_full:
	
	_neip = valueof(next_eip)
	*_top_addr = _neip
	_top_addr = _top_addr + sizeof(uint64_t)
	
	_ret_count = env->shadow_ret_count
	*env->shadow_ret_count = ++ _ret_count
	
	_offset = (_ret_count-1) * sizeof(unsigned long)
	_ret_addr_base = env->shadow_ret_addr
	_ttop_addr = _ret_addr_base + _offset
	*_ttop_addr = host_eip
````

### pop stack

- Implementation of `void pop_shack(TCGv_ptr cpu_env, TCGv next_eip)`
````asm
	; _next_tip is arg passed in, moving it to local register to avoid branch error
	_next_eip_local = *_next_eip

	; check if it is empty
	_top_addr = env->shack_top
	_base = env->shack
	(_top==_base)? jump .end
	
	; check if shadow count is 0
	_ret_count env->shadow_ret_count
	(_ret_count == 0)? jmp .end

	; Dec and Store
	_ret_count = _ret_count - 1
	*env->shadow_ret_count = ret_count

	; check if guest address on stack top is same as guest eip passed in
	_top_addr = _top_addr - sizeof(uint64_t)
	_top_eip = *_top_addr
	(_top_eip != _next_eip_local?) jmp .end

	_offset = _ret_count * sizeof(unsigned long)
	_ret_addr_base = env->shadow_ret_addr
	_ret_addr = _ret_addr_base + _offset
	
	_target_addr = *_ret_addr
	_teip = *_target_addr
	
	(_target_addr == 0)? jmp .end
	jmp _target_addr
.end: 
````

### shack_set_shadow
````C
void shack_set_shadow(CPUState *env, target_ulong guest_eip, unsigned long* host_eip)
````

- Simply call `alloc_key_to_hash_list` and `put_key_value`


## Performance Test

- Test using MiBench's automotive benchmark
- Have most significant performance improvement in bitcount test
