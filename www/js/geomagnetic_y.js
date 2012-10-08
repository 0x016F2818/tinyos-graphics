$(document).ready(function() {
    $("#nodeID").html("node ID:"+nodeID);
    $("em#nodeID").html("node ID:"+nodeID);

    var sense = "y_mag";
    drawDynamicSpline(networkName, nodeID, sense, "dynamicspline2");
})
