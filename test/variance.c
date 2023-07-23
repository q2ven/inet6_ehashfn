#include <stdio.h>

#include <common.h>

#define HASH_SIZE 0xFFFF
#define HIST_SIZE 10
#define HIST_MAX  (1024 * 1024)

void histogram(u32 hashes[])
{
	u32 counts[HASH_SIZE / HIST_SIZE] = {};
	int i, j, max = 0;

	for (i = 0; i < HIST_SIZE; i++)
		for (j = 0; j < HASH_SIZE / HIST_SIZE; j++)
			counts[i] += hashes[i * HASH_SIZE / HIST_SIZE + j];

	for (i = 0; i < HIST_SIZE; i++) {
		printf("%5d - %5d : %10d : ",
		       i * HASH_SIZE / HIST_SIZE,
		       (i + 1) * HASH_SIZE / HIST_SIZE - 1,
		       counts[i]);

		for (j = 0; j < counts[i] * 100 / HIST_MAX; j++)
			printf("*");

		printf("\n");
	}

	printf("\n");

}

int main(void)
{
	struct in6_addr laddr = {}, faddr = {};
	u32 hash, hashes[HASH_SIZE] = {};
	struct net rootns;
	u32 *ptr;
	int i;

	for (i = 0; i < HIST_MAX; i++) {
		ptr = (u32 *)&faddr;
		ptr[0] = i;
		ptr[1] = i;

		hash = inet6_ehashfn(&rootns, &laddr, 0, &faddr, 0);
		hashes[hash % HASH_SIZE]++;
	}

	histogram(hashes);
}
