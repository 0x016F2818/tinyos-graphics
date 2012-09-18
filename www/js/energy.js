var energy = {},
n = 50,
i;

for(i=0; i<50;i++){
    energy[i]=100*Math.random();
}

function showEnergy(value) {
    var type;
    if(value>60) {
        type = "success";
        value = value+"%";
    } else if (value>10) {
        type = "warning";
        value = value+"%";
    } else if (value>0) {
        type = "danger";
        value = value+"%";
    }
    var dangerElement = "<div class=\"progress progress-"+type+"\">"
    var percentageElement = "<div class=\"bar\" color=\"#333\" style=\"width:"+value+";color:#333;\">"+value+"<\/div>"
    var divEndElement = "</\div>"
    return dangerElement+percentageElement+divEndElement
}


$(function() {
    for (i = 0; i < n; i++) {
        id = i+1;
        $("#energy").append(
            "Node "+ id
        ).append(
            showEnergy(energy[i])
        ).css({
            'color':'#000000'
        })
    }
})
