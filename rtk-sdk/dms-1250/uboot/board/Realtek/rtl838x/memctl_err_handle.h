#ifndef __MT_ERR_HANDLE__
#define __MT_ERR_HANDLE__

#define MT_SUCCESS    (0)
#define MT_FAIL       (-1)
#ifdef CONFIG_MT_ERR_HANDLE
#define HANDLE_FAIL \
({ \
    printf("%s (%d) test failed.\n", __FUNCTION__,__LINE__);\
    return MT_FAIL;\
})
#else
#define HANDLE_FAIL \
({ \
    printf("%s (%d) test failed.\n", __FUNCTION__,__LINE__);\
    while(1);\
})
#endif


#endif //end of __MT_ERR_HANDLE__


