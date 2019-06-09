# VMController-WebService
Automate virtual machine operations and manipulate files within guest operating systems with VIX API and use Django WebFramework as the webservice backend.

## Requirements
- Python 3.
- Django web framework.
- VMware Workstation.
- VIX API
- Installed Virtual Machines.

## Getting started
- Clone Project.
```
$ git clone https://github.com/madeyoga/VMController-WebService.git
$ cd VMController-WebService-master/
```
- Create super user
```
$ python manage.py createsuperuser
```
- Run server.
```
$ python manage.py runserver

Django version 2.1.3, using settings 'Project-Name.settings'
Starting development server at http://127.0.0.1:8000/
Quit the server with CTRL-BREAK.
```
- Open browser & go to `localhost:8000/admin`. 
- Login & open `Virtual Machines` table.

`User` has 1 to many relation with Virtual Machine table. so, 1 user can own many Virtual-Machines & 1 Virtual Machine Owned by only 1 User.
you have to create a new Virtual Machine & assign it to a specific user.

- Add new Virtual Machine. or edit the existing one.
- Open new tab and go to `localhost:8000/vmc/`
