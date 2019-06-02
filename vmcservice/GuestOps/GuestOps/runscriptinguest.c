#include <stdio.h>
#include <stdlib.h>

#include "vix.h"

/*
* guest OS username/password
*#define  GUEST_USERNAME "hnp"
#define  GUEST_PASSWORD "hnPalit"

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

#define  VMPOWEROPTIONS   VIX_VMPOWEROP_LAUNCH_GUI

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

#define  TOOLS_TIMEOUT  300

#ifdef _WIN32
#define DEST_FILE ".\\outfile"
#else
#define DEST_FILE "./outfile.txt"
#endif

#define GUEST_USERNAME "hnp"
#define GUEST_PASSWORD "hnPalit"

static char *progname;

static void usage()
{
	fprintf(stderr, "Usage: %s <user> <pass> <vmx-path> <interpreter> <script-text>\n", progname);
	fprintf(stderr, "%s\n", VMXPATH_INFO);
}

int main(int argc, char **argv)
{
	char       *vmpath;
	char	   *interpreter;
	char	   *scripttext;
	char	   *user, *pass;
	VixError    err;
	VixHandle   hostHandle = VIX_INVALID_HANDLE;
	VixHandle   jobHandle = VIX_INVALID_HANDLE;
	VixHandle   vmHandle = VIX_INVALID_HANDLE;

	progname = argv[0];
	if (argc > 1) 
	{
		user = argv[1];
		pass = argv[2];
		vmpath = argv[3];
		if (strcmp(argv[4], "python") == 0)
		{
			interpreter = "/usr/bin/python3";
		}
		else if (strcmp(argv[4], "perl") == 0)
		{
			interpreter = "/usr/bin/perl";
		}
		else if (strcmp(argv[4], "bash") == 0)
		{
			interpreter = "/usr/bin/bash";
		}
		else
		{
			printf("Interpreter Error unknown %s\n", argv[4]);
			exit(EXIT_FAILURE);
		}
		scripttext = argv[5];
	}
	else 
	{
		usage();
		exit(EXIT_FAILURE);
	}

	jobHandle = VixHost_Connect(VIX_API_VERSION,          // api version
								CONNTYPE,                 // connection type
								HOSTNAME,                 // host name
								HOSTPORT,                 // host port
								USERNAME,                 // username
								PASSWORD,                 // passwd
								0,                        // options
								VIX_INVALID_HANDLE,       // property list handle
								NULL,                     // callback
								NULL);                    // client data

	err = VixJob_Wait(jobHandle, VIX_PROPERTY_JOB_RESULT_HANDLE,
		&hostHandle, VIX_PROPERTY_NONE);
	Vix_ReleaseHandle(jobHandle);
	if (VIX_FAILED(err)) 
	{
		fprintf(stderr, "failed to connect to host (%"FMT64"d %s)\n", err,
			Vix_GetErrorText(err, NULL));
		goto abort;
	}
	//printf("connected to host (%d)\n", hostHandle);

	//printf("about to open %s\n", vmpath);
	jobHandle = VixVM_Open( hostHandle,               // host connection
							vmpath,                   // path to vmx
							NULL,                     // callback
							NULL);                    // client data
	err = VixJob_Wait(jobHandle, VIX_PROPERTY_JOB_RESULT_HANDLE,
		&vmHandle, VIX_PROPERTY_NONE);
	Vix_ReleaseHandle(jobHandle);
	if (VIX_FAILED(err)) 
	{
		fprintf(stderr, "failed to open virtual machine '%s'(%"FMT64"d %s)\n", vmpath, err,
			Vix_GetErrorText(err, NULL));
		goto abort;
	}
	//printf("opened %s (%d)\n", vmpath, vmHandle);

	jobHandle = VixVM_LoginInGuest( vmHandle,
									user,					  // guest OS user
									pass,					  // guest OS passwd
									0,                        // options
									NULL,                     // callback
									NULL);                    // client data
	err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
	Vix_ReleaseHandle(jobHandle);
	if (VIX_FAILED(err)) 
	{
		fprintf(stderr, "failed to login to virtual machine '%s'(%"FMT64"d %s)\n", vmpath, err,
			Vix_GetErrorText(err, NULL));
		goto abort;
	}
	printf("logged in to guest\n");

	printf(scripttext);
	// Run the target program.
	jobHandle = VixVM_RunScriptInGuest( vmHandle,
										interpreter,
										scripttext,
										0, // options,
										VIX_INVALID_HANDLE, // propertyListHandle,
										NULL, // callbackProc,
										NULL); // clientData

	err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);

	if (VIX_OK != err) 
	{
		// Handle the error...
		fprintf(stderr, "\nfailed to run script in virtual machine '%s'(%"FMT64"d %s)\n", vmpath, err,
			Vix_GetErrorText(err, NULL));
		goto abort;
	} 
	else 
	{
		printf("\nRun script successful.\n");
	}
	return 0;
abort:
	Vix_ReleaseHandle(vmHandle);
	VixHost_Disconnect(hostHandle);

	return 1;
}