from django.shortcuts import render
from django.http import HttpResponse, JsonResponse
from .models import VirtualMachine
from django.core import serializers

from django.contrib.auth.decorators import login_required
from django.contrib.auth import authenticate, login, logout
from django.contrib import messages

import subprocess
import json

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

def login_view(request):
    if request.method == 'POST':
        username = request.POST['username']
        password = request.POST['password']
        user = authenticate(request, username=username, password=password)
        if user:
            login(request, user)
            return index(request)
        else:
            messages.error(request, 'username or password not correct')
    return render(request, 'vmcservice/login.html')

def logout_view(request):
    logout(request)
    return index(request)

def virtual_machines(request):
    """
    Views to handle ajax request, to response with all vms data.
    url: localhost:8000/vmc/ajax/vms
    """

    data = serializers.serialize('json', VirtualMachine.objects.all())
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