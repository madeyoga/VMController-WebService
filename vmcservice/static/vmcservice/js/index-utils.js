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

getAllVm();