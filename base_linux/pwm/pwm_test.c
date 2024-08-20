#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

static char pwm_path[75];
static int pwm_config(const char *attr, const char *val)    // 配置 PWM
{
    char file_path[100];
    int len;
    int fd;
    sprintf(file_path, "%s/%s", pwm_path, attr);
    if ((fd = open(file_path, O_WRONLY)) < 0) {
        perror("open error");
        return fd;
    }
    len = strlen(val);
    if (len != write(fd, val, len)) {
        perror("write error");
        close(fd);
        return -1;
    }
    close(fd); // 关闭文件
    return 0;
}

int main(int argc, char *argv[])
{
    /* 校验传参 */
    if (4 != argc) {
        fprintf(stderr, "usage: %s <id> <period> <duty>\n", argv[0]);
        exit(-1);
    }
    /* 打印配置信息 */
    printf("PWM config: id<%s>, period<%s>, duty<%s>\n", argv[1], argv[2], argv[3]);

    /* 将 argv[1] 从字符串转换为整数 */
    int number = atoi(argv[1]);

    /* 确定 pwmchip 控制器编号 */
    int controller;
    if (number >= 0 && number <= 3) {
        controller = 0;
    } else if (number >= 4 && number <= 7) {
        controller = 4;
    } else if (number >= 8 && number <= 11) {
        controller = 8;
    } else if (number >= 12 && number <= 15) {
        controller = 12;
    } else {
        fprintf(stderr, "number is out of the expected range.\n");
        return 1;
    }

    /* 求余数，确定 pwm 编号 */
    int remainder = number % 4;
    char buf[10];
    sprintf(buf, "%d", remainder);

    /* 导出 pwm */
    sprintf(pwm_path, "/sys/class/pwm/pwmchip%d/pwm%d", controller, remainder);

    // 如果 pwm0 目录不存在，则导出
    if (access(pwm_path, F_OK)) {
        char temp[100];
        int fd;
        sprintf(temp, "/sys/class/pwm/pwmchip%d/export", controller);
        if (0 > (fd = open(temp, O_WRONLY))) {
            perror("open error");
            exit(-1);
        }
        // 导出 pwm
        if (1 != write(fd, buf, strlen(buf))) {
            perror("write error");
            close(fd);
            exit(-1);
        }
        close(fd); // 关闭文件
    }

    /* 配置 PWM 周期 */
    if (pwm_config("period", argv[2]))
        exit(-1);
    /* 配置占空比 */
    if (pwm_config("duty_cycle", argv[3]))
        exit(-1);
    /* 使能 pwm */
    pwm_config("enable", "1");
    getchar();
    /* 退出程序 */
    exit(0);
}
