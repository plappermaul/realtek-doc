#ifndef __LOG_H__
#define __LOG_H__
#include "ca_module.h"
#include "ca_types.h"
#ifdef __KERNEL__
#include <linux/time.h>
#else
#include <time.h>
#endif

#define LOG_FILE_NAME_LEN       32
#define LOG_MSG_NUM                 1024
#define LOG_USER_NUM                512
#define LOG_FMT_MSG_LEN          512
#define LOG_SYSFILE_BIT             1
#define LOG_USRFILE_BIT             2

#define CAP_LOG_ENABLE       1
#define CAP_LOG_DISABLE      0
#define CAP_LOG_IOCTL_NAME  "calog-ioctl-dev"
#define CAP_LOG_IOCTL_MAGIC 'C'
#define CAP_LOG_MAX_CFG_LEN (4096)

typedef void (*ca_loglevel_cb)(ca_uint32 module_id, ca_uint32 oper, ca_uint32 data);

#define REMOTE_SYSLOG_PORT      514
#define LOG_DEFAULT_RECORD_LEVEL    CAP_LOG_ERROR
#define LOG_DEFAULT_PRINT_LEVEL     CAP_LOG_ERROR
#define LOG_DEFAULT_MODE        CAP_LOG_RECORD_ONLY
#define LOG_DEFAULT_REMOTE_IP   0xC0A8016A

#define LOG_PRINT(arg,...)  ca_printf(arg,##__VA_ARGS__)

#define LOG_ERROR(arg,...)   LOG_PRINT(arg,##__VA_ARGS__)

#define LOG_ASSERT(x)  \
    do{ \
        if (!(x))  { \
            LOG_ERROR("%s() _%d_: assert fail\n", __FUNCTION__, __LINE__); \
            return ; \
        } \
    }while(0)

#define LOG_ASSERT_RET(x,y) \
    do{ \
        if (!(x))  { \
            LOG_ERROR("%s() _%d_: assert fail\n", __FUNCTION__, __LINE__); \
            return (y); \
        } \
    }while(0)


#define LOG_MALLOC(size)        ca_malloc((size))
#define LOG_FREE(ptr)           if((ptr) != NULL){ ca_free((ptr)); (ptr) = NULL; }


#define LOG_MSG_LEN         256
/*Don't print it for kernel*/
#define CAP_LOG_NOPRINT_ERROR_CODE (0x4E4F5054)
#define LOG_MOD_NAME_LEN        16

#define CAP_LOG_ENABLE       1
#define CAP_LOG_DISABLE      0

#define LOG_FILE_TMP_NAME       "/tmp/user_log.txt"
#define LOG_FILE_DEFAULT_NAME   "/overlay/ca_log.txt"
#define LOG_FILE_USERFILE_NAME  "/overlay/user_log.txt"

#define   CA_CUST_MOD_DEFINES \
   xx(CA_MOD_CUST_MIN=CA_MOD_ID_MAX,        "Cmin"  )   \
   xx(CA_MOD_CUST_LOGIN,                                  "login"     )   \
   xx(CA_MOD_CUST_MGMT,                                  "mgmt"      )   \
   xx(CA_MOD_CUST_ATTACK,                               "attack"    )   \
   xx(CA_MOD_CUST_ALARM,                                "Calarm"   )   \
   xx(CA_MOD_CUST_MAX,                                    "Cmax"  )

typedef enum {
#undef xx
#define xx(mod_id,mod_desc)  mod_id,
   CA_CUST_MOD_DEFINES
   CAP_LOG_ID_MAX
} cap_log_id_e;

#define CA_MOD_ID_MIN  CA_MOD_DRVR_GEN

#define CAP_LOG_ERROR_CODE(code) (CA_MOD_LOG_KERN<<16|(code))

typedef enum
{
    LOG_LEVEL_SET_CMD = 0,
    LOG_MAX_CMD,
}cap_log_cmd_type_t;

typedef struct{
    ca_uint8    mod;
    ca_uint8    level;
    ca_uint32   err_code;
    time_t          time;
    ca_int8     log[LOG_MSG_LEN];
}cap_log_msg_t;

typedef struct{
    ca_uint32   from_pid;
    ca_uint32   to_pid;
    ca_uint8    msg_type;
    ca_uint16   length;
    ca_uint8    data[1];
}cap_log_ioctl_msg_t;

#define CAP_LOG_MSG_HDR_LEN          ((ca_uint)(((cap_log_ioctl_msg_t *)0)->data))

