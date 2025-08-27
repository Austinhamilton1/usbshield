#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>

#include "usb.h"

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);
} output SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);
    __type(key, struct usb_t);
    __type(value, u32);

} device_lookup SEC(".maps");

SEC("lsm/device_bind_driver")
int BPF_PROG(block_usb, struct device *dev, struct device_driver *drv) {
    struct usb_t *data;

    data = bpf_ringbuf_reserve(&output, sizeof(*data), 0);
    if(!data)
        return 0;

    const char *busname = BPF_CORE_READ(dev, bus, name);
    char tmp[4] = {};
    if(busname) {
        bpf_probe_read_kernel_str(tmp, sizeof(tmp), busname);
        if(tmp[0] == 'u' && tmp[1] == 's' && tmp[2] == 'b') {
            struct usb_device *udev = (struct usb_device *)dev;
            const char *prod = BPF_CORE_READ(udev, product);
            if(prod) {
                bpf_probe_read_kernel_str(data->name, sizeof(data->name), prod);
            }
        }
    }

    bpf_ringbuf_submit(data, 0);

    return 0;
}

char LICESNSE[] SEC("license") = "Dual BSD/GPL";