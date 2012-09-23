/**
* nodeSlot is to store the ndoe
*/
var nodeSlot=new Array();


function dispatch(data) {
    // var key1 = 'n'+data["nodeId"]+'x';
    // if(!($.jStorage.get(key1))) {
    //     var key2 = 'n'+data["nodeId"]+'y';
    //     $.jStorage.set(key1, data["position_x"]);
    //     $.jStorage.set(key2, data["position_y"]);
    // }
    // alert(data);
    // var i;
    // for(i = 0; i < nodeSlot.length; i++) {
    //     if(nodeSlot[i].id == undefined) {
    //         var n = new node(data["nodeId"]);
    //         nodeSlot[i] = n;
    //         nodeSlot[i].parentId = data["parentId"];
    //         nodeSlot[i].coordinate = new coordinate(data["position_x"], data["position_y"]);
    //         drawNode(nodeSlot);
    //     }
    // }

    if(nodeSlot[data["nodeId"]] == undefined) { 
        var n = new node(data["nodeId"]);
        nodeSlot[data["nodeId"]] = n;
        nodeSlot[data["nodeId"]].parentId = data["parentId"];
        nodeSlot[data["nodeId"]].coordinate = new coordinate(data["position_x"], data["position_y"]);
        drawNode(nodeSlot);
    }
    nodeSlot[data["nodeId"]].addTemp(data["temp"]);
    nodeSlot[data["nodeId"]].addPhoto(data["photo"]);
    nodeSlot[data["nodeId"]].addSound(data["sound"]);
    drawSpline('temperature-spline', nodeSlot[2]);
}


// function dispatch(data) {
//     if(nodeSlot[data["nodeId"]] == undefined) { 
//         var n = new node(data["nodeId"]);
//         nodeSlot[data["nodeId"]] = n;
//         nodeSlot[data["nodeId"]].parentId = data["parentId"];
//         nodeSlot[data["nodeId"]].coordinate = new coordinate(data["position_x"], data["position_y"]);
//         drawNode(nodeSlot);
//     }
//     nodeSlot[data["nodeId"]].addTemp(data["temp"]);
//     nodeSlot[data["nodeId"]].addPhoto(data["photo"]);
//     nodeSlot[data["nodeId"]].addSound(data["sound"]);
//     drawSpline('temperature-spline', nodeSlot[2]);
// }

//****************coordinate**************************//
// var nodeCoorinate = {}

function coordinate(x, y) {
    this.x = x;
    this.y = y;
}

coordinate.prototype.distance = function(coordinate) {
    dltx = Math.abs(this.x - coordinate.x);
    dlty = Math.abs(this.y - coordinate.y)
    return Math.sqrt(dltx*dltx + dlty*dlty);
}


//****************node**************************//
function node() {
    this.id = arguments[0];
    this.parentId;
    this.coordinate = {};
    this.temp = [];
    this.photo = [];
    this.sound = [];
}
node.prototype.addTemp = function() {
    this.temp.push(arguments[0]);
    return this.temp;
}
node.prototype.addPhoto = function() {
    this.photo.push(arguments[0]);
    return this.photo;
}
node.prototype.addSound = function() {
    this.sound.push(arguments[0]);
    return this.sound;
}
node.prototype.getID = function() {
    return this.id;
}
node.prototype.getCoordinate = function() {
    return this.Coordinage;
}
node.prototype.getTemp = function(id) {
    if (id)
        return this.temp[id];
    else
        return;
}
node.prototype.getSound = function(id) {
    if (id)
        return this.Sound[id];
    else
        return ;
}
node.prototype.getPhoto = function(id) {
    if (id)
        return this.Photo[id];
    else
        return;
}


//****************Dispathc**************************//
// function dispatch(data) {
//     if(nodeSlot[data["nodeId"]] == undefined) { 
//         var n = new node(data["nodeId"]);
//         nodeSlot[data["nodeId"]] = n;
//         nodeSlot[data["nodeId"]].parentId = data["parentId"];
//         nodeSlot[data["nodeId"]].coordinate = new coordinate(data["position_x"], data["position_y"]);
//     }
//     nodeSlot[data["nodeId"]].addTemp(data["temp"]);
//     nodeSlot[data["nodeId"]].addPhoto(data["photo"]);
//     nodeSlot[data["nodeId"]].addSound(data["sound"]);
//     // drawNode(nodeSlot);
//     // alert(nodeSlot[data["nodeId"]].id + " "
//           // nodeSlot[data["nodeId"]].coordinate.x + " "
//           // nodeSlot[data["nodeId"]].coordinate.y + " "
//           // nodeSlot[data["nodeId"]].temp[0] + " "
//           // nodeSlot[data["nodeId"]].photo[0] + " "
//           // nodeSlot[data["nodeId"]].sound[0]
//           // )
// }


// function dispatch(data) {
//     switch(data["nodeId"]) {
//     case 1: 
//         if(nodeSlot[0] == undefined) { 
//             var n = new node(data["nodeId"]);
//             nodeSlot[0] = n;
//             nodeSlot[0].coordinate = new coordinate(data["position_x"], data["position_y"]);
//         }
//         nodeSlot[0].addTemp(data["temp"]);
//         nodeSlot[0].addPhoto(data["photo"]);
//         nodeSlot[0].addSound(data["sound"]);
//         break;
//     case 2: 
//         if(nodeSlot[1] == undefined) { 
//             var n = new node(data["nodeId"]);
//             nodeSlot[1] = n;
//             nodeSlot[1].coordinate = new coordinate(data["position_x"], data["position_y"]);
//         }
//         nodeSlot[1].addTemp(data["temp"]);
//         nodeSlot[1].addPhoto(data["photo"]);
//         nodeSlot[1].addSound(data["sound"]);
//         break;
//     case 3: 
//         if(nodeSlot[2] == undefined) { 
//             var n = new node(data["nodeId"]);
//             nodeSlot[2] = n;
//             nodeSlot[2].coordinate = new coordinate(data["position_x"], data["position_y"]);
//         }
//         nodeSlot[2].addTemp(data["temp"]);
//         nodeSlot[2].addPhoto(data["photo"]);
//         nodeSlot[2].addSound(data["sound"]);
//         break;
//     }
// }
//Test
// data={"time":"20020202","nodeId":3,"position_x":300,"position_y":450,"temp":2,"photo":80,"sound":90}
// data["temp"]
// dispatch(data);
// nodeSlot[2].id
// nodeSlot[2].coordinate.x
// nodeSlot[2].temp[0]
// $(fu)
// alert(nodeSlot[2]);
