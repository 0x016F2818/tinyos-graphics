$(document).ready(function() {
    $("#nodeID").html("node ID:"+nodeID);
    $("em#nodeID").html("node ID:"+nodeID);

    var sense1 = "x_mag", 
    sense2 = "y_mag";
    // alert(networkName+nodeID+sense);
    drawDynamicSpline(networkName, nodeID, sense1, "dynamicspline");
    // drawDynamicSpline(networkName, nodeID, sense2, "dynamicspline2");
    drawSymbolsSpline();
    drawRotatedColumn();
    drawMaxandMinColumn();
    drawGradientPie();
})
