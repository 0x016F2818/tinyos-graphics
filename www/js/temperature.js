$(document).ready(function() {
    // drawSpline('temperature');
    // drawSpline('container');

    var json_obj = {
        net_name: networkName,
        nodeId: nodeID,
        sense: "temp",
        requestNum: 30
    },
    flag, 
    json_text = JSON.stringify(json_obj, null, 2),
    xmlhttp = createxmlhttp();
    go(xmlhttp, "POST", "realTime.wsn", json_text);

    drawDynamicSpline();
    drawSymbolsSpline();
    // drawRotatedColumn();
    // drawMaxandMinColumn();
    // drawGradientPie();
})
