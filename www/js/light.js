$(document).ready(function() {
    $("#nodeID").html("node ID:"+nodeID);
    $("em#nodeID").html("node ID:"+nodeID);

    var sense = "brightness";
    drawDynamicSpline(networkName, nodeID, sense);
    drawSymbolsSpline();
    drawRotatedColumn();
    drawMaxandMinColumn();
    drawGradientPie();
})
