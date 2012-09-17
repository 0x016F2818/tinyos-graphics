function init() {
    // Instanciate sigma.js and customize it :
    // var sigInst = sigma.init(document.getElementById('sigma-example')).drawingProperties({
    var sigInst = sigma.init($('#sigma-node')[0]).drawingProperties({
        defaultLabelColor: '#fff'
    }).graphProperties({
        minNodeSize: 0.5,
        maxNodeSize: 5
    });

    // Generate a random graph with :
    //   . N nodes
    //   . E edges
    var i, N = 500, E = 3000;

    for(i = 0; i < N; i++){
        sigInst.addNode('n'+i,{
            'x': Math.random(),
            'y': Math.random(),
            'label': 'Node '+i,
            'size': 0.5+4.5*Math.random(),
            'color': 'rgb('+Math.round(Math.random()*256)+','+
                Math.round(Math.random()*256)+','+
                Math.round(Math.random()*256)+')'
        });
    }

    for(i = 0; i < E; i++){
        sigInst.addEdge(i,'n'+(Math.random()*N|0),'n'+(Math.random()*N|0));
    }

    //when the cursor over the node, pop its information
    (function() {
        
        //Get the ID of the Node and insert into html
        function nodeInfoDisplay(node) {
            return  "X:" + node.displayX +"<br />"+
                    "Y:" + node.displayY +"<br />"+
                    showEnergy(69);
        }

        var popUp

        ///this valible is use to record the id of node and 
        //And clickNode below will need it
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
                nodeInfoDisplay(node)
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
                'top': node.displayY+15 
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
                // $("#nodeInfo").modal('toggle');
                window.location = "./nodeInformation.htm";
            }
        }

        var self = this;
        sigInst.bind('overnodes', function() {
            showNodeInfo.apply(self, arguments);
        }).bind('outnodes', function() {
            hideNodeInfo.apply(self, arguments);
        }).draw();
        document.getElementById("sigma-node").addEventListener('dblclick', clickNode, false);
 
    })();


    // Finally, let's bind our methods to some buttons:
    // document.getElementById('circular').addEventListener('click',function(){
    //     sigInst.myCircularLayout();
    // },true);
    // document.getElementById('random').addEventListener('click',function(){
    //     sigInst.myRandomLayout();
    // },true);

}

if (document.addEventListener) {
    document.addEventListener('DOMContentLoaded', init, false);
} else {
    window.onload = init;
}
