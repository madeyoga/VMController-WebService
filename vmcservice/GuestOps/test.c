#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "vix.h"

#define GUEST_USERNAME "hnp"
#define GUEST_PASSWORD "hnPalit"
#ifdef USE_WORKSTATION

#define CONNTYPE VIX_SERVICEPROVIDER_VMWARE_WORKSTATION

#define HOSTNAME ""
#define HOSTPORT 0
#define USERNAME ""
#define PASSWORD ""

#define VMPOWEROPTIONS VIX_VMPOWEROP_LAUNCH_GUI

#define VMXPATH_INFO "where vmxpath is an absolute path to the .vmx file " \
                     "for the virtual machine."

#else // USE_WORKSTATION

/*
 * For VMware Server 2.0
 */

#define CONNTYPE VIX_SERVICEPROVIDER_VMWARE_VI_SERVER

#define HOSTNAME "https://192.2.3.4:8333/sdk"
/*
 * NOTE: HOSTPORT is ignored, so the port should be specified as part
 * of the URL.
 */
#define HOSTPORT 0
#define USERNAME "root"
#define PASSWORD "hideme"

#define VMPOWEROPTIONS VIX_VMPOWEROP_NORMAL

#define VMXPATH_INFO "where vmxpath is a datastore-relative path to the " \
                     ".vmx file for the virtual machine, such as "        \
                     "\"[standard] ubuntu/ubuntu.vmx\"."

#endif // USE_WORKSTATION

#define TOOLS_TIMEOUT 300

#ifdef _WIN32
#define DEST_FILE ".\\outfile"
#else
#define DEST_FILE "./outfile.txt"
#endif

static char *progname;

static void usage()
{
    fprintf(stderr, "Usage: %s <vmxpath> \n");
    fprintf(stderr, "%s\n", VMXPATH_INFO);
}

