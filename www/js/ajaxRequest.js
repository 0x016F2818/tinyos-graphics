var createxmlhttp = function() {
    var xmlhttp, 
    alerted; 
    /*@cc_on 
      @if (@_jscript_version >= 5)
      try {
          xmlhttp = new ActiveXObject("Msxml2.XMLHTTP")
      } catch (e) {
          try {
              xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
          } catch (E) {
              alert ("You browser is ???");
          } 
      }
      @else
          alert("Your javascript is ???");
      xmlhttp = false;
      alerted = true;
      @end @*/
    if (!xmlhttp && !alerted) {
        try {
            xmlhttp = new XMLHttpRequest();
        } catch (e) {
            alert ("Your browser is not support XMLHttpRequest");
        }
    }
    return xmlhttp;
},
go = function(xmlhttp, requestType, module, bool, request) {
    if(!requestType)
        requestType = "GET";
    if(!module)
        module = "";
    if(!request)
        request = null;
    if (xmlhttp) {
        if(bool === 'false')               // bool is false do it
            xmlhttp.open(requestType, module, false);
        else                    // if bool not false do it
            xmlhttp.open(requestType, module, true);
        xmlhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");  
        // console.log(request);
        // alert(request);
        xmlhttp.send(request); 
        // alert(xmlhttp);
        xmlhttp.onreadystatechange = RSchange(xmlhttp);
    }
},
RSchange = function (xmlhttp) {
    // alert(xmlhttp.readyState);
    if (xmlhttp.readyState === 4 && xmlhttp.status === 200) {
        // var type = xmlhttp.getResponseHeader("Content-Type");
        // alert(type);
        var jsondata = xmlhttp.responseText;
        // because of now only use json, so...
        // if(jsondata !== undefined && type === "application/json") {
        if(jsondata !== undefined) {
            var data = JSON.parse(xmlhttp.responseText);
            // alert(data);
            dataHandle(data);
        } 
    }
}