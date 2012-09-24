$(function() {

    // control the index.html
    if($.browser.chrome) {
        $("#clockIEFF").addClass("hide");
        $("#clockSVG").ready(updateTime);
    } else {
        $("#clockSVG").addClass("hide");
        $("#clockIEFF").removeClass("hide");
        $("#clock2").load(displayTime);
    }

    // send post request and put the node object into array(nodeSlot)
    var nodeSlot = new Array(), 
    networkSlot = new Array(),
    i,
    xmlhttp = createxmlhttp();
    go(xmlhttp, "POST", "nodeInfo.wsn", "false", "null");
    // put the node information into the array nodeSlot
    getNodeBasicInformation(networkSlot);
    
    for(i = 0; i < networkSlot.length; i++) {
        // draw the node distribute
        drawNetworkChart(networkSlot[i]["networkId"], networkSlot[i]["networkName"]);
        drawNode(networkSlot[i]["networkId"], networkSlot[i]["networkName"], networkSlot[i].nodeSlot);
    }

})
