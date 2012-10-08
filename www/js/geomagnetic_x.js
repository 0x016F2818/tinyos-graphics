$(document).ready(function() {
    $("#nodeID").html("node ID:"+nodeID);
    $("em#nodeID").html("node ID:"+nodeID);

    var sense = "x_mag";
    drawDynamicSpline(networkName, nodeID, sense, "dynamicspline1");
})
