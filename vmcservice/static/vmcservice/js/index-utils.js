/**
 * get all vms from database.
 * gonna delete this soon.
 */
function getAllVm() {
    $.ajax({
        url: 'http://localhost:8000/vmc/ajax/vms',
        dataType: 'json',
        headers: {
            'Access-Control-Allow-Origin': '*',
        },
        success: function (data) {
            vmsElement = document.getElementById('vms');
            console.log(data);
            for (var i in data) { 
                vmsElement.innerHTML += data[i].model + "<br>"
                vmsElement.innerHTML += data[i].pk + "<br>"
                vmsElement.innerHTML += data[i].fields.name + "<br>"
                vmsElement.innerHTML += data[i].fields.username + "<br>"
                vmsElement.innerHTML += data[i].fields.password + "<br>"
            }
        }
    });
}

/**
 * 
 * @param {*} status <on/off/suspend>.
 */
function powerVM(id, status) {
    console.log(id, status);
    if (status == 'on') {
        status = 'off';
    } 
    else {
        status = 'on';
    }
    $.ajax({
        url: 'http://localhost:8000/vmc/ajax/vmpower/' + id + '/' + status,
        dataType: 'json',
        headers: {
            'Access-Control-Allow-Origin': '*',
        },
        success: function (data) {
            if (data.returncode == 0) {
                document.getElementById('vm-stat-' + data.vmid).innerHTML = data.status;
                if (data.status == 'on') {
                    var suspendButtonElement = document.createElement('button');
                    suspendButtonElement.innerHTML = "suspend";
                    suspendButtonElement.id = "suspend-" + data.vmid;
                    suspendButtonElement.onclick = function() {
                        this.parentNode.removeChild(this);
                        suspendVM(data.vmid);
                    };
                    document.getElementById('vm-' + data.vmid).appendChild(suspendButtonElement);
                }
                else {
                    document.getElementById('suspend-' + data.vmid).remove();
                }
            } 
            else {
                alert("Failed to turn " + data.status + " " + data.vmname)
            }
            console.log('return data ', data);
        }
    });
    document.getElementById('vm-stat-' + id).innerHTML = "";
}
/**
 * 
 * @param {*} id 
 * suspend a virtual machine from a given virtual-machine-id.
 */
function suspendVM(id) {
    $.ajax({
        url: 'http://localhost:8000/vmc/ajax/vmpower/' + id + '/suspend',
        dataType: 'json',
        headers: {
            'Access-Control-Allow-Origin': '*',
        },
        success: function (data) {
            if (data.returncode == 0) {
                document.getElementById('vm-stat-' + data.vmid).innerHTML = data.status;
            } 
            else {
                alert("Failed to " + data.status + " " + data.vmname)
            }
            console.log('return data ', data);
        }
    });
    document.getElementById('vm-stat-' + id).innerHTML = "";
}
