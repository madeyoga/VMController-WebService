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
 * @param {*} status On, Off, Suspend.
 */
function power(status) {

}

function powerOn() {
    $.ajax({
        url: 'http://localhost:8000/vmc/ajax/vmpower/on/',
        dataType: 'json',
        headers: {
            'Access-Control-Allow-Origin': '*',
        },
        success: function (data) {
            document.getElementById('vm-stat').innerHTML = 'On';
            document.getElementById('vm-stat').onclick = powerOff;
            console.log('return data', data);
        }
    });
    document.getElementById('vm-stat').onclick = null;
}

function powerOff() {
    $.ajax({
        url: 'http://localhost:8000/vmc/ajax/vmpower/off/',
        dataType: 'json',
        headers: {
            'Access-Control-Allow-Origin': '*',
        },
        success: function (data) {
            document.getElementById('vm-stat').innerHTML = 'Off';
            document.getElementById('vm-stat').onclick = powerOn;
            console.log('return data', data);
        }
    });
    document.getElementById('vm-stat').onclick = null;
}
