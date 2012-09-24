$(document).ready(function() {
    // drawSpline('temperature');
    // drawSpline('container');

    // var json_obj = {
    //     net_name: networkName,
    //     nodeId: nodeID,
    //     sense: "temp",
    //     requestNum: 30
    // },
    // firstFlag = true, 
    // json_text = JSON.stringify(json_obj, null, 2),
    // xmlhttp = createxmlhttp();
    // if(firstFlag) {
    //     go(xmlhttp, "POST", "realTime.wsn", json_text);
    // } else {
    //     var json_obj = {
    //         net_name: networkName,
    //         nodeId: nodeID,
    //         sense: "temp"
    //     };
    //     setInterval(function() {
    //         go(xmlhttp, "POST", "realTime.wsn", json_text)
    //     }, 1000);
    // }
    $("#nodeID").html("node ID:"+nodeID);
    $("em#nodeID").html("node ID:"+nodeID);

    var sense = "temperature";
    // alert(networkName+nodeID+sense);
    drawDynamicSpline(networkName, nodeID, sense);
    drawSymbolsSpline();
    drawRotatedColumn();
    drawMaxandMinColumn();
    drawGradientPie();
})
