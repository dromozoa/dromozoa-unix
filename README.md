# dromozoa-unix

Lua bindings for UNIX system interface.

## pthread issues

This library itself links pthread. However, glibc cannot detect pthread functions because most of lua are not built with pthread. There are two workarounds:

1. rebuild lua with pthread.
2. use environment LD_PRELOAD to load pthread.

```
% ldd /usr/bin/lua
	linux-vdso.so.1 =>  (0x00007fffb7703000)
	liblua-5.1.so => /usr/lib64/liblua-5.1.so (0x0000003acea00000)
	libreadline.so.6 => /lib64/libreadline.so.6 (0x0000003ac8e00000)
	libncurses.so.5 => /lib64/libncurses.so.5 (0x0000003ac8200000)
	libm.so.6 => /lib64/libm.so.6 (0x0000003ac7a00000)
	libdl.so.2 => /lib64/libdl.so.2 (0x0000003ac7200000)
	libc.so.6 => /lib64/libc.so.6 (0x0000003ac6a00000)
	libtinfo.so.5 => /lib64/libtinfo.so.5 (0x0000003ac8600000)
	/lib64/ld-linux-x86-64.so.2 (0x0000003ac6600000)
% export LD_PRELOAD=/lib64/libpthread.so.0
% ldd /usr/bin/lua                        
	linux-vdso.so.1 =>  (0x00007fff9333b000)
	/lib64/libpthread.so.0 (0x0000003ac6e00000)
	liblua-5.1.so => /usr/lib64/liblua-5.1.so (0x0000003acea00000)
	libreadline.so.6 => /lib64/libreadline.so.6 (0x0000003ac8e00000)
	libncurses.so.5 => /lib64/libncurses.so.5 (0x0000003ac8200000)
	libm.so.6 => /lib64/libm.so.6 (0x0000003ac7a00000)
	libdl.so.2 => /lib64/libdl.so.2 (0x0000003ac7200000)
	libc.so.6 => /lib64/libc.so.6 (0x0000003ac6a00000)
	/lib64/ld-linux-x86-64.so.2 (0x0000003ac6600000)
	libtinfo.so.5 => /lib64/libtinfo.so.5 (0x0000003ac8600000)
```
