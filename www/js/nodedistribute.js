function drawNode(nodeSlot) {
    // Instanciate sigma.js and customize it :
    // var sigInst = sigma.init(document.getElementById('sigma-example')).drawingProperties({
    alert(nodeSlot);
    var sigInst = sigma.init($('#sigma-node')[0]).drawingProperties({
        defaultLabelColor: '#fff'
    }).graphProperties({
        minNodeSize: 0.5,
        maxNodeSize: 50,
        minEdgeSize:4,
        maxEdgeSize:24
        }).mouseProperties({
        maxRatio:4
        });

    // Generate a random graph with :
    //   . N nodes
    //   . E edges
    // var i, N = 500, E = 3000;
    // var i, N = nodeSlot.length;
    var i, N = 4;
    for(i = 1; i < N; i++){
            sigInst.addNode(nodeSlot[i].id,{
            'x': nodeSlot[i].coordinate.x,
            'y': nodeSlot[i].coordinate.y,
            'label': 'Node '+nodeSlot[i].id,
            'size': 29,
            // 'color': 'rgb('+Math.round(Math.random()*256)+','+
            //     Math.round(Math.random()*256)+','+
            //     Math.round(Math.random()*256)+')'
            'color':'rgb(0, 120, 0)'
        });
    }
    // for(i = 0; i < E; i++){
    //     sigInst.addEdge(i,'n'+(Math.random()*N|0),'n'+(Math.random()*N|0));
    // }
    // for(i = 0; i < N; i++){
    //     sigInst.addEdge(i,nodeSlot[i].id,nodeSlot[i].parentId);
        sigInst.addEdge(1,1,2);
        sigInst.addEdge(2,2,3);
    // }

    //when the cursor over the node, pop its information
    (function() {
    // sigma.publicPrototype.myPop = function() {
        //Get the ID of the Node and insert into html
        function nodeInfoDisplay(node) {
            // return  "X:" + nodeSlot[i].coordinate.x +"<br />"+
            //         "Y:" + nodeSlot[i].coordinate.y +"<br />"+
            return  "X:" + node.coordinate.x +"<br />"+
                    "Y:" + node.coordinate.y +"<br />"+
                    showEnergy(69);
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
                overNode = true;
            }, [event.content[0]]);

            popUp = $(
                '<div class="node-info-popup"></div>'
            ).append(
                nodeInfoDisplay(nodeSlot[nodeID])
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
            $('#sigma-node').append(popUp);
        }

        function hideNodeInfo(event) {
            popUp && popUp.remove();
            popUp = false;
            overNode = false;
        }

        //Click the node
        function clickNode() {
            if(overNode) {
                window.location = "./nodeInformation.htm";
                overNode = false;
            }
        }

        var self = this;
        sigInst.bind('overnodes', function() {
            showNodeInfo.apply(self, arguments);
        }).bind('outnodes', function() {
            hideNodeInfo.apply(self, arguments);
        }).draw();
        document.getElementById("sigma-node").addEventListener('dblclick', clickNode, false);
        
    })(nodeSlot);

    // for(i = 1; i < N; i++){
    //     // sigInst.addEdge(i,nodeSlot[i].id,nodeSlot[i].parentId);
    //     sigInst.addEdge(i,1,2);
    // }
}
