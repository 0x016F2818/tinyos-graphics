/**
 * Class: Node
 **/
var Node = function() {
    this.nodeId = 0;
    this.parentId = 0;
    this.position_x = 0;
    this.position_y = 0;
    this.energy = 0;

    this.sense = [];

    this.temp = [];
    this.light = [];
    this.sound = [];

    this.node = this;
}
Node.prototype.init = function(data) {
    this.nodeId = data["nodeId"] | 0;
    this.parentId = data["parentId"] | 0;
    this.position_x = data["position_x"] | 0;
    this.position_y = data["position_y"] | 0;
    this.energy = data["energy"] | 0;
}
Node.prototype.addSense = function(sense) {
    if(sense instanceof Array) {
        for(var i in sense)
            this.sense.push(sense[i]);   
    }
    else {
        this.sense.push(sense);
    }
}
Node.prototype.clearSense = function(sense) {
    this.sense = [];
}
Node.prototype.addTemp = function(temp) {
    if(temp instanceof Array) {
        for(var i in temp)
            this.temp.push(temp[i]);   
    }
    else {
        this.temp.push(temp);
    }
}
Node.prototype.addLight = function(light) {
    if(light instanceof Array) {
        for(var i in light)
            this.light.push(light[i]);   
    }
    else {
        this.light.push(light);
    }
}
Node.prototype.addSound = function(sound) {
    if(sound instanceof Array) {
        for(var i in sound)
            this.sound.push(sound[i]);   
    }
    else {
        this.sound.push(sound);
    }
}
Node.prototype.drawDynamicSpline = function(info) {
    var name, unit,
    node = this.node,
    // addSense,
    // sense,
    senseType = info["sense"],
    networkName = info["networkName"],
    // networkId = data["networkId"],
    nodeId = this.nodeId;
    // readerName = data["readerName"];

    switch(senseType){
    case "temp":
        name = "Temperature spline";
        unit = '°C';
        // addSense = node.addTemp;
        // sense = node.temp;
        break;

    case "photo":
        name = "Light spline";
        unit = 'lux';
        // addSense = node.addLight;
        // sense = node.light;
        break;

    case "sound":
        name = "Sound spline";
        unit = 'Hz';
        // addSense = node.addSound;
        // sense = node.sound;
        break;

    case "x_acc":
        name = "Sound spline";
        unit = 'Hz';
        break;

    case "y_acc":
        name = "Sound spline";
        unit = 'Hz';
        break;

    default:
        break;
    }


    Highcharts.setOptions({
        global: {
            useUTC: false
        }
    });

    var spline;
    spline = new Highcharts.Chart({
        chart: {
            renderTo: 'bodyOfChart',//container is a vector
            // type: 'spline',
            // marginRight: 10,
            zoomType: 'x',
            events: {
                load: function() {
                    // set up the updating of the chart each second
                    var series = this.series[0],
                    lastCaptureTime = node.sense[0]["time"],
                    // lastCaptureTime = 0,
                    json_obj = {
                        net_name: networkName,
                        nodeId: nodeId,
                        sense: senseType,
                        requestNum: "1"
                    },
                    json_text = JSON.stringify(json_obj, null, 2);

                    timer = setInterval(function() {
                        $.ajax({
                            url: "modules/realTime.wsn",
                            type: "POST",
                            data: json_text,
                            async: false
                        }).success(function(msg) {
                            var x = parseInt(msg[0]["time"]+'000'),
                            y = msg[0]["value"];
                            if( x !== lastCaptureTime && x !== 0 ) { 
                                series.addPoint([x, y], true, true);
                                lastCaptureTime = x;
                            }
                        }).complete(function() {
                            node.sense = [];
                        }).fail(function() {
			    window.location = "404.htm";
			});
                    }, 1000);
                }
            }
        },
        title: {
            text: name
        },
        xAxis: {
            type: 'datetime',
            tickPixelInterval: 44,
            // plotBands: [{
            //     id: 'mask-before',
            //     from: Date.UTC(2006, 0, 1),
            //     to: Date.UTC(2008, 7, 1),
            //     color: 'rgba(0, 0, 0, 0.2)'
            // }],
            labels: {
                rotation: -45,
                align: 'right',
                style: {
                    fontSize: '10px',
                    fontFamily: 'Verdana, sans-serif'
                }
            },
            title: {
                text: null
            }
        },
        yAxis: {
            labels: {
                formatter: function() {
                    return this.value + unit;
                },
                style: {
                    color: '#89A54E'
                }
            },
            title: {
                text: name,
                style: {
                    color: '#89A54E'
                }
            }
        },
        tooltip: {
            formatter: function() {
                return '<b>'+ this.series.name +'</b><br/>'+
                    Highcharts.dateFormat('%Y-%m-%d %H:%M:%S', this.x) +'<br/>'+
                    Highcharts.numberFormat(this.y, 2);
            }
        },
        legend: {
            enabled: true
        },
        exporting: {
            enabled: true
        },
        series: [{
            name: name,
            data: (function() {
                var data = [],
                json_obj = {
                    "net_name": networkName,
                    "nodeId": nodeId,
                    "sense": senseType,
                    "requestNum": "30"
                },
                json_text = JSON.stringify(json_obj, null, 2);

                jQuery.ajax({
                    type: "POST",
                    url: "modules/realTime.wsn",
                    data: json_text,
                    async: false
                }).success(function(msg) {
                    for( var i in msg ) {
                        msg[i]["time"] = parseInt(msg[i]["time"]+"000");
                        node.addSense(msg[i]);
                    }
                    var sense = node.sense;
                    for (var i = 29, j = -29; i >= 0; i--, j++) {
                        if(sense[i]["value"] !== undefined || sense[i]["value"] !== null) {
                            // var time = parseInt(sense[i]["time"]+'000');
                            data.push({
                                x: sense[i]["time"],
                                y: sense[i]["value"]
                            });
                        }
                    }
                }).fail(function() {
		    window.location = "404.htm";
		});
                return data;


                // var data = [],
                // time = new Date().getTime();
                // for(var i = -40; i <= 0; i++) {
                //     data.push({
                //         x: time + i * 1000,
                //         y: 0
                //     })
                // }
                // return data;
                // captureTime.length = 0; // array set 0 because of i will use it in event:load
            })()
        }]
    });
};

