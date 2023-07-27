#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#define CACHE_LINE_SIZE 64
#define NLOOP (4*1024UL*1024*1024)
#define NSECS_PER_SEC 1000000000UL

static inline long diff_nsec(struct timespec before, struct timespec after) {
	return ((after.tv_sec * NSECS_PER_SEC + after.tv_nsec) - (before.tv_sec * NSECS_PER_SEC + before.tv_nsec));
}

int main(int argc, char* argv[]){
            // 첫 번째로, 프로그램은 입력을 확인합니다. 이 프로그램은 입력으로 받은 값이 하나만 있어야 합니다.
	    // 이 값은 캐시 테스트를 수행할 버퍼의 크기를 킬로바이트(KB) 단위로 결정합니다.
	    //     // 입력이 잘못되었거나 없으면, 프로그램은 사용자에게 올바른 사용법을 알리고 종료됩니다.
	    //
	    //         // 다음으로, 프로그램은 mmap() 함수를 사용하여 메모리에서 버퍼를 할당하고 이에 대한 포인터를 가져옵니다.
	    //             // 이 때, 메모리 할당에 실패하면 에러 메시지를 출력하고 프로그램을 종료합니다.
	    //
	    //                 // 이제 프로그램은 CLOCK_MONOTONIC을 사용하여 현재 시간을 얻고, 이를 'before'라는 변수에 저장합니다.
	    //                     // CLOCK_MONOTONIC은 시스템 시작 이후의 절대 시간을 나타냅니다.
	    //
	    //                         // 그 다음, 프로그램은 버퍼 전체에 쓰기 작업을 수행합니다. 이 때, 쓰기 작업은 캐시 라인의 크기만큼 건너뛰며 수행됩니다.
	    //                             // 이는 CPU가 메모리를 캐시 라인 단위로 캐시하기 때문에, 각 쓰기 작업이 가능한 한 많은 캐시 라인을 건드리도록 하기 위함입니다.
	    //
	    //                                 // 모든 쓰기 작업이 완료되면, 프로그램은 다시 CLOCK_MONOTONIC을 사용하여 현재 시간을 얻고, 이를 'after'라는 변수에 저장합니다.
	    //
	    //                                     // 프로그램은 이제 'before'와 'after' 사이의 차이를 계산하여, 각 쓰기 작업에 대한 평균 시간을 나타내는 값을 계산하고 출력합니다.
	    //
	    //                                         // 마지막으로, 프로그램은 mmap()으로 할당한 메모리를 해제하고 종료합니다.

	char *progname;
	progname = argv[0];

	if (argc != 2) {
		fprintf(stderr, "usage: %s <size[KB]>\n", progname);
		exit(EXIT_FAILURE);
	}

	register int size;
	size = atoi(argv[1]) * 1024;

	if (!size){
		fprintf(stderr, "size should be >= 1 %d\n", size);
		exit(EXIT_FAILURE);
	}

	char *buffer;
	buffer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (buffer == (void*) -1)
		err(EXIT_FAILURE, "mmap() failed");

	struct timespec before, after;

	clock_gettime(CLOCK_MONOTONIC, &before);

	int i;
	for (i=0; i<NLOOP / (size / CACHE_LINE_SIZE); i++){
		long j;
		for (j=0; j<size; j+= CACHE_LINE_SIZE){
			buffer[j] = 0;
		}
	}

	clock_gettime(CLOCK_MONOTONIC, &after);
	printf("%f\n", (double)diff_nsec(before, after)/ NLOOP);

	if (munmap(buffer, size) == -1)
		err(EXIT_FAILURE, "munmap() failed");

	exit(EXIT_SUCCESS);
	
}
