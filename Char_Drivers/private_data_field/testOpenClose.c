#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

int main(int argc, char *argv[])
{
    if(argc == 0)
    {
        printf("Give some arguements to open the file\n");
        return 1;
    }
    /*---------------------------------------------------*/

    int fd, ret;
    fd = open(argv[1],O_RDWR);
    if(fd<0)
    {
        printf("Failed to open the file, status fd : %d\n",fd);
        return fd;
    }
    printf("Opened the file : %s\n",argv[1]);

    int len = write(fd, argv[2], strlen(argv[2]));
    if(len<0)
    {
        printf("Failed write\n");
        goto close_fd;
    }
    printf("Written %d bytes\n",len);

    getchar();

    char buffer[64];
    len = read(fd, buffer, 64);
    if(len<0)
    {
        printf("Failed read\n");
        goto close_fd;
    }
    printf("Read %d bytes, and data is %s : \n",len,buffer);


close_fd:
    ret = close(fd);
    if(ret<0)
    {
        printf("Failed to close the file, status fd : %d\n",fd);
        return ret;
    }
    printf("Closed the file : %s\n",argv[1]);
    

    return 0;
}