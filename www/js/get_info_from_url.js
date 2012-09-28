var cookie = document.cookie;
var info = {},
key, value, KeyAndValue, i, 
data = location.search.substr(1).split('&&');
for(i = 0; i < data.length; i++) {
    KeyAndValue = data[i].split('=');
    key = KeyAndValue[0];
    value = KeyAndValue[1];
    info[key] = value;
}
var networkID = info["networkID"];
if(isNaN(networkID)) {
    delete(networkID);
    window.location = "404.htm"

}
var nodeID = info["nodeID"];
var nodeNum = localStorage.getItem("nodeNum");
if(isNaN(nodeID)) {
    delete(nodeID);
    window.location = "404.htm";
} else if(nodeID > nodeNum) {
    delete(nodeID);
    window.location = "404.htm";
}
var networkName = info["networkName"],
temp = networkName.split(":"),i,
ip = temp[0], port = temp[1],
ipadd = ip.split(".");
if(isNaN(port)) {
    delete(nodeID);
    window.location = "404.htm";
}
for(i=0; i<ipadd.length; i++) {
    if(isNaN(ipadd[i])) {
        delete(networkName);
        window.location = "404.htm";
    }
}
// alert("Hello, icecream")
