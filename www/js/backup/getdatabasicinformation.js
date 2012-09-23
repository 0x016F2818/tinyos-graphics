/**
 * nodeSlot is to store the ndoe
 */
var nodeCount = 4,

i = 0,

node_object = {};

isDraw = false,

isDrawSpline = false,

nodeSlot=new Array(),

getNodeBasicInformation = function(data) {
    
    for( i = 0; i < data.length; i++ ) {
        node_object.nodeId     = data["nodeId"];
        node_object.parentId   = data["parentId"];
        node_object.position_x = data["position_x"];
        node_object.position_y = data["position_y"];
        node_object.energy     = data["energy"];
        nodeSlot.push(node_object);
    }

}
//     var i = 0, isExist = false;

//     for( i = 0; i < nodeSlot.length; i++ ) {
//         if(nodeSlot[i].nodeId == data["nodeId"]) {
//             isExist = true;
//         }
//     }

//     //Judge it is in the list(nodeSlot)
//     if(isExist) {
//         //because when leave for loop, the i is +1 again, so
//         //I have to -1
//         nodeSlot[i-1].addTemp(data["temp"]);
//         nodeSlot[i-1].addPhoto(data["photo"]);
//         nodeSlot[i-1].addSound(data["sound"]);
//     } else {
//         var n = new node();
//         n.nodeId = data["nodeId"];
//         n.parentId = data["parentId"];
//         n.energy = 44;
//         n.coordinate = new coordinate(data["position_x"], data["position_y"]);

//         // this will remove a place where want these data. For example: temperatuer.html
//         // n.addTemp(data["temp"]);
//         // n.addPhoto(data["photo"]);
//         // n.addSound(data["sound"]);

//         nodeSlot.push(n);
//     }
//     if(nodeSlot.length === nodeCount && !isDraw) {
//         drawNode(nodeSlot);
//         isDraw = true;
//     }
//     // if(nodeSlot[1] && nodeSlot[1].temp && nodeSlot[1].temp.length == 50 && !isDrawSpline) {
//     //     drawSpline("temp", nodeSlot[1].temp);
//     //     isDraw = true;
//     // }
// }

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
