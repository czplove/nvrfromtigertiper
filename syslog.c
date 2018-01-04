#ifdef _LINUX_
#include <stdio_ext.h>
#endif

#include "syslog.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
 
#define FINC_SLASH	'/'
#define PATH_MAX	512

static FILE *syslog_fp = NULL;	//-记录系统日志文件句柄
static char syslog_tag[PATH_MAX] = "";
static int  syslog_pri = LOG_DEBUG;		/* default level */

int getrname(char *buff, int buff_size)
{
	char path[PATH_MAX] = "", *p;
	//-获取当前程序的绝对路径
	if (readlink("/proc/self/exe", path, sizeof(path)) <= 0)	//-将参数path的符号链接内容存储到参数buf所指的内存空间
		return -1;

	if ((p = strrchr(path, FINC_SLASH)) == NULL)	//-查找字符在指定字符串中从左面开始的最后一次出现的位置
		return -1;

	snprintf(buff, buff_size, p + 1);	//-获取到名字
	return 0;
}


void openlog(const char *ident, int opt, int fac)	//-打开log会根据实际情况进行选择输出地点
{
	char fname[PATH_MAX];
	char prog[512];
	int  ret;

	if (strlen(syslog_tag) > 0)		/* already open */
		return;

	ret = getrname(prog, sizeof(prog));	//-得到运行程序的名字
	if (fac == LOG_LOCAL0 || ret == -1)
		syslog_fp = stdout;
	else { 
		snprintf(fname, sizeof(fname), "/var/log/%s.log", prog); //-形成系统日志名字

		if ((syslog_fp = fopen(fname, "a")) == NULL)
			syslog_fp = stdout;
	}
 
	setlinebuf(syslog_fp);	/* set stream to line buffered */ //-设置文件流为线性缓冲区

#define TAG_LEN	 strlen(syslog_tag)
#define TAG_LEFT (PATH_MAX - TAG_LEN)
	if (strlen(prog) > 0)
		snprintf(&syslog_tag[TAG_LEN], TAG_LEFT, "%s", prog);
	if (opt & LOG_PID)
		snprintf(&syslog_tag[TAG_LEN], TAG_LEFT, "[%d]", (int)getpid());
	if (opt & LOG_CONS)
		/* TODO */;
	if (opt & LOG_PERROR)
		/* TODO */;

	snprintf(&syslog_tag[TAG_LEN], TAG_LEFT, ":");
}

void syslog(int pri, const char *fmt, ...)	//-打印信息,调用一次输出一条信息
{
	va_list ap;
	char buf[LOG_MAX_LEN] = "";
	char st[32] = "";
	struct tm tm;
	time_t tt;

	if (pri > syslog_pri)	//-决定打印的水平
		return;

	if (syslog_fp == NULL)	/* not open syslog yet */
		openlog(NULL, LOG_CONS | LOG_PID, LOG_LOCAL0);

	tt = time(NULL); 
	if (localtime_r(&tt, &tm) != NULL)
		strftime(st, sizeof(st), "%F %T", &tm); 

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	fprintf(syslog_fp, "%s %s %s\n", st, syslog_tag, buf);
}

void closelog()
{
	if (syslog_fp != NULL && syslog_fp != stdout)
		fclose(syslog_fp);
}

int setlogmask(int mask)	//-设置log等级
{
	int syslog_mask = 0xFF;
	syslog_pri = LOG_DEBUG;

	while (syslog_mask > mask) {
		syslog_mask = (syslog_mask >> 1) - 1;
		syslog_pri--;
	}

    return syslog_pri;
}
