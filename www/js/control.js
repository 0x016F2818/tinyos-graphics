$(function() {

    /**
     * jQuery Tiny Pub/Sub -v0.7 - 10/27/2011
     * http://benalman.com/
     * Copyright (c) 2011 "Cowboy" Ben Alman; Licensed MIT, GPL
     **/
    (function($) {
	var o = $({});

	$.subscribe = function() {
	    o.on.apply(o, arguments);
	};
	
	$.unsubscribe = function() {
	    o.off.apply(o, arguments);
	};

	$.publish = function() {
	    o.trigger.apply(o, arguments);
	};
    }(jQuery));

    
    function draw(event, networkMuster) {
	for(var i in networkMuster) {
	    // draw the node distribute
	    var network = networkMuster[i];
	    drawNetworkChart(network["networkId"], network["networkName"]);
	    network.drawNode();
	    // drawNode(network["networkId"], network["networkName"], network.nodeSlot);
	    // network.eraseNodeById(0);
	}
    }

    $.subscribe("network", draw);
    // setInterval(function() {

    $.ajax({
        type: "POST",
        url: "modules/nodeInfo.wsn",
	async: false
    }).success(function(data) {
        var networkMuster = [];
        for(var i in data) {
            var network = new Network,
            nodes = data[i]["nodes"];
            network.init({"networkId": data[i]["network_id"], "networkName": data[i]["network_name"]});
            for(var j in nodes) {
                network.addNode({
                    "nodeId": nodes[j]["node_id"],
                    "parentId": nodes[j]["parent_id"],
                    "position_x": nodes[j]["position_x"],
                    "position_y": nodes[j]["position_y"],
                    "energy": nodes[j]["power"]
                })
            }
            networkMuster.push(network);
        }
	$.publish("network",[networkMuster]);
    }).fail(function() {
	window.location = "404.htm";
    });
    

    // }, 1000);

})