#include "/usr/lib/gcc/x86_64-redhat-linux/4.8.2/include/stddef.h"
#include "event.h"
#include "console.h"
#include "time.h"
#include "printk.h"

char stack[8192];

extern shared_info_t shared_info;
shared_info_t * HYPERVISOR_shared_info;

void start_kernel(start_info_t *start_info)
{
    struct timeval tp;
    struct tm datetime;
    HYPERVISOR_update_va_mapping((unsigned long) &shared_info,
            __pte(start_info->shared_info | 7),
            UVMF_INVLPG);
    HYPERVISOR_shared_info = &shared_info;
    init_events();
    console_init(start_info);
    printk("Hello from printk!\r\n");

    if (gettimeofday(&tp, 0) != 0) {
        printk("#: gtod FAILED\r\n");
    } else {
        convert(tp.tv_sec, &datetime);
        printk("%d/%02d/%02d %02d:%02d:%02d\r\n", 
            datetime.tm_year, datetime.tm_mon, datetime.tm_mday, 
            datetime.tm_hour, datetime.tm_min, datetime.tm_sec);
    }

    while (1)
    {
        HYPERVISOR_sched_op(SCHEDOP_block, 0);
    }
    
}
