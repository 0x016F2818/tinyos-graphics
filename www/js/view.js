// var view = function() {
//     var temperature = $('<li class="active"><a href="./temperature.htm?networkID=' + networkID + '&&networkName=' + networkName + '&&nodeID=' + nodeID + '">temperature</a></li>');
//     $("#navigate").append(temperature);

//     var light = $('<li><a href="./light.htm?networkID=' + networkID + '&&networkName=' + networkName + '&&nodeID=' + nodeID +'">light</a></li>');
//     $("#navigate").append(light);

//     var sound = $('<li><a href="./sound.htm?networkID=' + networkID + '&&networkName=' + networkName + '&&nodeID=' + nodeID +'">sound</a></li>');
//     $("#navigate").append(sound);

//     // var acceleration = $('<li><a href="./acceleration.htm?networkID=' + networkID + '&&networkName=' + networkName + '&&nodeID=' + nodeID +'">acceleration</a></li>');
//     // $("#navigate").append(acceleration);

//     // var geomagnetic = $('<li><a href="./geomagnetic.htm?networkID=' + networkID + '&&networkName=' + networkName + '&&nodeID=' + nodeID +'">geomagnetic</a></li>');
//     // $("#navigate").append(geomagnetic);
// }

var drawNavigate = function(data) {
    window.timer;
    var node = data["node"];

    $("#navigate").removeClass("hide");
    $("#networkId").html("Network ID: " + data["networkId"]);
    $("#networkId").wrap("<h4 />");
    $("#nodeId").html("Node ID: " + node.nodeId);
    $("#nodeId").wrap("<h4 />");

    //init here
    $("#titleOfChart").html("<h4>Real Time: temperature</h4>");
    // alert(node.nodeId);
    node.drawDynamicSpline({
        "networkName": data["networkName"],
        "sense": "temp",
    });

    //listen Navigate
    (function() {
        // var self = this;
        $("#temp").addClass("active");
        // createChart();

        var clicky = $('#navigate li').not(".nav-header, .nodeinfo, #networkId, #nodeId").css({
            "cursor": "pointer"
        });
        clicky.click(function(event) {
            var clickName = $(this).attr("id");
            if(clickName === "light") 
                clickName = "photo";
            var title = "<h4>Real Time: " + clickName + "</h4>";
            $("#titleOfChart").html(title);
            $("#navigate li").not(".nav-header, .nodeinfo").removeClass("active");
            // $('#'+clickName).addClass("active");
            $(this).addClass("active")
            clearInterval(timer);
            node.drawDynamicSpline({
                "networkName": data["networkName"],
                "sense": clickName,
                "clock": clock
            });
        });
    })();
}



//Network Chart
var drawNetworkChart = function(networkId, networkName) {

    if(networkId === 1) {

	var title = '<h3>Network ID:' + networkId + '  ' + 'NetworkName:' +networkName + '</h3>';
	$("#titleOfChart").html(title);
	// var html = createSigma(networkId, title);
	// alert($("#displayArea").children());
	var body = $('<div id="sigma-node' + networkId +'" class="sigma-parent"></div>');
	// createChart(id, title, body);
	$("#bodyOfChart").append(body);

    } else {

	var html = $('<div id="sigma"><div class="accordion" id="accordionChart1"><div class="accordion-group"> <div class="accordion-heading"><a style="text-align: left;text-decoration: none" class="accordion-toggle btn collapsed" data-toggle="collapse" data-parent="#accordion2" href="#networkChart'+networkId+'"><h1><em id="nodeID">Network ID:'+networkId+'    NetworkName:'+networkName+'</em></h1></a> </div> <div id="networkChart'+networkId+'" class="accordion-body collapse" style="height: 0px; "> <div class="accordion-inner">  <!-- Here is the node distribute --> <div id="sigma-node'+networkId+'" class="sigma-parent"></div> </div>     <!-- Here is control slider --> <input type="text" id="cpatureFrequency" style="border:0; color:#30F628; font-weight:bold" /><div id="controlSlider" class="ui-slier ui-slider-horizontal ui-widget ui-widget-content ui-corner-all" ></div> <script src="./lib/jquery-ui-1.8.22.js"></script> <link rel="stylesheet" href="./css/jquery-ui-1.8.22.css" type="text/css"/>  </div> </div></div> </div><!-- networkChart'+networkId+' end --> ');

	$("#displayArea").append(html);

    }

    
}

var eraseSigmaById = function(networkId) {
    $("#sigma-node"+networkId).remove();
}

var eraseNetworkChart = function(id) {
    if(id === undefined || id === null)
        return false;
    // $("#sigma").remove();
    // $("#accordionChart"+id).remove(); 
    eraseChartById(id);
}

var drawRealTimeChart = function() {
}

var createSigma = function(id, title) {
    // var body = $('<div id="Chart' + id + '" class="accordion-body collapse in"><div class="accordion-inner"><div id="sigma-node' + id +'" class="sigma-parent"></div></div></div>');
    var body = $('div id="sigma-node' + id +'" class="sigma-parent"></div>');
    // createChart(id, title, body);
    $("#bodyChart").append(body);
}

var createChart = function(id, title, body) {
    if(title === undefined || title === null)
        var title = "Test";
    var type = $('<div id=sigma></div>');
    var chart = $('<div class="accordion" id="accordionChart' + id + '"></div>');
    var group = $('<div class="accordion-group"></div>');
    var heading = $('<div class="accordion-heading"><a style="text-align: left;text-decoration: none" class="accordion-toggle btn" data-toggle="collapse" data-parent="#accordion2" href="#Chart' + id + '"><i class="icon-bar-chart"></i>' + title + '<i class="icon-minus pull-right"></i></a></div>');
    // var body = $('<div id="Chart' + id + '" class="accordion-body collapse in"><div class="accordion-inner"></div></div>');
    var body = $('<div id="Chart' + id + '" class="accordion-body collapse in"><div class="accordion-inner"><div id="sigma-node' + id +'" class="sigma-parent"></div></div></div>');

    // var context = $('<div id="sigma-node' + id +'" class="sigma-parent"></div>');
    // body = context.wrap(body);
    var temp = group.append(heading).append(body);
    chart.append(temp).appendTo("#displayArea");
}
var eraseChartById = function(id) {
    $('#accordionChart'+id).remove();
}