/**
 * Class: Network
 *
 **/
var Network = function() {

    //the basic information of the network
    this.networkId;
    this.networkName;

    this.nodeMuster = []; //to storage the node

    //draw node need this "global" variable
    this.sigmaNodeName;
    this.sigInst;
};
Network.prototype.init = function(data) {
    this.networkId = data["networkId"];
    this.networkName = data["networkName"];
};
Network.prototype.addNode = function(data) {
    var node = new Node();
    if(this.exist(data["nodeId"])) {
        throw data["nodeId"] + " has exists!";
    }
    node.init(data);
    this.nodeMuster.push(node);
};
Network.prototype.index = function(index) {
    if(index > this.nodeMuster.length)
        return 0;
    else
        return this.nodeMuster[index - 1];
};
//one day i have to change this algorithm to improve
Network.prototype.exist = function(id) {
    for(var i in this.nodeMuster) {
        if(id === this.nodeMuster[i].nodeId)
            return true;
    }
    return false;
};
Network.prototype.findById = function(id) {
    // if(id > this.nodeMuster.length)
    //     return false;
    // alert(this.nodeMuster);
    for(var i in this.nodeMuster) {
        if(id === this.nodeMuster[i].nodeId)
            return this.nodeMuster[i];
    }
    return false;
};
Network.prototype.compare = function(network) {
    var Muster = {};
    // a = new JS.Set(this.nodeMuster),
    // b = new JS.Set(network);

    Muster.dropNodeMuster = [];
    Muster.addNodeMuster = [];

    


    return Muster;
    // if(a.contains(b)) {
    // 	dropNodeMuster = a.difference(b);
    // } else if(b.contains(a)) {
    // 	addNodeMuster = b;
    // } else {
    // 	dropNodeMuster = this.nodeMuster.complement(network);
    // 	addNodeMuster = network.co
    // }
};
Network.prototype.drawNode = function() {

    /**
     * draw the node distribute
     * nodeSlot is where the node information(id, position_x, position_y, energy) storage
     **/
    // drawNode = function(networkID, networkName, nodeSlot) {

    var loaderName = '#loader' + this.networkId,
    i, N = this.nodeMuster.length,
    nodeMuster = this.nodeMuster,
    networkId = this.networkId,
    networkName = this.networkName,

    // findById = this.findById;
    // console.log(findById(0));
    //sigmaNodeName is the name in div in index.html/view.js
    sigmaNodeName = '#sigma-node' + this.networkId;

    // Instanciate sigma.js and customize it :
    sigInst = sigma.init($(sigmaNodeName)[0]).drawingProperties({
        defaultLabelColor: '#000',
        // defaultLabelSize:
        // defaultLabelBGColor:
        // defaultLabelHoverColor:
        defaultEdgeType:'curve',
        labelThreshold: 6
    }).graphProperties({
        minNodeSize: 0.5,
        maxNodeSize: 20,
        minEdgeSize:4,
        maxEdgeSize:20
    }).mouseProperties({
        maxRatio:8
        // mouseEnable:
    });


    $(loaderName).addClass("hide");



    //when the cursor over the node, pop its information
    (function() {

        var popUp;
        ///this valible is use to record the id of node and 
        //And clickNode below will need it
        var node,
        nodeId,
        overNode = false;


        //Get the position in array by the id of the node
        function getPosition(nodeId, nodeMuster){
            for(var i in nodeMuster) {
                if(nodeMuster[i].nodeId == nodeId) {
                    break;
                }
            }
            return i;
        }

        //show node information
        function nodeInfoDisplay(node) {
            return  "X:" + node.position_x +"<br />"+
                "Y:" + node.position_y +"<br />"+
                showEnergy(node.energy);//the showEnergy is in energy.js
        }


        function showNodeInfo(event) {
            popUp && popUp.remove();

            // var node;
            sigInst.iterNodes(function(n) {
		// n.hidden = 1;
                node = n;
                nodeId = [event.content[0]];
                // alert("nodeID"+nodeID);
                overNode = true;
            }, [event.content[0]]).iterEdges(function(e) {
		// e.hidden = 1;
	    });

            popUp = $(
                '<div class="node-info-popup"></div>'
            ).append(
                nodeInfoDisplay(nodeMuster[getPosition(nodeId, nodeMuster)])
                // console.log(nodeMuster[getPosition(nodeId, nodeMuster)])
            ).attr(
                'id',
                'node-info'+sigInst.getID() //This error i find for a long time
            ).css({
                'display': 'inline-block',
                'border-radius': 3,
                'padding': 5,
                'background': '#fff',
                'color': '#000',
                'box-shadow': '0 0 4px #666',
                'position': 'absolute',
                'left': node.displayX,
                'top': node.displayY+15,
                'width': '155px'
            });
            // $('ul',popUp).css('margin','0 0 0 20px');
            $(sigmaNodeName).append(popUp);
        }

        function hideNodeInfo(event) {
            popUp && popUp.remove();
            popUp = false;
            overNode = false;
        }

        function myContentMenuShow(event) {
            popUp && popUp.remove();

            // var node;
            sigInst.iterNodes(function(n) {
                sigInstNode = n;
                nodeId = [event.content[0]];
                overNode = true;
            }, [event.content[0]]);

            popUp = $(
                '<div class="node-info-popup"></div>'
            ).append(
                //because of the array, so have to -1
                nodeInfoDisplay(nodeId, this.nodeMuster)
            ).attr(
                'id',
                'node-info'+sigInst.getID() //This error i find for a long time
                // 'sigma_mouse_'+sigInst.getID()
            ).css({
                'display': 'inline-block',
                'border-radius': 3,
                'padding': 5,
                'background': '#fff',
                'color': '#000',
                'box-shadow': '0 0 4px #666',
                'position': 'absolute',
                'left': sigInstNode.displayX,
                'top': sigInstNode.displayY+15,
                'width': '155px'
            });
            // $('ul',popUp).css('margin','0 0 0 20px');
            $(sigmaNodeName).append(popUp);
        }

        function myContentMenuHiden(event) {
            popUp && popUp.remove();
            popUp = false;
            overNode = false;
        }

        var self = this;
        sigInst.bind('overnodes', function() {
            showNodeInfo.apply(self, arguments);
        }).bind('outnodes', function() {
            hideNodeInfo.apply(self, arguments);
        });

        // document.getElementById("sigma-node"+networkID).addEventListener('dblclick', clickNode, false);
        $(sigmaNodeName).dblclick(dblClickNode);
        //Double Click the node
        function dblClickNode() {
	    // alert(nodeId);
            if(overNode && nodeId != 0) {

                // alert(getPosition(nodeId, nodeMuster));
                // eraseNetworkChart(networkId); //eraseNetworkChart is in view.js
                eraseSigmaById(networkId);
                drawNavigate({
                    "networkId": networkId,
                    "networkName": networkName, 
                    "node": nodeMuster[getPosition(nodeId, nodeMuster)]
                });// drawNavigate is in view.js

                overNode = false;// To judge the course whethre over node 
            }
        }
        
        // turn off the right click menu
        $(sigmaNodeName).bind("contextmenu", function() { return false; });
        // $("#sigma-node").mousedown(function(event) {
        //     if(3 === event.which) {
        //         alert("this is icecream");
        //     }
        // })

    })(this.nodeMuster);


    // sigInst.emptyGraph();
    // var node_array_id = [];
    // for(var i in this.nodeMuster) {
    //     node_array_id.push(this.nodeMuster[i].nodeId);
    // }
    // sigInst.dropNode(node_array_id);

    //draw every node
    for(i = 0; i < N; i++){
        var node = this.nodeMuster[i],
        energy = node.energy,
        size = 14,//control the size of the node
        color = 0;//control the color of the node

        if(node.nodeId === 0)
            size = 26;
        else
            size = 17;

        if(energy>60) {
            color = 'rgb(0, 120, 0)';
        } else if (energy>10) {
            color = 'rgb(255, 255, 0)';
        } else if (energy>=0) {
            color = 'rgb(120, 0, 0)';
        }
        
        sigInst.addNode(node.nodeId,{
            'x': node.position_x,
            'y': node.position_y,
            'label': 'Node '+ node.nodeId,
            'size': size,
            'color': color
        });
    }


    for(i = 0; i < N; i++) {
        var node = this.nodeMuster[i];
        if(node.nodeId) {
            sigInst.addEdge(
                i,
                node.nodeId,
                node.parentId, 
                {
                    // 'color': 'rgb(255, 255, 255)'
                    // 'size': 10
                }
            );
        }
    }
    
    // sigInst.iterEdges(function(e) {
    // 	e.hidden = 0;
    // }).iterNodes(function(n) {
    // 	n.hidden = 0;
    // }).draw(2,2,2);
    // sigInst.dropNode(0);
    // sigInst.dropNode(1);
    // sigInst.dropNode(5);
    // sigInst.dropNode(6);



    // var node_array_id = [];
    // for(var i in this.nodeMuster) {
    //     node_array_id.push(this.nodeMuster[i].nodeId);
    // }
    // sigInst.dropNode(node_array_id);
    sigInst.draw();
    sigInst.dropNode(1);
    var timer1 = setInterval(function() {
	// alert("1");
	sigInst.draw();

	// sigInst.dropNode(1).draw();
	clearInterval(timer1);

	(function(millis) {
	    var date = new Date();
	    var curDate = null;
	    do {
		curDate = new Date();
	    }
	    while(curDate - date < millis) ;
	})(3000);

	sigInst.dropNode(2);
	var timer2 = setInterval(function() {
	    sigInst.draw();

	    // sigInst.dropNode(1).draw();
	    clearInterval(timer2);
	}, 5000);

    }, 5000);
    

    // console.log(sigInst);
    // console.log(sigma);
    // sigInst.draw();


    // sigInst.emptyGraph();
    // sigInst.emptyGraph();
    // sigInst.refresh();
    // sigInst.kill();
    

    // $.ajax({
    //     type: "POST",
    //     url: "modules/nodeInfo.wsn",
    // 	async: false
    // }).done(function(data) {
    //     var networkMuster = [];
    //     for(var i in data) {
    //         var network = new Network,
    //         nodes = data[i]["nodes"];
    //         network.init({"networkId": data[i]["network_id"], "networkName": data[i]["network_name"]});
    //         for(var j in nodes) {
    //             network.addNode({
    //                 "nodeId": nodes[j]["node_id"],
    //                 "parentId": nodes[j]["parent_id"],
    //                 "position_x": nodes[j]["position_x"],
    //                 "position_y": nodes[j]["position_y"],
    //                 "energy": nodes[j]["power"]
    //             })
    //         }
    //         networkMuster.push(network);
    //     }
    // })

};
//delete/erase from the canvas by id
Network.prototype.eraseNodeById = function(id) {
    sigInst.dropNode(id);
};
//delete/erase from the canvas all
Network.prototype.eraseAllNode = function() {
    // if(nodeSlot === undefined || nodeSlot === null)
    //     return 0;
    var node_array_id = [];

    for(var i in this.nodeMuster) {
        node_array_id.push(this.nodeMuster[i].nodeId);
    }
    sigInst.dropNode(node_array_id);
};


