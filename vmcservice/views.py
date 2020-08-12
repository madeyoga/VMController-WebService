from django.shortcuts import render
from django.http import HttpResponse, JsonResponse, HttpResponseRedirect
from .models import VirtualMachine
from django.core import serializers

from django.contrib.auth.decorators import login_required
from django.contrib.auth import authenticate, login, logout
from django.contrib import messages

import subprocess
import json
import os

@login_required(login_url='/vmc/login/')
def index(request):
    """
    VMC homepage.
    lists all vms that the user have.
    """

    vms = VirtualMachine.objects.filter(user=request.user)
    vms_dictionary = {
        'vms' : vms
    }
    return render(request, 'vmcservice/index.html', vms_dictionary)

@login_required(login_url='/vmc/login/')
def vm_detail(request, vm_id):
    """
    Virtual Machine detail page.
    to get the virtual machine current information.
    such as memory usage. Online time. etc.
    """
    
    vm = VirtualMachine.objects.get(id=vm_id, user=request.user)
    dictionary = {
        'vm': vm
    }
    return render(request, "vmcservice/detail.html", dictionary)

def login_view(request):
    if request.method == 'POST':
        username = request.POST['username']
        password = request.POST['password']
        user = authenticate(request, username=username, password=password)
        if user:
            login(request, user)
            return HttpResponseRedirect('/vmc/')
        else:
            messages.error(request, 'username or password not correct')
    return render(request, 'vmcservice/login.html')

def logout_view(request):
    logout(request)
    return HttpResponseRedirect('/vmc/login/')

def virtual_machines(request):
    """
    Views to handle ajax request, to response with all vms data.
    url: localhost:8000/vmc/ajax/vms
    """

    data = serializers.serialize('json', VirtualMachine.objects.get(user=request.user))
    return HttpResponse(data, content_type='application/json')

def vmpower(request, vm_id, todo):
    """
    Views to power a virtual machine & update virtual machine status.
    url: localhost:8000/vmc/ajax/vmpower/<vm-id>/<command>
    """

    print(vm_id + " " + todo)
    vm = VirtualMachine.objects.get(id=vm_id, user=request.user)
    args = [
        'G:\\Kuliah\\github\\VMControlWebService\\vmcservice\\GuestOps\\Debug\\Power.exe',
        todo, vm.vmx_path
        ]
    output = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    print(output.returncode, output.stdout, output.stderr)
    if output.returncode == 0:
        # Update virtual machine status in database
        vm.status = todo
        vm.save()
    elif output.returncode == 1 and todo == 'off':
        vm.status = todo
        vm.save()
    elif output.returncode == 1 and todo == 'on':
        vm.status = todo
        vm.save()

    data = {
        'vmid'   : vm.id,
        'vmname'     : vm.name,
        'returncode' : output.returncode,
        'stdout' : output.stdout,
        'stderr' : output.stderr,
        'status' : todo
    }
    return HttpResponse(json.dumps(data), content_type='application/json')

def clone(request, vm_id, todo, clone_name):
    """
    views to clone a virtual machine.
    url: localhost:8000/vmc/ajax/clone/<vm-id>/[linked|full]/<vm-clone-name>

    Clone.exe <linked|full> <vmx-path> <clone-path>
    """

    vm = VirtualMachine.objects.get(id=vm_id, user=request.user)
    args = [
        'G:\\Kuliah\\github\\VMControlWebService\\vmcservice\\GuestOps\\Debug\\Clone.exe',
        todo, vm.vmx_path, clone_name
        ]
    output = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    print(output.returncode, output.stdout, output.stderr)
    if output.returncode == 1:
        # Insert new cloned vm data to database.
        # ...
        print("Cloned vm{}".format(vm_id))

    data = {
        'vmid'   : vm.id,
        'vmname'     : vm.name,
        'returncode' : output.returncode,
        'stdout' : output.stdout,
        'stderr' : output.stderr,
        'status' : todo
    }
    return HttpResponse(json.dumps(data), content_type='application/json')

def delete(request, vm_id):
    """
    views to delete a virtual machine
    url: localhost:8000/vmc/ajax/delete/<vm-id>

    Clone.exe DELETE <vmx-path>
    """

    vm = VirtualMachine.objects.get(id=vm_id, user=request.user)
    args = [
        'G:\\Kuliah\\github\\VMControlWebService\\vmcservice\\GuestOps\\Debug\\Clone.exe',
        "DELETE", vm.vmx_path
        ]
    output = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    print(output.returncode, output.stdout, output.stderr)
    if output.returncode == 1:
        # Remove vm data from database.
        # ...
        print("Deleted vm{}".format(vm_id))
    
    data = {
        'vmid'   : vm.id,
        'vmname'     : vm.name,
        'returncode' : output.returncode,
        'stdout' : output.stdout,
        'stderr' : output.stderr,
        'status' : todo
    }
    return HttpResponse(json.dumps(data), content_type='application/json')

def info(request, vm_id):
    """
    views to get virtual-machine's variables
    variables: Ip Address, Memory Usage, Disk Space usage.

    Info.exe <vmx-path>
    """

    vm = VirtualMachine.objects.get(id=vm_id, user=request.user)
    args = [
        'G:\\Kuliah\\github\\VMControlWebService\\vmcservice\\GuestOps\\Debug\\Info.exe',
        vm.vmx_path
    ]
    output = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    print(output.returncode, output.stdout, output.stderr)

    ipadd = '-'
    memory = '-'
    disk = '-'
    # read disk1.txt, ipadd.txt, mem.txt file
    with open('./vmcservice/GuestOps/Debug/ipadd.txt', 'r') as f:
        ipadd = f.readline()

    with open('./vmcservice/GuestOps/Debug/mem.txt', 'r') as f:
        memory = f.readline()
        memory = memory.split()
        memory_total = memory[1]
        memory_used = memory[2]
        memory_available = memory[6]

    with open('./vmcservice/GuestOps/Debug/disk1.txt', 'r') as f:
        disk = f.readline()
        disk = disk.split()
        disk_total = disk[1]
        disk_used = disk[2]
        disk_available = disk[3]

    data = {
        'vmid': vm.id,
        'vmname': vm.name,
        'returncode': output.returncode,
        'stdout': output.stdout,
        'stderr': output.stderr,
        'ipaddress': ipadd.rstrip(),
        'used_memory': memory_used,
        'available_memory': memory_available,
        'total_memory': memory_total,
        'total_disk': disk_total,
        'used_disk': disk_used,
        'available_disk': disk_available
    }

    return HttpResponse(json.dumps(data), content_type='application/json')

def run_script(request, vm_id):
    """
    views to run script in guest virtual machine.
    url: localhost:8000/vmc/ajax/runscript/<vm-id>
    """

    script_text = request.GET.get('scriptText')
    interpreter = request.GET.get('interpreter')

    vm = VirtualMachine.objects.get(id=vm_id, user=request.user)
    args = [
        'G:\\Kuliah\\github\\VMControlWebService\\vmcservice\\GuestOps\\Debug\\RunScript.exe',
        vm.username, vm.password, vm.vmx_path, interpreter, script_text
    ]
    output = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    print(output.returncode, output.stdout, output.stderr)

    data = {
        'vmid'   : vm.id,
        'vmname'     : vm.name,
        'returncode' : output.returncode,
        'stdout' : output.stdout,
        'stderr' : output.stderr
    }
    return HttpResponse(json.dumps(data), content_type='application/json')
