#ifndef _INFO_H
#define _INFO_H

#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#define _FILE_OFFSET_BITS 64

#include<pthread.h>
#include<string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

//#include"util.h"
#define MaxBlocks (1024*1024/8)
#define MaxUsers  1024
#define CNameLength       64	//����ͷ������󳤶�
#define VolNameLength     64	//¼������ֳ���
#define MaxchunkCount     32	//һ·����ͷ�������ɢchunk��Ŀ
#define  FNameLength      7	//�ļ�ϵͳ���ֵĳ���
#define MaxDesLen        1024	//������Ϣ����󳤶�
#define MaxBufBitmapLen  (1024/8)	//��������bitmap����
#define BuffNodeNum        192
#define H264 1
#define MJPEG 2
#define MPEG4 3
#define MAGIC 0x7061
#define SIZEOF(a)  ((64-sizeof(a)%64)+sizeof(a))
#define size_t			////////////////////////
#define ReadRECORD 0
#define WriteRECORD    1
#define EXTEND     1
#define NOTEXTEND  0
#define CHUNKNULL (MaxBlocks*8+1)
#define ISRecycled 1
#define NOTRecycled 0
#define TLEN      (1728*3)
#define FISTTIMESIZE (90*24*60*60/TLEN)
#define WriteLen  (FISTTIMESIZE/30)
#define Maxtime  (90*24*60*60)	//modify
#define NOTSET  0
#define SET     1
#define BufTime 60*2
#define TimeBuffSize (800)
#define BLOCKSIZE (128*1024*1024)	//////////////////////////??????
#define TimeNULL -1
#define FILE_SYS_NAME "VEDIO_\0"
#define STRING_NULL   "NULL_\0"


#define CONF_LEN      (VolNameLength+CNameLength+20)

#define SB_SIZE 128
#define Vnode_SIZE 512
#define DSI_SIZE   1048
#define SEG_SIZE  8
#define SEGINFO_SIZE (DSI_SIZE+10*1024)
#define Tnode_SIZE   16
#define LvmCount  256
#define BitmapAddr  (SB_SIZE)
#define VBitmapAddr (SB_SIZE+MaxBlocks)
#define VnodeAddr   (SB_SIZE+MaxBlocks+MaxUsers/8)
#define DataAddr    (SB_SIZE+MaxBlocks+MaxUsers/8+MaxUsers*Vnode_SIZE)
#define DataAddr1   (1024*SEG_SIZE+1024*SEGINFO_SIZE+FISTTIMESIZE*Tnode_SIZE+Maxtime*Tnode_SIZE)
#define BLKGETSIZE   _IO(0x12,96)

#define MALLOC_ERR 10010	//�����ڴ�ʧ��
#define OPEN_FILE_ERR 10011	//���߼���ʧ��
#define READ_LVM_ERR  10012	//���߼���ʧ��
#define NOT_MATCH_SIZE 10013	//�ٴ�����ʱ���ϴα�����߼����С�뵱ǰ�Ĵ�С����1M
#define LVM_SMALL     10014	//�߼��������̫С
#define CREATING_LVM  10015	//��ǰ���ڴ�����һ�߼������Ժ�����
#define NOT_EXIST_LVM 10015	//û���ҵ���Ӧ�߼���
#define NOT_EXIST_RECORD 10016	//¼���δ����
#define CREATING_RECORD 10017	//��ǰ���ڴ���¼������Ժ�����
#define MAX_LVM_COUNT  10018	//�������256����Ƶ�߼���
#define BUILD_MEM_ERR  10019	//Ϊ��Ƶ�����ڴ�ṹʧ��
#define WRITE_LVM_ERR  10020	//д�߼���ʧ��
#define MAX_RDWR_COUNT 10021	//��һ�߼���ﵽ�����Ķ�д·����128·
#define ALLOC_FD_ERR   10022	//����fdʧ��
#define TOO_FLAGS       10023	//Ϊ¼������Ŀ�̫��ɢ
#define EXIST_SAME_NAME  10024	//�Ѿ�������ͬ���֡�
#define ERR_HANDLE       10025	//����ľ��
#define TIME_ERR      10026	//����Ŀ�ʼʱ������ʱ�������������
#define COLSE_ERR     10027	//���ж�����дδ�رգ����ܹر�¼���
#define LOCK_ERR      10028	//
#define NOT_EXIST_CAMERA   10029	//�������ļ���δ�ҵ�����ͷ�ļ�¼��Ϣ
#define ERR_TIME_INTERL 10031
#define NVR_CREATWRITEDATATHREADFAIL 10030
#define SPACE_NOT_ENOUGH 10032
#define ZERO_TNODE 10033
#define NOT_EXIST_CAMERALIST 10034
#define CAMERA_IS_RECORDING 10035




