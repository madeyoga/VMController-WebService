from django.urls import path
from . import views

urlpatterns = [
    path('', views.index, name='vmcindex'),
    path('ajax/vms', views.virtual_machines, name='vmcvms')
]