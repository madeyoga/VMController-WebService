 /**
  * 
  * @param {*} vmId | 
  */
 function getInfo(vmId) {
    refreshButton.disabled = true;
    console.log("Getting info vm" + vmId);
    $.ajax({
        url: '/vmc/ajax/info/' + vmId,
        dataType: 'json',
        headers: {
            'Access-Control-Allow-Origin': '*',
        },
        success: function (data) {
            refreshButton.disabled = false;
            console.log(data)
            if (data.returncode == 0) {
                ipAddress.innerHTML = data.ipaddress;

                diskLabel = document.getElementById('disk-label');
                memoLabel = document.getElementById('memo-label');

                diskLabel.innerHTML = "Disk Space Usage: " + data.used_disk + "/" + data.total_disk 
                memoLabel.innerHTML = "Memory Usage: " + data.used_memory + "MB/" + data.total_memory + "MB"

                diskPercentage = Math.floor((parseFloat(data.used_disk) / parseFloat(data.total_disk)) * 100)
                memoPercentage = Math.floor((parseFloat(data.used_memory) / parseFloat(data.total_memory)) * 100)

                diskElement = document.getElementById('disk-usage');
                diskElement.style.width = diskPercentage + "%";
                diskElement.innerHTML = diskPercentage + "%";

                memoElement = document.getElementById('memo-usage');
                memoElement.style.width = memoPercentage + "%";
                memoElement.innerHTML = memoPercentage + "%";
                
                
                if (diskPercentage < 50) {
                    diskElement.className = "progress-bar progress-bar-success";
                } else if (diskPercentage < 80) {
                    diskElement.className = "progress-bar progress-bar-warning";
                } else {
                    diskElement.className = "progress-bar progress-bar-danger";
                }

                if (memoPercentage < 50) {
                    memoElement.className = "progress-bar progress-bar-success";
                } else if (memoPercentage < 80) {
                    memoElement.className = "progress-bar progress-bar-warning";
                } else {
                    memoElement.className = "progress-bar progress-bar-danger";
                }
            }
            else {
                alert(data.stderr);
                diskElement = document.getElementById('disk-usage');
                diskElement.style.width = "0%";
                diskElement.innerHTML = "";

                memoElement = document.getElementById('memo-usage');
                memoElement.style.width = "0%";
                memoElement.innerHTML = "";
            }
        }
    });
}

var interpreter = 'python';

var inputFileElement = document.getElementById('inputFile');
inputFileElement.addEventListener("change", displayScriptOnInput);

var scriptText = "";

var reader = new FileReader();
reader.addEventListener('load', function(e) {
    // set content;
    var codeBlockElement = document.getElementById('code-block');
    codeBlockElement.innerHTML = e.target.result;
    scriptText = e.target.result;
    Prism.highlightElement(codeBlockElement);
});

function displayScriptOnInput() {
    if (this.files && this.files[0]) {
        var file = this.files[0];
        reader.readAsBinaryString(file);
    }
}

function changeLang(selectObject) {
    var codeBlockElement = document.getElementById('code-block');
    var value = selectObject;
    if (value.value == 'python') {
        codeBlockElement.className = 'language-python';
        interpreter = 'python';
    } else if (value.value == 'perl') {
        codeBlockElement.className = 'language-perl';
        interpreter = 'perl';
    } else {
        codeBlockElement.className = 'language-bash';
        interpreter = 'bash';
    }
    Prism.highlightElement(codeBlockElement);
    console.log("changed to " + value.value);
}

function runScript(vmid) {
    console.log(scriptText);
    if (scriptText.trim().length == 0) {
        alert('script-text is empty');
        return;
    }
    $.ajax({
        url: '/vmc/ajax/runscript/' + vmid,
        dataType: 'json',
        headers: {
            'Access-Control-Allow-Origin': '*',
        },
        data: {
            scriptText: scriptText,
            interpreter: interpreter
        },
        success: function (data) {
            console.log(data);
            if (data.returncode == 0) {
                alert("Run script success.");
            } else {
                alert(data.stdout);
            }
        }
    })
}