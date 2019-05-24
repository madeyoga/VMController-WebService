from django.urls import path
from django.conf.urls import url
from django.conf import settings
from . import views

urlpatterns = [
    path('', views.index, name='vmcindex'),
    path('ajax/vms', views.virtual_machines, name='vmcvms'),
    path('login/', views.login_view, name='vmclogin'),
    url(r'^logout/$', views.logout_view, name='vmclogout'),
    url(r'^ajax/vmpower/(?P<vm_id>\d+)/(?P<todo>\D+)$', views.vmpower, name='vmcvmpower'),
]