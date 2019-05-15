from django.shortcuts import render
from django.http import HttpResponse, JsonResponse
from .models import VirtualMachine
from django.core import serializers

# Create your views here.
def index(request):
    return render(request, 'vmcservice/index.html')

def virtual_machines(request):
    data = serializers.serialize('json', VirtualMachine.objects.all())
    return HttpResponse(data, content_type='application/json')