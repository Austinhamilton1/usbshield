#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <bpf/libbpf.h>
#include "usb.h"
#include "usb.skel.h"

static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args) {
    if(level >= LIBBPF_DEBUG)
        return 0;

    return vfprintf(stderr, format, args);
}

int handle_event(void *ctx, void *data, unsigned long data_sz) {
    struct usb_t *u = data;

    printf("Name: %s was probed.\n", u->name);

    return 0;
}

int main() {
    struct usb_bpf *skel;
    int err;
    struct ring_buffer *rb = NULL;

    libbpf_set_print(libbpf_print_fn);

    skel = usb_bpf__open_and_load();
    if(!skel) {
        printf("Failed to open BPF object\n");
        return 1;
    }

    err = usb_bpf__attach(skel);
    if(err) {
        fprintf(stderr, "Failed to attach BPF skeleton: %d\n", err);
        usb_bpf__destroy(skel);
        return 1;
    }

    rb = ring_buffer__new(bpf_map__fd(skel->maps.output), handle_event, NULL, NULL);
    if(!rb) {
        err = -1;
        fprintf(stderr, "Failed to create ring buffer\n");
        usb_bpf__destroy(skel);
        return 1;
    }

    while(true) {
        err = ring_buffer__poll(rb, 100);
        if(err == -EINTR) {
            err = 0;
            break;
        }
        if(err < 0) {
            printf("Error polling ring buffer: %d\n", err);
            break;
        }
    }

    ring_buffer__free(rb);
    usb_bpf__destroy(skel);
    return -err;
}