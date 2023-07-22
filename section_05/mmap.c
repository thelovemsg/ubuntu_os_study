#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#define BUFFER_SIZE 1000
#define ALLOC_SIZE (100*1024*1024)

static char command[BUFFER_SIZE];

int main(void) {
	pid_t pid;

	pid = getpid();
	snprintf(command, BUFFER_SIZE, "cat /proc/%d/maps", pid);
	
	puts("*** memory map before money allocation ***");
	fflush(stdout);
	system(command);

	void *new_memory;
	//mmap함수 : 메모리를 직접 매핑하는 POSIXS시스템 호출. 즉, 파일안 장치를 메모리에 매핑하거나, 메모리를 다른 메모리에 매핑할 때 사용된다. 
	new_memory = mmap(NULL, ALLOC_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (new_memory == (void*) -1)
		err(EXIT_FAILURE, "mmap() failed");
	puts(""); //문자열을 출력하고 줄바꿈을 추가한다.
	printf("*** succeeded to allocate memory: address = %p; size = 0x%x ***\n", new_memory, ALLOC_SIZE);
	puts("");
	puts("*** memory map after memory allocation ***");
	fflush(stdout);
	system(command);

	if (munmap(new_memory, ALLOC_SIZE) == -1)
		err(EXIT_FAILURE, "munmap() failed");
	exit(EXIT_SUCCESS);
}
