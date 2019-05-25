/* This demonstrates how to open a virtual machine,
* power it on, and power it off.
*
* This uses the VixJob_Wait function to block after starting each
* asynchronous function. This effectively makes the asynchronous
* functions synchronous, because VixJob_Wait will not return until the
* asynchronous function has completed.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "vix.h"


/*
* Certain arguments differ when using VIX with VMware Server 2.0
* and VMware Workstation.
*
* Comment out this definition to use this code with VMware Server 2.0.
*/
#define USE_WORKSTATION

#ifdef USE_WORKSTATION

#define  CONNTYPE    VIX_SERVICEPROVIDER_VMWARE_WORKSTATION

#define  HOSTNAME ""
#define  HOSTPORT 0
#define  USERNAME ""
#define  PASSWORD ""

#define  VMPOWEROPTIONS   VIX_VMPOWEROP_LAUNCH_GUI   // Launches the VMware Workstaion UI
// when powering on the virtual machine.

#define VMXPATH_INFO "where vmxpath is an absolute path to the .vmx file " \
                     "for the virtual machine."

#else    // USE_WORKSTATION

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

#define  VMPOWEROPTIONS VIX_VMPOWEROP_NORMAL

#define VMXPATH_INFO "where vmxpath is a datastore-relative path to the " \
                     ".vmx file for the virtual machine, such as "        \
                     "\"[standard] ubuntu/ubuntu.vmx\"."

#endif    // USE_WORKSTATION


/*
* Global variables.
*/

static char *progName;


/*
* Local functions.
*/
static void
usage()
{
	fprintf(stderr, "\nUsage: <program> <COMMAND> <vmx-file> [<cl-vmx-file>] COMMAND can be DELETE, FCLONE, or LCLONE. %s", progName);
	fprintf(stderr, "%s\n", VMXPATH_INFO);
}

int main(int argc, char **argv)
{
	VixError err;
	char *vmxPath;
	char *vmxClone = "";
	VixHandle hostHandle = VIX_INVALID_HANDLE;
	VixHandle jobHandle = VIX_INVALID_HANDLE;
	VixHandle vmHandle = VIX_INVALID_HANDLE;
	VixHandle cloneVMHandle = VIX_INVALID_HANDLE;

	progName = argv[0];
	char *todo = "";

	if (argc == 3) {
		vmxPath = argv[2];
		todo = argv[1];
		int i = 0;
		while (argv[1][i]) {
			todo[i] = toupper(argv[1][i]);
			i++;
		}
		printf(todo);
	}
	else if (argc == 4) {
		vmxPath = argv[2];
		vmxClone = argv[3];
		todo = argv[1];
		int i = 0;
		while (argv[1][i]) {
			todo[i] = toupper(argv[1][i]);
			i++;
		}
		printf(todo);
	}
	else {
		usage();
		exit(EXIT_FAILURE);
	}

	jobHandle = VixHost_Connect(VIX_API_VERSION,
		CONNTYPE,
		HOSTNAME, // *hostName,
		HOSTPORT, // hostPort,
		USERNAME, // *userName,
		PASSWORD, // *password,
		0, // options,
		VIX_INVALID_HANDLE, // propertyListHandle,
		NULL, // *callbackProc,
		NULL); // *clientData);
	err = VixJob_Wait(jobHandle,
		VIX_PROPERTY_JOB_RESULT_HANDLE,
		&hostHandle,
		VIX_PROPERTY_NONE);
	if (VIX_FAILED(err)) {
		goto abort;
	}

	Vix_ReleaseHandle(jobHandle);
	jobHandle = VixVM_Open(hostHandle,
		vmxPath,
		NULL, // VixEventProc *callbackProc,
		NULL); // void *clientData);
	err = VixJob_Wait(jobHandle,
		VIX_PROPERTY_JOB_RESULT_HANDLE,
		&vmHandle,
		VIX_PROPERTY_NONE);
	if (VIX_FAILED(err)) {
		goto abort;
	}

	if (strcmp(todo, "DELETE") == 0) {
		Vix_ReleaseHandle(jobHandle);
		jobHandle = VixVM_Delete(
			vmHandle,
			VIX_VMDELETE_DISK_FILES,
			NULL,
			NULL
		);
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		if (VIX_OK != err) {
			// Handle the error...
			goto abort;
		}
		Vix_ReleaseHandle(jobHandle);
	}
	else if (strcmp(todo, "FCLONE") == 0) {
		Vix_ReleaseHandle(jobHandle);
		printf("\nF-CLONING!\n");
		jobHandle = VixVM_Clone(vmHandle,
			VIX_INVALID_HANDLE,  // snapshotHandle
			VIX_CLONETYPE_FULL,  // cloneType
			vmxClone,  // destConfigPathName
			0,  //options,
			VIX_INVALID_HANDLE,  // propertyListHandle
			NULL,  // callbackProc
			NULL);  // clientData
		err = VixJob_Wait(jobHandle,
			VIX_PROPERTY_JOB_RESULT_HANDLE,
			&cloneVMHandle,
			VIX_PROPERTY_NONE);
		if (VIX_FAILED(err)) {
			// Handle the error...
			printf(err);
			goto abort;
		}
		Vix_ReleaseHandle(jobHandle);
		printf("F-CLONED!\n");
	}
	else if (strcmp(todo, "LCLONE") == 0) {
		Vix_ReleaseHandle(jobHandle);
		printf("L-CLONING!");
		// Create a clone of this virtual machine.
		jobHandle = VixVM_Clone(vmHandle,
			VIX_INVALID_HANDLE,  // snapshotHandle
			VIX_CLONETYPE_LINKED,  // cloneType
			vmxClone,  // destConfigPathName
			0,  //options,
			VIX_INVALID_HANDLE,  // propertyListHandle
			NULL,  // callbackProc
			NULL);  // clientData
		err = VixJob_Wait(jobHandle,
			VIX_PROPERTY_JOB_RESULT_HANDLE,
			&cloneVMHandle,
			VIX_PROPERTY_NONE);
		if (VIX_FAILED(err)) {
			// Handle the error...
			goto abort;
		}
		Vix_ReleaseHandle(jobHandle);
		printf("L-CLONED!");
	}
	else {
		usage();
	}

abort:
	Vix_ReleaseHandle(jobHandle);
	Vix_ReleaseHandle(vmHandle);
	Vix_ReleaseHandle(cloneVMHandle);
	VixHost_Disconnect(hostHandle);
	return 0;
}

