qemu-system-i386 \
    -m 64 \
    -hda win95.qcow2 \
    -monitor unix:/tmp/qemu-monitor.sock,server,nowait \
    -display default