#define spin_init(spinlock) pthread_mutex_init(&(spinlock), NULL)
#define  spin_lock(spinlock)  pthread_mutex_lock(&(spinlock))
#define spin_trylock(spinlock) pthread_mutex_trylock(&(spinlock))
#define spin_unlock(spinlock)   pthread_mutex_unlock(&(spinlock))
#define spin_destroy(spinlock)  pthread_mutex_destroy(&(spinlock))
#define spin_rwinit(spinlock)  pthread_rwlock_init(&(spinlock),NULL)
#define spin_rdlock(spinlock)   pthread_rwlock_rdlock(&(spinlock))
#define spin_wrlock(spinlock) pthread_rwlock_wrlock(&(spinlock))
#define spin_rwunlock(spinlock) pthread_rwlock_unlock(&(spinlock))
#define spin_trywrlock(spinlock) pthread_rwlock_trywrlock(&(spinlock))
#define spin_tryrdlock(spinlock) pthread_rwlock_trywrlock(&(spinlock))
#define spin_rwdestroy(spinlock) pthread_rwlock_destroy(&(spinlock))

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef int int32_t;
typedef long long int64_t;
typedef pthread_mutex_t spinlock_t;
typedef pthread_rwlock_t spinrwlock_t;

typedef struct superBlcok {
    char fileSystemName[FNameLength];	//�ļ�ϵͳ����
    unsigned long long totalSize;	//������
    short vnodeCount;		//�ѷ����vnode
    long long blockSize;	//���ݿ��С
    int mTime;			//���һ��װ��fs��ʱ��
    int wTime;			//����������һ��д��ʱ��
    short magic;		//����ʶ���ļ�ϵͳ�Ļ���
// long long  dataAddr;//���ݿ����ʼ��ַ
    long long dataBlocksCount;	//�ܵĴ洢���ݿ����Ŀ
    long long freeBlocksCount;	//�������ݿ����Ŀ
    int interval;		//д�ش��̵ļ��ʱ��
} SBlock;

typedef struct initInfo {	//�����ļ��ļ�¼�ṹ
    int mode;
    char volName[VolNameLength];	//���ھ�����
    char fileSystemName[FNameLength];	//�ļ�ϵͳ����
    unsigned long long size;	//������
} _initInfo;

typedef struct timeNode {
    int time;
    int len;
    long long addr;
} tnode;

typedef struct vnodeInfo {
    char key;
    short ID;
    int fd;			//�򿪵��ļ�������
    char status;		//1��ʾ����д¼��0��ʾ��¼��
    int count;			//��ʱ�������ڼ�¼tnode������Ŀ
    tnode t[TimeBuffSize];	//ʱ�仺��ռ�
    int beginTime;
    int endTime;
    union {
	unsigned long long nextTimeAddr;
	char is_starttime;
    };
    struct vnodeInfo *next;	//ʹ������ṹ���� ��������
    pthread_mutex_t mutex;
} *_vnodeInfo;

typedef struct _vnode {
    char cameraid[CNameLength];	//����ͷ����
    char alias[CNameLength];	//����ͷ����
    char volName[VolNameLength];	//���ھ�����
    int block[MaxchunkCount][2];	//�������vnodeָʾ¼���Ŀ飬�ÿ�ż�¼
    long long storeAddr;	//��һ��д���ַ
    long long curSnode;		//��ǰsnode��
    long long firstIndex;	//ʱ���������׵�ַ
    long long queryAdd;		//��һ��дʱ��������ַ
    char isRecycle;		//��ת��־λ
    short savedDays;		//¼�񱣴�����
    char delPolicy;		//¼�񸲸ǲ���
    char encodeType;		//�����ʽ #define H264 1��
//#define MJPEG 2��#define MPEG4 3��
    short count;		//һ�������ļ�����
    short wr_count;		//һ��������дλ��
    char SnodeRecycle;		////1 Recycled; 0 notRecycled 
    _vnodeInfo _bf;		//ָ���Ӧ�Ļ�����
    spinrwlock_t spin;
    char status;
#ifdef SPACE_TIME_SYNCHRONIZATION
    char SpaceState;
    short origin_count;
    uint32_t origin_time;
#endif    
} vnode;

