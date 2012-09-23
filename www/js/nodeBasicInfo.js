var i = 0, 
j = 0,
_data,
node_object = function(){
    this.nodeId;
    this.parentId;
    this.position_x;
    this.position_y;
    this.energy;
},
network_object = function() {
    this.networkId;
    this.NetworkName;
    this.nodeSlot = [];
},
dataHandle = function(data) {
    _data = data;
},
getNodeBasicInformation = function(networkSlot) {

    // alert(_data.length);
    for( i = 0; i < _data.length; i++ ) {
        var network = new network_object();
        network.networkId = _data[i]["network_id"];
        network.networkName = _data[i]["network_name"];
        for( j = 0; j < _data[i]["nodes"].length; j++ ) {
            node = new node_object();
            node.nodeId = _data[i]["nodes"][j]["node_id"];
            node.parentId = _data[i]["nodes"][j]["parent_id"];
            node.position_x = _data[i]["nodes"][j]["position_x"];
            node.position_y = _data[i]["nodes"][j]["position_y"];
            node.energy = _data[i]["nodes"][j]["power"];
            network.nodeSlot.push(node);
        }
        networkSlot.push(network);
    }
}
