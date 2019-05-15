from django.db import models

# Create your models here.
class VirtualMachine(models.Model):
    name     = models.CharField(max_length = 32 , default = '')
    username = models.CharField(max_length = 32 , default = '')
    password = models.CharField(max_length = 32 , default = '')
    vmx_path = models.CharField(max_length = 128, default = '')

    def __str__(self):
        return self.name + " " + self.vmx_path