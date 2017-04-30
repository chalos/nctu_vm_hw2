
## automotive/basicmath/basicmath_small

### base
real	0m0.788s
user	0m0.000s
sys	0m0.016s

### no opt
real	0m0.850s
user	0m0.244s
sys	0m0.016s

### opt
real	0m0.857s
user	0m0.224s
sys	0m0.024s


## automotive/basicmath/basicmath_large

### base
real	0m31.865s
user	0m0.284s
sys	0m0.248s

### no opt
real	0m31.954s
user	0m7.020s
sys	0m0.512s

### opt
real	0m31.936s
user	0m6.552s
sys	0m0.528s

## automotive/bitcount/bitcnts

### base

#### iter: 75000
Bit counter algorithm benchmark

Optimized 1 bit/loop counter          > Time:   0.001 sec.; Bits: 1250098
Ratko's mystery algorithm             > Time:   0.000 sec.; Bits: 1099133
Recursive bit count by nybbles        > Time:   0.000 sec.; Bits: 1064678
Non-recursive bit count by nybbles    > Time:   0.000 sec.; Bits: 1193637
Non-recursive bit count by bytes (BW) > Time:   0.000 sec.; Bits: 1280734
Non-recursive bit count by bytes (AR) > Time:   0.000 sec.; Bits: 1095696
Shift and count bits                  > Time:   0.001 sec.; Bits: 1237855

Best  > Non-recursive bit count by bytes (BW)
Worst > Shift and count bits

real	0m0.004s
user	0m0.000s
sys	0m0.000s


#### iter: 1125000
Bit counter algorithm benchmark

Optimized 1 bit/loop counter          > Time:   0.015 sec.; Bits: 18563087
Ratko's mystery algorithm             > Time:   0.003 sec.; Bits: 17272864
Recursive bit count by nybbles        > Time:   0.006 sec.; Bits: 17116098
Non-recursive bit count by nybbles    > Time:   0.004 sec.; Bits: 18244704
Non-recursive bit count by bytes (BW) > Time:   0.002 sec.; Bits: 18730970
Non-recursive bit count by bytes (AR) > Time:   0.002 sec.; Bits: 16962481
Shift and count bits                  > Time:   0.020 sec.; Bits: 17759895

Best  > Non-recursive bit count by bytes (BW)
Worst > Shift and count bits

real	0m0.118s
user	0m0.052s
sys	0m0.000s

### no opt:

#### iter: 75000
Optimized 1 bit/loop counter          > Time:   0.009 sec.; Bits: 1250098
Ratko's mystery algorithm             > Time:   0.006 sec.; Bits: 1099133
Recursive bit count by nybbles        > Time:   0.007 sec.; Bits: 1064678
Non-recursive bit count by nybbles    > Time:   0.007 sec.; Bits: 1193637
Non-recursive bit count by bytes (BW) > Time:   0.006 sec.; Bits: 1280734
Non-recursive bit count by bytes (AR) > Time:   0.007 sec.; Bits: 1095696
Shift and count bits                  > Time:   0.013 sec.; Bits: 1237855

Best  > Non-recursive bit count by bytes (BW)
Worst > Shift and count bits

real	0m0.154s
user	0m0.068s
sys	0m0.000s


#### iter: 1125000

Bit counter algorithm benchmark

Optimized 1 bit/loop counter          > Time:   0.141 sec.; Bits: 18563087
Ratko's mystery algorithm             > Time:   0.095 sec.; Bits: 17272864
Recursive bit count by nybbles        > Time:   0.109 sec.; Bits: 17116098
Non-recursive bit count by nybbles    > Time:   0.107 sec.; Bits: 18244704
Non-recursive bit count by bytes (BW) > Time:   0.099 sec.; Bits: 18730970
Non-recursive bit count by bytes (AR) > Time:   0.097 sec.; Bits: 16962481
Shift and count bits                  > Time:   0.196 sec.; Bits: 17759895

Best  > Ratko's mystery algorithm
Worst > Shift and count bits

real	0m1.411s
user	0m0.852s
sys	0m0.000s


### opt

#### iter: 75000

Bit counter algorithm benchmark

Optimized 1 bit/loop counter          > Time:   0.005 sec.; Bits: 1250098
Ratko's mystery algorithm             > Time:   0.002 sec.; Bits: 1099133
Recursive bit count by nybbles        > Time:   0.003 sec.; Bits: 1064678
Non-recursive bit count by nybbles    > Time:   0.003 sec.; Bits: 1193637
Non-recursive bit count by bytes (BW) > Time:   0.002 sec.; Bits: 1280734
Non-recursive bit count by bytes (AR) > Time:   0.002 sec.; Bits: 1095696
Shift and count bits                  > Time:   0.008 sec.; Bits: 1237855

Best  > Ratko's mystery algorithm
Worst > Shift and count bits

real	0m0.079s
user	0m0.036s
sys	0m0.000s

#### iter: 1125000