int main(int argc, char **argv)
{
    char *vmpath;
    VixError err;
    VixHandle hostHandle = VIX_INVALID_HANDLE;
    VixHandle jobHandle = VIX_INVALID_HANDLE;
    VixHandle vmHandle = VIX_INVALID_HANDLE;

    progname = argv[0];
    if (argc > 1)
    {
        vmpath = argv[1];
    }
    else
    {
        usage();
        exit(EXIT_FAILURE);
    }

    jobHandle = VixHost_Connect(VIX_API_VERSION,    // api version
                                CONNTYPE,           // connection type
                                HOSTNAME,           // host name
                                HOSTPORT,           // host port
                                USERNAME,           // username
                                PASSWORD,           // passwd
                                0,                  // options
                                VIX_INVALID_HANDLE, // property list handle
                                NULL,               // callback
                                NULL);              // client data

    err = VixJob_Wait(jobHandle, VIX_PROPERTY_JOB_RESULT_HANDLE,
                      &hostHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to connect to host (%" FMT64 "d %s)\n", err,
                Vix_GetErrorText(err, NULL));
        goto abort;
    }
    printf("connected to host (%d)\n", hostHandle);

    printf("about to open %s\n", vmpath);
    jobHandle = VixVM_Open(hostHandle, // host connection
                           vmpath,     // path to vmx
                           NULL,       // callback
                           NULL);      // client data
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_JOB_RESULT_HANDLE,
                      &vmHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to open virtual machine '%s'(%" FMT64 "d %s)\n", vmpath, err,
                Vix_GetErrorText(err, NULL));
        goto abort;
    }
    printf("opened %s (%d)\n", vmpath, vmHandle);

    printf("powering on\n");
    jobHandle = VixVM_PowerOn(vmHandle,           // vm handle
                              VMPOWEROPTIONS,     // options
                              VIX_INVALID_HANDLE, // property list
                              NULL,               // callback
                              NULL);              // client data
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to power on virtual machine (%" FMT64 "d %s)\n", err,
                Vix_GetErrorText(err, NULL));
        goto abort;
    }
    printf("powered on\n");

    printf("waiting for tools\n");
    jobHandle = VixVM_WaitForToolsInGuest(vmHandle,
                                          TOOLS_TIMEOUT, // timeout in secs
                                          NULL,          // callback
                                          NULL);         // client data
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to wait for tools in virtual machine (%" FMT64 "d %s)\n", err,
                Vix_GetErrorText(err, NULL));
        goto abort;
    }
    printf("tools up\n");

    jobHandle = VixVM_LoginInGuest(vmHandle,
                                   GUEST_USERNAME, // guest OS user
                                   GUEST_PASSWORD, // guest OS passwd
                                   0,              // options
                                   NULL,           // callback
                                   NULL);          // client data
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to login to virtual machine '%s'(%" FMT64 "d %s)\n", vmpath, err,
                Vix_GetErrorText(err, NULL));
        goto abort;
    }
    printf("logged in to guest\n");

    printf("about to do work\n");
    jobHandle = VixVM_RunProgramInGuest(vmHandle,
                                        "/usr/bin/free",              // command
                                        "-m > /tmp/mem.txt", // cmd args
                                        0,                      // options
                                        VIX_INVALID_HANDLE,     // prop handle
                                        NULL,                   // callback
                                        NULL);                  // client data
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to run program in virtual machine '%s'(%" FMT64 "d %s)\n",
                vmpath, err, Vix_GetErrorText(err, NULL));
        goto abort;
    }
    jobHandle = VixVM_RunProgramInGuest(vmHandle,
                                        "/bin/df",              // command
                                        "-h --output=source,used,avail,pcent >/tmp/disk.txt; /usr/bin/head -1 /tmp/disk.txt > /tmp/disk1.txt; /usr/bin/tail -n +2 /tmp/disk.txt | sort >> /tmp/disk1.txt", // cmd args
                                        0,                      // options
                                        VIX_INVALID_HANDLE,     // prop handle
                                        NULL,                   // callback
                                        NULL);                  // client data
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to run program in virtual machine '%s'(%" FMT64 "d %s)\n",
                vmpath, err, Vix_GetErrorText(err, NULL));
        goto abort;
    }
    jobHandle = VixVM_RunProgramInGuest(vmHandle,
                                        "/bin/hostname",              // command
                                        "-I > /tmp/ipadd.txt", // cmd args
                                        0,                      // options
                                        VIX_INVALID_HANDLE,     // prop handle
                                        NULL,                   // callback
                                        NULL);                  // client data
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to run program in virtual machine '%s'(%" FMT64 "d %s)\n",
                vmpath, err, Vix_GetErrorText(err, NULL));
        goto abort;
    }

    jobHandle = VixVM_CopyFileFromGuestToHost(vmHandle,
                                              "/tmp/mem.txt",     // src file
                                              DEST_FILE,          // dst file
                                              0,                  // options
                                              VIX_INVALID_HANDLE, // prop list
                                              NULL,               // callback
                                              NULL);              // client data
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to copy file to the host '%s'(%" FMT64 "d %s)\n",
                vmpath, err, Vix_GetErrorText(err, NULL));
        goto abort;
    }

    jobHandle = VixVM_CopyFileFromGuestToHost(vmHandle,
                                              "/tmp/disk1.txt",     // src file
                                              DEST_FILE,          // dst file
                                              0,                  // options
                                              VIX_INVALID_HANDLE, // prop list
                                              NULL,               // callback
                                              NULL);              // client data
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to copy file to the host '%s'(%" FMT64 "d %s)\n",
                vmpath, err, Vix_GetErrorText(err, NULL));
        goto abort;
    }

    jobHandle = VixVM_CopyFileFromGuestToHost(vmHandle,
                                              "/tmp/ipadd.txt",     // src file
                                              DEST_FILE,          // dst file
                                              0,                  // options
                                              VIX_INVALID_HANDLE, // prop list
                                              NULL,               // callback
                                              NULL);              // client data
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to copy file to the host '%s'(%" FMT64 "d %s)\n",
                vmpath, err, Vix_GetErrorText(err, NULL));
        goto abort;
    }

    jobHandle = VixVM_DeleteFileInGuest(vmHandle,
                                        "/tmp/mem.txt", // filepath
                                        NULL,           // callback
                                        NULL);          // client data
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to delete file in virtual machine '%s'(%" FMT64 "d %s)\n",
                vmpath, err, Vix_GetErrorText(err, NULL));
        goto abort;
    }

    jobHandle = VixVM_DeleteFileInGuest(vmHandle,
                                        "/tmp/disk.txt", // filepath
                                        NULL,           // callback
                                        NULL);          // client data
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to delete file in virtual machine '%s'(%" FMT64 "d %s)\n",
                vmpath, err, Vix_GetErrorText(err, NULL));
        goto abort;
    }

    jobHandle = VixVM_DeleteFileInGuest(vmHandle,
                                        "/tmp/disk1.txt", // filepath
                                        NULL,           // callback
                                        NULL);          // client data
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to delete file in virtual machine '%s'(%" FMT64 "d %s)\n",
                vmpath, err, Vix_GetErrorText(err, NULL));
        goto abort;
    }

    jobHandle = VixVM_DeleteFileInGuest(vmHandle,
                                        "/tmp/ipadd.txt", // filepath
                                        NULL,           // callback
                                        NULL);          // client data
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_FAILED(err))
    {
        fprintf(stderr, "failed to delete file in virtual machine '%s'(%" FMT64 "d %s)\n",
                vmpath, err, Vix_GetErrorText(err, NULL));
        goto abort;
    }

abort:
    Vix_ReleaseHandle(vmHandle);
    VixHost_Disconnect(hostHandle);

    return 0;
}
