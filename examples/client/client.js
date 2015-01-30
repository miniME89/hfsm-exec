poll = function(index) {
    $.ajax({
        url: 'http://localhost:8080/log',
        type: 'GET',
        success: function(data, textStatus, jqXHR) {
            if (jqXHR.status == 200) {
                console.log(data);
            }
        },
        error: function(jqXHR, textStatus) {
            console.error(jqXHR);
        },
        beforeSend: function(jqXHR) {
            jqXHR.setRequestHeader('Push-Notification-Index', index || 0);
        },
        complete: function(jqXHR) {
            var index = jqXHR.getResponseHeader('Push-Notification-Index');
            poll(index);
        }
    });
};

poll();
