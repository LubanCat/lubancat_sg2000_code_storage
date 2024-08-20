#include <unistd.h>  
#include <stdio.h>  
#include <fcntl.h>  
#include <linux/fb.h>  
#include <sys/mman.h>  
#include <stdlib.h>  
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <errno.h>

#define ARGB8888

#define PAUSE()                                                                                                        \
    do {                                                                                                           \
        printf("---------------press Enter key to continue!---------------\n");                                    \
        getchar();                                                                                             \
    } while (0)


#if defined(ARGB8888)	// 32bits
#define RED      0xFFFF0000
#define GREEN    0xFF00FF00
#define BLUE     0xFF0000FF
#define YELLOW   0xFFFFFF00
#define WHITE    0xFFFFFFFF 
#define BLACK    0xFF000000
void fill_color(uint32_t *fb_addr, uint32_t bit_map, int psize)
{
    int i;
    int pix_count = psize / 4;
    for(i=0; i<pix_count; i++) {
        *fb_addr = bit_map;
        fb_addr++;
    }
}
#else	// 16bits
#define RED      0xFC00
#define GREEN    0x83E0
#define BLUE     0x801F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF 
#define BLACK    0x8000
void fill_color(short *fb_addr, short bit_map, int psize)
{
    int i;
    for(i=0; i<psize; i++) {
        *fb_addr = bit_map;
        fb_addr++;
    }
}
#endif

void _fb_get_info(int fp, struct fb_fix_screeninfo *finfo, struct fb_var_screeninfo *vinfo)
{
    long screensize=0;  

    if(ioctl(fp, FBIOGET_FSCREENINFO, finfo)){  
        printf("Error reading fixed information/n");  
        exit(2);  
    }  

    if(ioctl(fp, FBIOGET_VSCREENINFO, vinfo)){  
        printf("Error reading variable information/n");  
        exit(3);  
    }  

    screensize = finfo->line_length * vinfo->yres;

    printf("The ID=%s\n", finfo->id);
    printf("The phy mem = 0x%x, total size = %d(byte)\n", finfo->smem_start, finfo->smem_len);  
    printf("line length = %d(byte)\n", finfo->line_length);  
    printf("xres = %d, yres = %d, bits_per_pixel = %d\n", vinfo->xres, vinfo->yres, vinfo->bits_per_pixel);  
    printf("xresv = %d, yresv = %d\n", vinfo->xres_virtual, vinfo->yres_virtual);  
    printf("vinfo.xoffset = %d, vinfo.yoffset = %d\n", vinfo->xoffset, vinfo->yoffset);  
    printf("vinfo.vmode is :%d\n", vinfo->vmode);  
    printf("finfo.ypanstep is :%d\n", finfo->ypanstep);  
    printf("vinfo.red.offset=0x%x\n", vinfo->red.offset);
    printf("vinfo.red.length=0x%x\n", vinfo->red.length);
    printf("vinfo.green.offset=0x%x\n", vinfo->green.offset);
    printf("vinfo.green.length=0x%x\n", vinfo->green.length);
    printf("vinfo.blue.offset=0x%x\n", vinfo->blue.offset);
    printf("vinfo.blue.length=0x%x\n", vinfo->blue.length);
    printf("vinfo.transp.offset=0x%x\n", vinfo->transp.offset);
    printf("vinfo.transp.length=0x%x\n", vinfo->transp.length);
    printf("Expected screensize = %d(byte), using %d frame\n", screensize, finfo->smem_len/screensize);
}

int main ()   
{  
    int fp=0;  
    struct fb_var_screeninfo vinfo;  
    struct fb_fix_screeninfo finfo;  
    void *fbp = NULL;    
    uint32_t *test_fbp = NULL;
    int x = 0, y = 0;  
    long location = 0;
    int i;
    int num = 1;
    int pix_size=0;

    fp = open("/dev/fb0", O_RDWR);  

    if(fp < 0) {  
        printf("Error : Can not open framebuffer device/n");  
        exit(1);  
    }  

    printf("-- Default fb info --\n");
    _fb_get_info(fp, &finfo, &vinfo);

    fbp = mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0);
    if (fbp == MAP_FAILED)
    {
        printf ("Error: failed to map framebuffer device to memory.\n");
        exit (4);  
    }
    printf("Get virt mem = %p\n", fbp);  

    pix_size = finfo.line_length * vinfo.yres;
    /* using first frame, for FBIOPAN_DISPLAY
     * 当刷新需要调用FBIOPAN_DISPLAY， 要告知驱动刷哪块帧， 用到下面两个参数
     * 如果使用第二帧buffer -> vinfo.xoffset = 0; vinfo.yoffset = vinfo.yres;
     */
    vinfo.xoffset = 0;
    vinfo.yoffset = 0;

    /* show color loop */
    while(num--) {
        printf("\ndrawing YELLOW......\n");
        vinfo.xoffset = 0;
        vinfo.yoffset = 0;
        fill_color(fbp, YELLOW, pix_size);
        if (ioctl(fp, FBIOPAN_DISPLAY, &vinfo) != 0) {
        	printf("\nPAN_DISPLAY err(%d)\n", errno);
	}
        sleep(1);

        printf("\ndrawing BLUE......\n");
        vinfo.xoffset = 0;
        vinfo.yoffset = 0;
        fill_color(fbp, BLUE, pix_size);
        if (ioctl(fp, FBIOPAN_DISPLAY, &vinfo) != 0) {
        	printf("\nPAN_DISPLAY err(%d)\n", errno);
	}
        sleep(1);
        
        printf("\ndrawing RED......\n");
        vinfo.xoffset = 0;
        vinfo.yoffset = 0;
        fill_color(fbp, RED, pix_size);
        if (ioctl(fp, FBIOPAN_DISPLAY, &vinfo) != 0) {
        	printf("\nPAN_DISPLAY err(%d)\n", errno);
	}
        sleep(1);
	PAUSE();
    }

    munmap(fbp, finfo.smem_len); /*解除映射*/  

    close (fp);
    return 0;
}  
