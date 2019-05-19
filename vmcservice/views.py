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
    Views to returns all vms from ajax request.
    url: localhost:8000/vmc/ajax/vms
    """

    data = serializers.serialize('json', VirtualMachine.objects.all())
    return HttpResponse(data, content_type='application/json')

def vmpower(request, key):
    """
    Views to update vm status.
    url: localhost:8000/vmc/ajax/vms
    """

    print(key)
    vm = VirtualMachine.objects.get(user=request.user)
    args = [
        'G:\\Kuliah\\github\\VMControlWebService\\vmcservice\\GuestOps\\Debug\\Power.exe',
        key, vm.vmx_path
        ]
    # output = subprocess.check_output(args)
    output = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    print(output.returncode, output.stdout, output.stderr)
    data = {
        'returncode' : output.returncode,
        'stdout' : output.stdout,
        'stderr' : output.stderr,
        'status' : 'OK'
    }
    return HttpResponse(json.dumps(data), content_type='application/json')
