$(function() {
    // show the node id
    $("#nodeID").html("node ID:"+nodeID);
    $("em#nodeID").html("node ID:"+nodeID);
    // localStorage.clear();
    // localStorage.removeItem("requestNode");

    // drawDynamicSpline();
    drawRotatedColumn();
    drawGradientPie();
    drawMaxandMinColumn();
    drawSymbolsSpline()
})
