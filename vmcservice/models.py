from django.db import models
from django.contrib.auth.models import User

class VirtualMachine(models.Model):
    """
    One user has many Virtual Machine.
    One virtual machine owned by one user.
    """

    user     = models.ForeignKey(User, on_delete=models.CASCADE, null=True)
    name     = models.CharField(max_length = 32 , default = '')
    username = models.CharField(max_length = 32 , default = '')
    password = models.CharField(max_length = 32 , default = '')
    vmx_path = models.CharField(max_length = 128, default = '')
    status   = models.CharField(max_length = 8  , default = 'off')
    operating_system = models.CharField(max_length = 128, default = 'Ubuntu Server 18.04.2 LTS')
    
    def __str__(self):
        return self.name + " " + self.vmx_path + " " + self.status