Bit counter algorithm benchmark

Optimized 1 bit/loop counter          > Time:   0.075 sec.; Bits: 18563087
Ratko's mystery algorithm             > Time:   0.025 sec.; Bits: 17272864
Recursive bit count by nybbles        > Time:   0.042 sec.; Bits: 17116098
Non-recursive bit count by nybbles    > Time:   0.039 sec.; Bits: 18244704
Non-recursive bit count by bytes (BW) > Time:   0.029 sec.; Bits: 18730970
Non-recursive bit count by bytes (AR) > Time:   0.030 sec.; Bits: 16962481
Shift and count bits                  > Time:   0.127 sec.; Bits: 17759895

Best  > Ratko's mystery algorithm
Worst > Shift and count bits

real	0m0.742s
user	0m0.380s
sys	0m0.000s


## automotive/qsort/

### basic

#### qsort_small input_small.dat
real	0m0.081s
user	0m0.004s
sys	0m0.008s

#### qsort_large input_large.dat
real	0m3.050s
user	0m0.048s
sys	0m0.012s


### no opt

#### qsort_small input_small.dat
real	0m0.210s
user	0m0.072s
sys	0m0.008s

#### qsort_large input_large.dat
real	0m3.622s
user	0m0.740s
sys	0m0.016s

### opt

#### qsort_small input_small.dat
real	0m0.176s
user	0m0.044s
sys	0m0.016s

#### qsort_large input_large.dat
real	0m3.572s
user	0m0.656s
sys	0m0.032s

## automotive/susan

### base

chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ./susan input_small.pgm output_small.smoothing.pgm -s

real	0m0.002s
user	0m0.000s
sys	0m0.000s
chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ./susan input_small.pgm output_small.edges.pgm -e

real	0m0.001s
user	0m0.000s
sys	0m0.000s
chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ./susan input_small.pgm output_small.corners.pgm -c

real	0m0.001s
user	0m0.000s
sys	0m0.000s

chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ./susan input_large.pgm output_large.smoothing.pgm -s

real	0m0.049s
user	0m0.024s
sys	0m0.000s
chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ./susan input_large.pgm output_large.edges.pgm -e

real	0m0.008s
user	0m0.004s
sys	0m0.000s
chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ./susan input_large.pgm  output_large.corners.pgm -c

real	0m0.004s
user	0m0.000s
sys	0m0.000s

### no opt

chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ../../qemu_hw2_build/i386-linux-user/qemu-i386 ./susan input_small.pgm output_small.smoothing.pgm -s

real	0m0.048s
user	0m0.024s
sys	0m0.000s
chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ../../qemu_hw2_build/i386-linux-user/qemu-i386 ./susan input_small.pgm output_small.edges.pgm -e

real	0m0.027s
user	0m0.016s
sys	0m0.000s
chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ../../qemu_hw2_build/i386-linux-user/qemu-i386 ./susan input_small.pgm output_small.corners.pgm -c

real	0m0.020s
user	0m0.016s
sys	0m0.000s

chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ../../qemu_hw2_build/i386-linux-user/qemu-i386 ./susan input_large.pgm output_large.smoothing.pgm -s

real	0m0.291s
user	0m0.212s
sys	0m0.000s
chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ../../qemu_hw2_build/i386-linux-user/qemu-i386 ./susan input_large.pgm output_large.edges.pgm -e

real	0m0.146s
user	0m0.068s
sys	0m0.000s
chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ../../qemu_hw2_build/i386-linux-user/qemu-i386 ./susan input_large.pgm output_large.corners.pgm -c

real	0m0.106s
user	0m0.044s
sys	0m0.004s

### opt

chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ../../qemu_hw2_build/i386-linux-user/qemu-i386 ./susan input_small.pgm output_small.smoothing.pgm -s

real	0m0.057s
user	0m0.028s
sys	0m0.000s
chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ../../qemu_hw2_build/i386-linux-user/qemu-i386 ./susan input_small.pgm output_small.edges.pgm -e

real	0m0.039s
user	0m0.020s
sys	0m0.000s
chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ../../qemu_hw2_build/i386-linux-user/qemu-i386 ./susan input_small.pgm output_small.corners.pgm -c

real	0m0.039s
user	0m0.016s
sys	0m0.000s

chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ../../qemu_hw2_build/i386-linux-user/qemu-i386 ./susan input_large.pgm output_large.smoothing.pgm -s

real	0m0.295s
user	0m0.216s
sys	0m0.000s
chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ../../qemu_hw2_build/i386-linux-user/qemu-i386 ./susan input_large.pgm output_large.edges.pgm -e

real	0m0.153s
user	0m0.076s
sys	0m0.000s
chalos@chalos-VirtualBox:~/Desktop/automotive/susan$ time ../../qemu_hw2_build/i386-linux-user/qemu-i386 ./susan input_large.pgm output_large.corners.pgm -c

real	0m0.102s
user	0m0.044s
sys	0m0.004s
