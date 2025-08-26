# usbshield

eBPF-driven USB sandboxing and monitoring.

usbshield provides a programmable kernel-level security layer for USB devices. By attaching eBPF programs to USB-related kernel hooks, it enables:

- Sandboxing: Isolate and restrict USB device behavior before it reaches user space.

- Policy Enforcement: Define fine-grained rules for allowed/denied devices, endpoints, or operations.

- Telemetry: Collect real-time USB event data for monitoring, auditing, or intrusion detection.

This project aims to make USB interactions safer by combining the flexibility of eBPF with practical, low-overhead device control.
