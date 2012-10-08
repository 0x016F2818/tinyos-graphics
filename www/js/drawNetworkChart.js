var html,
drawNetworkChart = function(networkId, networkName) {

    html = $('<div class="accordion" id="accordionChart1"><div class="accordion-group"> <div class="accordion-heading"><a style="text-align: left;text-decoration: none" class="accordion-toggle btn collapsed" data-toggle="collapse" data-parent="#accordion2" href="#networkChart'+networkId+'"><i class="icon-bar-chart"></i><h1><em id="nodeID">Network ID:'+networkId+'    NetworkName:'+networkName+'</em></h1> <i class="icon-minus pull-right"></i> </a> </div> <div id="networkChart'+networkId+'" class="accordion-body collapse" style="height: 0px; "> <div class="accordion-inner">  <!-- Here is the node distribute --> <div id="sigma-node'+networkId+'" class="sigma-parent"></div> </div>     <!-- Here is control slider --> <input type="text" id="cpatureFrequency" style="border:0; color:#30F628; font-weight:bold" /><div id="controlSlider" class="ui-slier ui-slider-horizontal ui-widget ui-widget-content ui-corner-all" ></div> <script src="./lib/jquery-ui-1.8.22.js"></script> <link rel="stylesheet" href="./css/jquery-ui-1.8.22.css" type="text/css"/>  </div> </div></div> <!-- networkChart'+networkId+' end --> ');

    // var html = $('<!-- This is a loading.... --> <img id="loader'+networkId+'"src="./imgs/loader.gif" class="loader" alt="loading..." style=""/> <div class="accordion" id="accordionChart1"><div class="accordion-group"> <div class="accordion-heading"><a style="text-align: left;text-decoration: none" class="accordion-toggle btn collapsed" data-toggle="collapse" data-parent="#accordion2" href="#networkChart'+networkId+'"><i class="icon-bar-chart"></i><h1><em id="nodeID">Network ID:'+networkId+'    NetworkName:'+networkName+'</em></h1> <i class="icon-minus pull-right"></i> </a> </div> <div id="networkChart'+networkId+'" class="accordion-body collapse" style="height: 0px; "> <div class="accordion-inner">  <!-- Here is the node distribute --> <div id="sigma-node'+networkId+'" class="sigma-parent"></div> </div>     <!-- Here is control slider --> <input type="text" id="cpatureFrequency" style="border:0; color:#30F628; font-weight:bold" /><div id="controlSlider" class="ui-slier ui-slider-horizontal ui-widget ui-widget-content ui-corner-all" ></div> <script src="./lib/jquery-ui-1.8.22.js"></script> <link rel="stylesheet" href="./css/jquery-ui-1.8.22.css" type="text/css"/>  </div> </div></div> <!-- networkChart'+networkId+' end --> ');

    $("#network").append(html);
    // $("#controlSlider").slider({range: "min", value: 200, min:0, max:700, slider:function(event, ui){$("#captureFrequency").val(ui.value);}});
    // $("#captureFrequency").val($("#controlSlider").slider("value"));
},
deletNetworkChart = function() {
    $("#network").remove(html);
}
