#include "vmlinux.h"
#include <bpf/bpf_tracing.h>

#include "usb.h"

struct {
    __uint(type, BPF_MAP_TYPE_PERF_EVENT_ARRAY);
    __uint(key_size, sizeof(u32));
    __uint(value_size, sizeof(u32));
} output SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);
    __type(key, struct usb_t);
    __type(value, u32);

} device_lookup SEC(".maps");

SEC("kprobe/driver_probe_device")
int BPF_KPROBE(stop_bind, struct device *dev) {
    struct usb_t data = {};
    u32 bind = 0;

    bpf_probe_read_kernel_str(data.name, sizeof(data.name), dev->init_name);
    bpf_probe_read_kernel_str(data.device_type, sizeof(data.device_type), dev->type->name);
    bpf_probe_read_kernel_str(data.usb_type, sizeof(data.usb_type), dev->bus->name);

    bpf_map_update_elem(&device_lookup, &data, &bind, BPF_ANY);

    bpf_perf_event_output(ctx, &output, BPF_F_CURRENT_CPU, &data, sizeof(data));

    return 0;
}