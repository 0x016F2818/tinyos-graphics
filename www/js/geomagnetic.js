$(document).ready(function() {
    $("#nodeID").html("node ID:"+nodeID);
    $("em#nodeID").html("node ID:"+nodeID);

    var sense1 = "x_mag";
    var sense2 = "y_mag";
    drawDynamicSpline(networkName, nodeID, sense1, "dynamicspline1");
    drawDynamicSpline2(networkName, nodeID, sense2, "dynamicspline2");
    drawSymbolsSpline();
    drawRotatedColumn();
    drawMaxandMinColumn();
    drawGradientPie();
})
