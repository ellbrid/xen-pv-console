#include "console.h"
#include <xen/sched.h>
#include <barrier.h>
#include "event.h"

static evtchn_port_t console_evt;
extern char _text;
struct xencons_interface * console;

int console_init(start_info_t *start)
{
    console = (struct xencons_interface*)
        ((machine_to_phys_mapping[start->console.domU.mfn] << 12)
         +
        ((unsigned long)&_text));
    console_evt = start->console.domU.evtchn;
    return 0;
}

void putchar(char ch)
{
    struct evtchn_send event;
    event.port = console_evt;
    // Wait for the backend to clear enough space in buffer
    XENCONS_RING_IDX data;
    do
    {
        data = console->out_prod - console->out_cons;
        HYPERVISOR_event_channel_op(EVTCHNOP_send, &event);
        mb();
    } while (data >= sizeof(console->out));
    int ring_idx = MASK_XENCONS_IDX(console->out_prod, console->out);
    console->out[ring_idx] = ch;
    wmb();
    console->out_prod++;
    HYPERVISOR_event_channel_op(EVTCHNOP_send, &event);
}

void console_flush(void)
{
    while (console->out_cons < console->out_prod)
    {
        HYPERVISOR_sched_op(SCHEDOP_yield, 0);
        mb();
    }
}