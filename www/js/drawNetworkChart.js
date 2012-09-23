var drawNetworkChart = function(networkId, networkName) {

    var html = $('<!-- This is a loading.... --> <img id="loader'+networkId+'"src="./imgs/loader.gif" class="loader" alt="loading..." style=""/> <div class="accordion" id="accordionChart1"><div class="accordion-group"> <div class="accordion-heading"><a style="text-align: left;text-decoration: none" class="accordion-toggle btn" data-toggle="collapse" data-parent="#accordion2" href="#networkChart'+networkId+'"><i class="icon-bar-chart"></i><h1><em id="nodeID">Network ID:'+networkId+'    NetworkName:'+networkName+'</em></h1> <i class="icon-minus pull-right"></i> </a> </div> <div id="networkChart'+networkId+'" class="accordion-body in collapse" style="height: auto; "> <div class="accordion-inner">  <!-- Here is the node distribute --> <div id="sigma-node'+networkId+'" class="sigma-parent"></div> </div> </div> </div> </div> <!-- networkChart'+networkId+' end --> ');

    $("#network").append(html);
}
