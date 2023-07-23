# inet6_ehashfn

The base commit is `1671bcfd76fd` in net-next.git.

In simulations with `llvm-mca`, hsiphash and siphash are actually slower in Total Cycles, ~10% and 20%+ respectively.
However, it's not dominant while benchmarking with `super_netperf`.


## llvm-mca

Note `hsiphash_4u32()` and `siphash_2u64()` are inlined in simulation as `llvm-mca` does not support `call`.

```
$ make
$ cd data/llvm-mca/
$ python3 analyse.py
```

```
skylake             	base		jhash2                  hsiphash                siphash                 
Iterations          	  1.00		  1.00 (100.00)		  1.00 (100.00)		  1.00 (100.00)		
Instructions        	104.00		137.00 (131.73)		166.00 (159.62)		222.00 (213.46)		
Total Cycles        	155.00		171.00 (110.32)		168.00 (108.39)		191.00 (123.23)		
Total uOps          	132.00		166.00 (125.76)		192.00 (145.45)		248.00 (187.88)		
Dispatch Width      	  6.00		  6.00 (100.00)		  6.00 (100.00)		  6.00 (100.00)		
uOps Per Cycle      	  0.85		  0.97 (114.12)		  1.14 (134.12)		  1.30 (152.94)		
IPC                 	  0.67		  0.80 (119.40)		  0.99 (147.76)		  1.16 (173.13)		

nehalem             	base		jhash2                  hsiphash                siphash                 
Iterations          	  1.00		  1.00 (100.00)		  1.00 (100.00)		  1.00 (100.00)		
Instructions        	104.00		137.00 (131.73)		166.00 (159.62)		222.00 (213.46)		
Total Cycles        	173.00		182.00 (105.20)		190.00 (109.83)		218.00 (126.01)		
Total uOps          	130.00		170.00 (130.77)		215.00 (165.38)		295.00 (226.92)		
Dispatch Width      	  4.00		  4.00 (100.00)		  4.00 (100.00)		  4.00 (100.00)		
uOps Per Cycle      	  0.75		  0.93 (124.00)		  1.13 (150.67)		  1.35 (180.00)		
IPC                 	  0.60		  0.75 (125.00)		  0.87 (145.00)		  1.02 (170.00)	
```

In the real kernel, `inet6_ehashfn` calls each function, but both functions do not have jumps.

hsiphash:
```
$ gdb -batch -ex 'file vmlinux' -ex 'disassemble inet6_ehashfn' | grep hsiphash
   0xffffffff81de5bcc <+92>:	call   0xffffffff81f637e0 <hsiphash_4u32>
$ gdb -batch -ex 'file vmlinux' -ex 'disassemble hsiphash_4u32' | grep j
   0xffffffff81f63970 <+400>:	jmp    0xffffffff81f85ec0 <__x86_return_thunk>
```

siphash:
```
$ gdb -batch -ex 'file vmlinux' -ex 'disassemble inet6_ehashfn' | grep siphash
   0xffffffff81de5bc4 <+84>:	call   0xffffffff81f625e0 <siphash_2u64>
$ gdb -batch -ex 'file vmlinux' -ex 'disassemble siphash_2u64' | grep j
   0xffffffff81f62837 <+599>:	jmp    0xffffffff81f85ec0 <__x86_return_thunk>
```


## super_netperf

Run super_netperf with each `patch/` applied:

```
for i in $(seq 1 5);
do
    for i in 64 128 256 512 1024 2048 4096;
    do
	printf "%4d: " $i;
	./super_netperf 32 -H ::1 -l 60 -- -m $i -M $i;
    done
done
```

Each result is saved in `data/super_netperf/` and here are average.

```
size	base		jhash2                  hsiphash                siphash                 
64	20735.80	20419.96 (98.48)	20335.60 (98.07)	20404.08 (98.40)	
128	40450.40	40167.66 (99.30)	39957.32 (98.78)	40222.06 (99.44)	
256	73740.84	74219.26 (100.65)	73625.76 (99.84)	74081.52 (100.46)	
512	142075.60	142461.80 (100.27)	141237.00 (99.41)	141789.60 (99.80)	
1024	254391.00	253553.20 (99.67)	252751.00 (99.36)	253718.00 (99.74)	
2048	421221.20	421175.60 (99.99)	420142.80 (99.74)	418189.20 (99.28)	
4096	599158.20	597958.20 (99.80)	594472.20 (99.22)	598591.00 (99.91)
```


## Hash Variance

Test 1048576 foreign addresses with same local address, local port, and foreign port.
Each foreign address has `addr[0] == addr[1]`, thus `addr[0] ^ addr[1] == 0`, and the tuple will always have the same hash on the base commit.

```
$ make
...
./test/test_base
    0 -  6552 :          0 : 
 6553 - 13106 :          0 : 
13107 - 19659 :          0 : 
19660 - 26213 :          0 : 
26214 - 32766 :          0 : 
32767 - 39320 :          0 : 
39321 - 45873 :          0 : 
45874 - 52427 :          0 : 
52428 - 58980 :    1048576 : ****************************************************************************************************
58981 - 65534 :          0 : 

./test/test_jhash2
    0 -  6552 :     105133 : **********
 6553 - 13106 :     104608 : *********
13107 - 19659 :     104990 : **********
19660 - 26213 :     105347 : **********
26214 - 32766 :     104384 : *********
32767 - 39320 :     105046 : **********
39321 - 45873 :     105246 : **********
45874 - 52427 :     104105 : *********
52428 - 58980 :     104707 : *********
58981 - 65534 :     104924 : **********

./test/test_hsiphash
    0 -  6552 :     105120 : **********
 6553 - 13106 :     104649 : *********
13107 - 19659 :     104640 : *********
19660 - 26213 :     104860 : **********
26214 - 32766 :     104571 : *********
32767 - 39320 :     104951 : **********
39321 - 45873 :     104890 : **********
45874 - 52427 :     105153 : **********
52428 - 58980 :     104869 : **********
58981 - 65534 :     104787 : *********

./test/test_siphash
    0 -  6552 :     104866 : **********
 6553 - 13106 :     104806 : *********
13107 - 19659 :     104664 : *********
19660 - 26213 :     104865 : **********
26214 - 32766 :     105197 : **********
32767 - 39320 :     105204 : **********
39321 - 45873 :     104779 : *********
45874 - 52427 :     104611 : *********
52428 - 58980 :     104796 : *********
58981 - 65534 :     104687 : *********
```