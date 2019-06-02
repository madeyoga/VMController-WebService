from django.urls import path
from django.conf.urls import url
from django.conf import settings
from . import views

urlpatterns = [
    path('', views.index, name='vmcindex'),
    path('home/', views.index, name='vmchome'),
    url(r'^detail/(?P<vm_id>\d+)$', views.vm_detail, name='vmcdetail'),
    path('ajax/vms', views.virtual_machines, name='vmcvms'),
    path('login/', views.login_view, name='vmclogin'),
    url(r'^logout/$', views.logout_view, name='vmclogout'),

    url(r'^ajax/vmpower/(?P<vm_id>\d+)/(?P<todo>\D+)$', views.vmpower, name='vmcvmpower'),
    url(r'^ajax/clone/(?P<vm_id>\d+)/(?P<todo>\D+)/(?P<clone_name>\D+)$', views.clone, name='vmclone'),
    url(r'^ajax/delete/(?P<vm_id>\d+)$', views.delete, name='vmdelete'),
    url(r'^ajax/info/(?P<vm_id>\d+)$', views.info, name='vmcinfo'),
    url(r'^ajax/runscript/(?P<vm_id>\d+)$', views.run_script, name='vmcrunscript'),
]