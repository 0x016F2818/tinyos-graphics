//$(function() {
		//setInterval(go, 300);
//});

var xmlhttp, alerted, JSONalerted=false, readyStatealerted=false;
/*@cc_on*/
/*@if (@_jscript_version >= 5)
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

function RSchange() {
    if (xmlhttp.readyState == 4) {
        var jsondata = xmlhttp.responseText;
        if(jsondata !== undefined) {
            var data = JSON.parse(xmlhttp.responseText);
            //Function dispatch define in data.js
            dispatch(data);
        }
        // else
        //     if(!JSONalerted) {
        //         alert("Sorry. There is no data. Maybe that server is donw now.");
        //         JSONalerted = true;
        //     }
    } 
    // else {
    //     if(!readyStatealerted) {
    //         alert("Sorry, maybe our server is shutdown now!");
    //         // $("#myAlert").removeClass("hide");
    //         readyStatealerted = true;
    //     }
    // }
}
function go() {
     if (xmlhttp) {
        xmlhttp.open("POST", "", false);
        xmlhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");  
        xmlhttp.send(null); 
        xmlhttp.onreadystatechange = RSchange;
    }
}
// document.addEventLisnter("click", go, false); 