/*typedef struct _Voluminfo {
	char cameraid[CNameLength];	//����ͷ����
	char alias[CNameLength];	//����ͷ����
	char volName[VolNameLength];	//���ھ�����
	short savedDays;	//¼�񱣴�����
	char delPolicy;		//¼�񸲸ǲ���
	char encodeType;	//�����ʽ #define H264 1��
} Voluminfo;*/

typedef struct bufferHead {
    char map[MaxBufBitmapLen];
    pthread_rwlock_t spin;
} *bfHead;

typedef struct sbInfo {
    SBlock *_es;		//�������ָ��
    char volName[VolNameLength];	//���ھ�����
    char *bitmap;		//λͼ�Ļ�����
    char *vnodemapping;		//vnodeӳ��Ļ�����
    char *vnodeTable;		//vnode�Ļ�����
    //pthread_mutex_t spinSuperblock;       //�������λͼ����
    //pthread_rwlock_t spinvnode;   //vbitmap��vnode��λͼ����
    bfHead _bh;			//���ڹ��������Ľṹ��
    char *_bf;			//����Ļ�����
    struct sbInfo *next;
    pthread_mutex_t mutex;	//for open volName mutex,
} *_sbinfo;

typedef struct {
    _sbinfo table[LvmCount];
    pthread_rwlock_t spin;
} SBTable;

typedef struct DateSegIndex {
    int start_time;
    int end_time;
} segindex;

typedef struct DataSegInfo {
    int size;			//��Ӧ�����ݶ�ͷ��С
    int frame;			//֡��,��λΪ��֡/��
    short width;		//�ֱ���
    short height;		//�ֱ���
    char des[MaxDesLen];	//������Ϣ
} seginfo;
//ȫ�ֱ���


extern SBTable sbTable;

void sb_to_buf(char *buf, SBlock * sb);
void buf_to_sb(char *buf, SBlock * sb);
void vnode_to_buf(char *buf, vnode * v);
void buf_to_vnode(char *buf, vnode * v);
void DSI_to_buf(char *buf, seginfo * seg);
void buf_to_DSI(char *buf, seginfo * seg);
int set_clr_bitmap(_sbinfo sbinfo, vnode * v, int flag);
int free_blocks(_sbinfo sbinfo, vnode * v, int time);
int put_sb(_sbinfo sbinfo, vnode * v);
char *extend_buf(vnode * v, _sbinfo sbinfo, char mode);
int test_or_alloc_ID(_sbinfo sbinfo, char *name, int len, int *n,
		     int flag);
int cal_alloc_chunk(_sbinfo sbinfo, vnode * v, unsigned long long  blocks);
_sbinfo get_sbinfo(const char *volumeid);
_vnodeInfo get_Vi(vnode * v, int key);
int delete_vi(_sbinfo sbinfo, vnode * v, char mode, int key);
int free_vi(_sbinfo sbinfo, int nr);
//int read_vol_by_camera(char *vol_path, const char *cameraid);
//int write_parm_file(const char *vol_path, const char *cameraid);

_vnodeInfo alloc_vi(_sbinfo sbinfo);

extern int put_vnode(_sbinfo sbinfo, vnode * v, char *_vbitmap, int ID);

/*Bitops_add.c*/
extern /*inline */ int bit(char *addr, int nr);
extern /*inline */ int setbit_(char *addr, int nr);
extern /*inline */ int clrbit_(char *addr, int nr);
extern /*inline */ int find_first_zero(char *addr, int nr);

/*util.c*/
/*
extern  int spin_init(pthread_mutex_t spinlock);
extern  int spin_lock(pthread_mutex_t spinlock);
extern  int spin_trylock(pthread_mutex_t spinlock);
extern  int spin_unlock(pthread_mutex_t spinlock);
extern  int spin_destroy(pthread_mutex_t spinlock);
extern  int spin_rwinit(pthread_rwlock_t spinlock);
extern int spin_rdlock(pthread_rwlock_t spinlock);
extern  int spin_wrlock(pthread_rwlock_t spinlock);
extern  int spin_rwunlock(pthread_rwlock_t spinlock);
extern / int spin_trywrlock(pthread_rwlock_t spinlock);
extern  int spin_rwdestroy(pthread_rwlock_t spinlock);
extern int spin_tryrdlock(pthread_rwlock_t spinlock);
*/


extern unsigned int ErrorFlag;
extern int buf_hash(char *name, int len);
extern void _Debug(char *s, int line, char *file);
extern int _read(int fd, char *buf, int size, long long offset);
extern int _write(int fd, char *buf, int size, long long offset);

/**init.c*/

extern unsigned long long InitFalg;
extern int free_vnode(_sbinfo sbinfo, vnode * v);


/*getsize.c*/

#endif
