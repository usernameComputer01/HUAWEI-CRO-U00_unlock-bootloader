# HUAWEI-CRO-U00_unlock-bootloader

### Build
```
$ git clone https://github.com/usernameComputer01/HUAWEI-CRO-U00_unlock-bootloader
$ cd HUAWEI-CRO-U00_unlock-bootloader
$ make
```

### Usage:
```
* ./code-HWCRO <u:0123456789AABBCCDDEEFF0A1B2C3D4E> [ DEBUG (0|1) ]
* ./code-HWCRO <l:0123456789ABCDEF>                 [ DEBUG (0|1) ]
* ./code-HWCRO <f:path/to/proinfo.bin>              [ DEBUG (0|1) ]
```

### Example:
```
$ hexdump -C -n 16 -s $((0x200)) proinfo.bin
00000200  30 98 8c 66 43 a9 d8 6e  38 9c 50 48 34 22 15 8c  |0..fC..n8.PH4"..|
00000210
```

Current code is: 30988c6643a9d86e389c50483422158c (remove space)
```
$ ./code-HWCRO u:30988c6643a9d86e389c50483422158c
[+] Key unlock: (0123456789ABCDEF)
```

Read code from **proinfo.bin** (/dev/block/platform/mtk-msdc.0/11120000.msdc0/by-name/proinfo)
```
$ adb pull /dev/block/platform/mtk-msdc.0/11120000.msdc0/by-name/proinfo proinfo.bin
$ ./code-HWCRO f:/var/proinfo.bin
[+] Key unlock: (0123456789ABCDEF)
```

If need get custom unlock code 0123456789ABCDEF, for example, to write to **proinfo.bin**
```
$ ./code-HWCRO l:0123456789ABCDEF 1
[+] Key unlock (hex): (30988c6643a9d86e389c50483422158c)
[*] (Debug) HEX: (30 98 8C 66 43 A9 D8 6E 38 9C 50 48 34 22 15 8C)
```

Now the unlocking code can be used
```
$ fastboot oem unlock 0123456789ABCDEF
```
