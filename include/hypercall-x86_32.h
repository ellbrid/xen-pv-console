#include <xen/xen.h>
#include <xen/sched.h>

#define __STR(x) #x
#define STR(x) __STR(x)
typedef struct { unsigned long pte_low, pte_high; } pte_t;

#define __pte(x) ({ unsigned long long _x = (x);        \
    ((pte_t) {(unsigned long)(_x), (unsigned long)(_x>>32)}); })


#define _hypercall1(type, name, a1) \
({\
    long __res, __ign1; \
    __asm volatile (\
        "call hypercall_page + ("STR(__HYPERVISOR_##name)" * 32)"\
        : "=a" (__res), "=b" (__ign1) \
        : "1" ((long)(a1)) \
        : "memory" \
    );\
    (type)__res;\
})

#define _hypercall2(type, name, a1, a2) \
({\
    long __res, __ign1, __ign2; \
    __asm volatile (\
        "call hypercall_page + ("STR(__HYPERVISOR_##name)" * 32)"\
        : "=a" (__res), "=b" (__ign1), "=c" (__ign2) \
        : "1" ((long)(a1)), "2" ((long)(a2)) \
        : "memory" \
    );\
    (type)__res;\
})

#define _hypercall3(type, name, a1, a2, a3) \
({\
    long __res, __ign1, __ign2, __ign3; \
    __asm volatile (\
        "call hypercall_page + ("STR(__HYPERVISOR_##name)" * 32)"\
        : "=a" (__res), "=b" (__ign1), "=c" (__ign2), "=d" (__ign3) \
        : "1" ((long)(a1)), "2" ((long)(a2)), "3" ((long)(a3)) \
        : "memory" \
    );\
    (type)__res;\
})

#define _hypercall4(type, name, a1, a2, a3, a4) \
({ \
    long __res, __ign1, __ign2, __ign3, __ign4;		\
    __asm volatile ( \
        "call hypercall_page + ("STR(__HYPERVISOR_##name)" * 32)"\
        : "=a" (__res), "=b" (__ign1), "=c" (__ign2), "=d" (__ign3), "=S" (__ign4) \
        : "1" ((long)(a1)), "2" ((long)(a2)), "3" ((long)(a3)), "4" ((long)(a4)) \
        : "memory" ); \
    (type)__res; \
})

static inline int
HYPERVISOR_console_io(int cmd, int count, char *str)
{
    return _hypercall3(int, console_io, cmd, count, str);
}

static inline int
HYPERVISOR_update_va_mapping(unsigned long va, pte_t new_val, unsigned long flags)
{
    return _hypercall4(int, update_va_mapping, va, new_val.pte_low, new_val.pte_high, flags);
}

static inline int
HYPERVISOR_set_trap_table(trap_info_t *table)
{
    return _hypercall1(int, set_trap_table, table);
}

static inline int
HYPERVISOR_sched_op(int cmd, void *arg)
{
    return _hypercall2(int, sched_op, cmd, arg);
}

static inline int
HYPERVISOR_event_channel_op(int cmd, void *op)
{
    return _hypercall2(int, event_channel_op, cmd, op);
}

static inline int
HYPERVISOR_xen_version(int cmd, void *arg)
{
    return _hypercall2(int, xen_version, cmd, arg);
}

static inline int
HYPERVISOR_set_callbacks(
	unsigned long event_selector, unsigned long event_address,
	unsigned long failsafe_selector, unsigned long failsafe_address)
{
	return _hypercall4(int, set_callbacks,
			   event_selector, event_address,
			   failsafe_selector, failsafe_address);
}