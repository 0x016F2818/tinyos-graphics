$(document).ready(function() {
    $("#nodeID").html("node ID:"+nodeID);
    $("em#nodeID").html("node ID:"+nodeID);

    var sense1 = "x_acc"; 
    var sense2 = "y_acc";
    drawDynamicSpline(networkName, nodeID, sense2, "dynamicspline");
    // drawDynamicSpline(networkName, nodeID, sense2, "dynamicspline2");
    drawSymbolsSpline();
    drawRotatedColumn();
    drawMaxandMinColumn();
    drawGradientPie();
})
