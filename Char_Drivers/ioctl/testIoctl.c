#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include <sys/ioctl.h>

#define MYCDEV_MAGIC        'M'
#define MYCDEV_CLEAR        _IO(MYCDEV_MAGIC, 1)
#define MYCDEV_SAY_HELLO    _IO(MYCDEV_MAGIC, 2)
#define MYCDEV_USER_READ    _IOR(MYCDEV_MAGIC, 3, int) // User read , Kernel Write
#define MYCDEV_USER_WRITE   _IOW(MYCDEV_MAGIC, 4, int) // User write, Kernel Read

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

    if(ioctl(fd, MYCDEV_SAY_HELLO) < 0)
    {
        perror("Failed ioctl hello cmd\n");
        goto close_fd;
    }
    printf("Kernel said hello\n");


    int userVal;
    if(ioctl(fd, MYCDEV_USER_READ, &userVal) < 0)
    {
        perror("Couldn't read from kernel the value\n");
        goto close_fd;
    }
    printf("Read the value 0x%x\n",userVal);


    userVal = 0xb00b00;
    if(ioctl(fd, MYCDEV_USER_WRITE, &userVal) < 0)
    {
        perror("Couldn't write to kernel space\n");
        goto close_fd;
    }
    printf("Written the value 0x%x\n",userVal);

    
    char buffer[64];
    len = read(fd, buffer, 64);
    if(len<0)
    {
        printf("Failed read\n");
        goto close_fd;
    }
    printf("Read %d bytes, and data is %s : \n",len,buffer);
    
    if(ioctl(fd, MYCDEV_CLEAR) < 0)
    {
        perror("Failed ioctl clear the memory\n");
        goto close_fd;
    }
    memset(buffer,0,64);
    printf("Local buffer cleared too\n");

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