typedef struct
{
    ca_uint32 (*ioctl_cb)(cap_log_ioctl_msg_t *pMsg);
} cap_log_ioctl_cmd_t;


typedef struct{
    ca_int8 mod_name[LOG_MOD_NAME_LEN];
    ca_int8 record_level;
    ca_int8 print_level;
    ca_int8 enable;
}cap_log_mod_ctrl_t;

typedef struct{
    ca_uint8        init;
    ca_uint8        mod_num;
    ca_uint8        need_save;
    ca_uint8        mode;
    ca_uint32       record_index;
    ca_uint32       user_index;
    ca_uint          mutex;
    ca_uint          socket;
    cap_log_mod_ctrl_t   *mod_ctrl;
    ca_uint32       remote_ip;
    ca_loglevel_cb      log_level_cb;
    cap_log_msg_t        log_pool[LOG_MSG_NUM];
    cap_log_msg_t        usr_pool[LOG_USER_NUM];
    ca_int8         log_file[LOG_FILE_NAME_LEN];
    ca_int8         usr_file[LOG_FILE_NAME_LEN];
}cap_log_ctrl_t;


#define LOG_INFO_LEN    32
typedef struct{
    ca_int8        manu[LOG_INFO_LEN];
    ca_int8        product[LOG_INFO_LEN];
    ca_int8        sn[LOG_INFO_LEN];
    ca_int8        ip[LOG_INFO_LEN];
    ca_int8        hwver[LOG_INFO_LEN];
    ca_int8        swver[LOG_INFO_LEN];
}cap_log_file_header_t;

typedef struct {
    ca_uint8 module_id;
    ca_uint8 log_oper;
    ca_uint8 data;
}cap_log_msg_level_t;

typedef enum {
   CAP_LOG_OPER_PRINT = 0,
   CAP_LOG_OPER_RECORD,
   CAP_LOG_OPER_ENABLE,
   CAP_LOG_OPER_MAX
}cap_log_oper_t;

typedef enum {
   CAP_LOG_OFF = 0,
   CAP_LOG_EMERGENCY,
   CAP_LOG_ALERT,
   CAP_LOG_CRITICAL,
   CAP_LOG_ERROR,
   CAP_LOG_WARNING,
   CAP_LOG_NOTICE,
   CAP_LOG_INFO,
   CAP_LOG_DEBUG,
   CAP_LOG_LEVEL_MAX
}cap_log_level_t;

#define CAP_LOG_MAJOR CAP_LOG_ERROR
#define CAP_LOG_MINOR CAP_LOG_WARNING

typedef enum {
   CAP_LOG_RECORD_ONLY = 0,
   CAP_LOG_REMOTE_ONLY,
   CAP_LOG_RECORD_AND_REMOTE,
}cap_log_mode_t;

ca_status_t cap_log_init(ca_device_id_t device_id, ca_uint16 mod_num, ca_int8 mod_names[][LOG_MOD_NAME_LEN]);
#ifdef __KERNEL__
void cap_log_msg(ca_device_id_t device_id, ca_uint8 mod_id,ca_uint8 level,const ca_int8 * fmt, ...);
#else
void cap_log_msg(ca_device_id_t device_id, ca_uint8 mod_id,ca_uint8 level, ca_uint32 err_code,const ca_int8 * fmt,...);
#endif
void cap_log_show(ca_device_id_t device_id, ca_boolean_t clear);
void* log_msg_process(void *data);
cap_log_ctrl_t * cap_log_ctrl_get(void);
ca_int8 * cap_log_level_str_get(ca_uint8 level);

ca_status_t cap_log_level_set(ca_device_id_t device_id, ca_int8 * mod_name, ca_uint8  oper,ca_uint8  data);
ca_status_t cap_log_level_get(ca_device_id_t device_id, ca_int8 * mod_name, ca_uint8  oper, ca_uint8* data);

void ca_log2str(cap_log_msg_t * log, ca_int8* str, ca_uint32 len);
void cap_log_set(ca_boolean enable);

void cap_log_usr_set(ca_boolean enable);
void cap_log_usr_init(ca_uint16 mod_num, ca_int8 (*mod_names)[LOG_MOD_NAME_LEN]);
void cap_log_usr_savefile(void *data);
void cap_log_usr_header_get(cap_log_file_header_t * header);
void cap_log_usr_header_set(cap_log_file_header_t * header);
int cap_log_usr_clean(void);

int cap_log_file_get(ca_int8 * file_name, ca_int8 is_usr, ca_int8 * mod_name, ca_uint8 level);
void cap_log_file_remove(void);


#endif
