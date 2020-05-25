#include "skl_public.h"
#include "skl_common_queue.h"

#include "skl_stor_common.h"
#include "skl_access_common.h"

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static void skl_vehicle_exception_handler(int signal, siginfo_t *info, void *ptr)
{
    ULONG ulRet = 0;

    switch (signal)
    {
        case SIGTERM:
        case SIGKILL:
        case SIGINT:
        //case SIGSEGV:
        {
            SklStor_destory();
            SklAccess_destory();

            ulRet = SklQueue_destory();
            printf("[exception_handler] Queue_destory Ret %lu\n", ulRet);
            
            exit(1);
        }
        break;
        default:
            printf("unknow signal\n");
        break;
    }
}

static void skl_vehicle_exception_init(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));

    sa.sa_sigaction = skl_vehicle_exception_handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction (SIGTERM, &sa, NULL);
    sigaction (SIGKILL, &sa, NULL);
    sigaction (SIGINT, &sa, NULL);
    //sigaction (SIGSEGV, &sa, NULL);

    return;
}

int main(int argc, char **args)
{
    ULONG ulRet = SKL_STAT_SUCCESS;

    skl_vehicle_exception_init();
    
    ulRet = SklQueue_init();
    if (ulRet != SKL_STAT_SUCCESS)
    {
        printf("[%s %d] Queue_init fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }

    ulRet = SklStor_init();
    if (ulRet != SKL_STAT_SUCCESS)
    {
        SklQueue_destory();
        printf("[%s %d] Stor_init fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }

    ulRet = SklAccess_init();
    if (ulRet != SKL_STAT_SUCCESS)
    {
        SklQueue_destory();
        SklStor_destory();
        printf("[%s %d] Access_init fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }
    
    while (1)
    {
        sleep(1);
    }
    
	return 0;
}

