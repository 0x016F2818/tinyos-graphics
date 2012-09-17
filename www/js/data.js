/**
* nodeSlot is to store the ndoe
*/
var nodeSlot=new Array();
var nodeCount = 3;
var isDraw = false;

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
    this.nodeId;
    this.parentId;
    this.energy;
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
