#include "soft_public.h"
#include "soft_common_queue.h"

#include "soft_stor_common.h"
#include "soft_access_common.h"

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static void soft_vehicle_exception_handler(int signal, siginfo_t *info, void *ptr)
{
    ULONG ulRet = 0;

    switch (signal)
    {
        case SIGTERM:
        case SIGKILL:
        case SIGINT:
        //case SIGSEGV:
        {
            SoftStor_destory();
            SoftAccess_destory();

            ulRet = SoftQueue_destory();
            printf("[exception_handler] Queue_destory Ret %lu\n", ulRet);
            
            exit(1);
        }
        break;
        default:
            printf("unknow signal\n");
        break;
    }
}

static void soft_vehicle_exception_init(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));

    sa.sa_sigaction = soft_vehicle_exception_handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction (SIGTERM, &sa, NULL);
    sigaction (SIGKILL, &sa, NULL);
    sigaction (SIGINT, &sa, NULL);
    //sigaction (SIGSEGV, &sa, NULL);

    return;
}

int main(int argc, char **args)
{
    ULONG ulRet = SOFT_STAT_SUCCESS;

    soft_vehicle_exception_init();
    
    ulRet = SoftQueue_init();
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] Queue_init fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }

    ulRet = SoftStor_init();
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        SoftQueue_destory();
        printf("[%s %d] Stor_init fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }

    ulRet = SoftAccess_init();
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        SoftQueue_destory();
        SoftStor_destory();
        printf("[%s %d] Access_init fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }
    
    while (1)
    {
        sleep(1);
    }
    
	return 0;
}

