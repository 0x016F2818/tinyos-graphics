// var isStarted = false;
/**
 * draw the node distribute
 * nodeSlot is where the node information(id, position_x, position_y, energy) storage
 **/
var drawNode = function(networkID, networkName, nodeSlot) {
    var rootSize = 15;
    var sigmaNodeName = '#sigma-node' + networkID;
    // Instanciate sigma.js and customize it :
    var sigInst = sigma.init($(sigmaNodeName)[0]).drawingProperties({
        defaultLabelColor: '#000',
        // defaultLabelSize:
        // defaultLabelBGColor:
        // defaultLabelHoverColor:
        defaultEdgeType:'curve',
        labelThreshold: 6
    }).graphProperties({
        minNodeSize: 0.5,
        maxNodeSize: 10,
        minEdgeSize:4,
        maxEdgeSize:10
    }).mouseProperties({
        maxRatio:8
        // mouseEnable:
    });

    $('#loader'+networkID).addClass("hide");
    var i, N = nodeSlot.length;

    for(i = 0; i < N; i++){
        sigInst.addNode(nodeSlot[i].nodeId,{
            'x': nodeSlot[i].position_x,
            'y': nodeSlot[i].position_y,
            'label': 'Node '+ nodeSlot[i].nodeId,
            'size': 14,
            // 'color': 'rgb('+Math.round(Math.random()*256)+','+
            //     Math.round(Math.random()*256)+','+
            //     Math.round(Math.random()*256)+')'
            'color':'rgb(0, 120, 0)'
        });
    }

    //when the cursor over the node, pop its information
    // sigma.publicPrototype.myPopandClick = function(nodeSlot) {
    (function() {
        //Get the position in array by the id of the node
        function getPosition(nodeID, nodeSlot){
            var i = 0;
            for(i=0; i<nodeSlot.length; i++) {
                if(nodeSlot[i].nodeId == nodeID)
                    break;
            }
            return i;
        }

        //show node information
        function nodeInfoDisplay(node) {
            return  "X:" + node.position_x +"<br />"+
                "Y:" + node.position_y +"<br />"+
                showEnergy(node.energy);
            // return  "X:" + nodeSlot[i].coordinate.x +"<br />"+
            //         "Y:" + nodeSlot[i].coordinate.y +"<br />"+
        }

        var popUp;
        ///this valible is use to record the id of node and 
        //And clickNode below will need it
        var node;
        var nodeID;
        var overNode = false;

        function showNodeInfo(event) {
            popUp && popUp.remove();

            // var node;
            sigInst.iterNodes(function(n) {
                node = n;
                nodeID = [event.content[0]];
                // alert("nodeID"+nodeID);
                overNode = true;
            }, [event.content[0]]);

            popUp = $(
                '<div class="node-info-popup"></div>'
            ).append(
                nodeInfoDisplay(nodeSlot[getPosition(nodeID, nodeSlot)])
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

        //Click the node
        function clickNode() {
            if(overNode) {
                // drawSpline("temp", nodeSlot[getPosition(nodeID, nodeSlot)]);
                // cookie
                if(navigator.cookieEnabled)
                    document.cookie = "requestNode" + "=" + encodeURIComponent(nodeID);

                // localStorage
                var name = "nodeNum";
                localStorage.setItem(name, nodeSlot.length);
                // window.location = "./nodeinfo.htm?networkID="+networkID+"&&networkName="+networkName+"&&nodeID="+nodeID;
                window.location = "./temperature.htm?networkID="+networkID+"&&networkName="+networkName+"&&nodeID="+nodeID;
                overNode = false;
            }
        }

        function myContentMenuShow(event) {
            popUp && popUp.remove();

            // var node;
            sigInst.iterNodes(function(n) {
                node = n;
                nodeID = [event.content[0]];
                overNode = true;
            }, [event.content[0]]);

            popUp = $(
                '<div class="node-info-popup"></div>'
            ).append(
                //because of the array, so have to -1
                nodeInfoDisplay(nodeID, nodeSlot)
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
                'left': node.displayX,
                'top': node.displayY+15,
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
        document.getElementById("sigma-node"+networkID).addEventListener('dblclick', clickNode, false);
        // $(sigmaNodeName).addEventListener('dblclick', clickNode, false);
        
        // turn off the right click menu
        $(sigmaNodeName).bind("contextmenu", function() { return false; });
        // $("#sigma-node").mousedown(function(event) {
        //     if(3 === event.which) {
        //         alert("this is icecream");
        //     }
        // })

    })(nodeSlot);

    for(i = 0; i < N; i++) {
        if(nodeSlot[i].nodeId) {
            sigInst.addEdge(i,nodeSlot[i].nodeId,nodeSlot[i].parentId);
        }
    }

    sigInst.draw();
}
