
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
	int fd,err;
	fd = open("/dev/firstdrv", O_RDONLY);
	if (fd < 0)
	{
		printf("can't open!\n");
	}
	sleep(0x03);
	err=close(fd);
	if (err < 0)
	{
		printf("can't close!\n");
	}
	return 0;
}
//This is a git test.

