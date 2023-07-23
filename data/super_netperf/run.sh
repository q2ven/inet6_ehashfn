for i in $(seq 1 5);
do
    for i in 64 128 256 512 1024 2048 4096;
    do
	printf "%4d: " $i;
	./super_netperf 32 -H ::1 -l 60 -- -m $i -M $i;
    done
done
