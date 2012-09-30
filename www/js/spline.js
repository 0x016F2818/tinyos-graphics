var captureTime = [], 
value = [],
i,
dataHandle = function(data) {
    for( i = 0; i < data.length; i++ ) {
        // captureTime.push(data[i]["time"].slice(-8));
        captureTime.push(data[i]["time"]+'000');
        value.push(data[i]["value"]);
    }
    // alert(value.length);
    // alert(captureTime[0]);
};

function drawDynamicSpline(networkName, nodeID, sense) {
    // alert(networkName+nodeID+sense);
    var firstFlag = true;
    // var value =  [0,0,0,0,0,0,0,121,11,11,11,11,1,1,113,141,15,116,117,20,1,23,4,3,2,1,5,4,3,44,3,2,1,3,3,4,54,4,3,3,2,3,,4,5,4,6,7,8,23,9,123,34,32,1,2,32,14,5,32,123,34,253,2,123,14,523,234,123,32,123,123,324,5,34,2342,34,213,123,23,52,25,132];
    var name, unit;
    switch(sense){
        case "temp":
        name = "Tmperature spline";
        unit = '°C';
        break;
        case "photo":
        name = "Light spline";
        unit = 'lux';
        break;
        case "sound":
        name = "Sound spline";
        unit = 'Hz';
        break;
        default:
        break;
    }

    if(name) {
        var drawContent = name.slice(0, 1);
        // alert(drawContent);
        // if ( drawContent == 'tempe' )
        //     drawContent = drawContent.slice(0, 4);
        Highcharts.setOptions({
            global: {
                useUTC: false
            }
        });

        var spline;
        spline = new Highcharts.Chart({
            chart: {
                renderTo: 'dynamicspline',//container is a vector
                // type: 'spline',
                // marginRight: 10,
                zoomType: 'x',
                events: {
                    load: function() {
                        // set up the updating of the chart each second
                        var series = this.series[0],
                        i = 0,
                        lastTime = parseInt(captureTime[0]);
                        json_obj = {
                            net_name: networkName,
                            nodeId: nodeID,
                            sense: sense,
                            requestNum: "1"
                        },
                        json_text = JSON.stringify(json_obj, null, 2),
                        xmlhttp = createxmlhttp();
                        
                        setInterval(function() {
                            var time = (new Date()).getTime(); // current time
                            if(!firstFlag) {
                                go(xmlhttp, "POST", "realTime.wsn", "false", json_text);
                            }
                            // here 30 is represent the start of the "second"
                            // (request 1 record)
                            if(value[30+i]!==undefined || value[30+i]!==null){ 
                                x = parseInt(captureTime[30+i]);
                                y = value[30+i];
                                i++;
                                if(x !== parseInt(captureTime[28+i])) // Make the draw stop!!!! Because of i has add 1(i++), so is 28+i and not 30+i-1(29+i)

                                    series.addPoint([x, y], true, true);
                            }
                        }, 1000);
                    }
                }
            },
            title: {
                text: name
            },
            xAxis: {
                type: 'datetime',
                tickPixelInterval: 44,
                // plotBands: [{
                //     id: 'mask-before',
                //     from: Date.UTC(2006, 0, 1),
                //     to: Date.UTC(2008, 7, 1),
                //     color: 'rgba(0, 0, 0, 0.2)'
                // }],
                labels: {
                    rotation: -45,
                    align: 'right',
                    style: {
                        fontSize: '10px',
                        fontFamily: 'Verdana, sans-serif'
                    }
                },
                title: {
                    text: null
                }
            },
            yAxis: {
                labels: {
                    formatter: function() {
                        return this.value + unit;
                    },
                    style: {
                        color: '#89A54E'
                    }
                },
                title: {
                    text: name,
                    style: {
                        color: '#89A54E'
                    }
                }
            },
            tooltip: {
                formatter: function() {
                    return '<b>'+ this.series.name +'</b><br/>'+
                        Highcharts.dateFormat('%Y-%m-%d %H:%M:%S', this.x) +'<br/>'+
                        Highcharts.numberFormat(this.y, 2);
                }
            },
            legend: {
                enabled: true
            },
            exporting: {
                enabled: true
            },
            series: [{
                name: name,
                data: (function() {
                    // generate an array of random data
                    var data = [],
                    json_obj = {
                        net_name: networkName,
                        nodeId: nodeID,
                        sense: sense,
                        requestNum: "30"
                    },
                    json_text = JSON.stringify(json_obj, null, 2),
                    xmlhttp = createxmlhttp();
                    go(xmlhttp, "POST", "realTime.wsn", "false", json_text);
                    firstFlag = false;
                    time = (new Date()).getTime(),
                    i, j = 0, k = 29; // k = 29 because of i request 30 records

                    for (i = -29; i <= 0; i++) {
                        if(value[k] !== undefined || value[k] !== null) {
                            data.push({
                                x: captureTime[k],
                                y: value[k]
                            });
                            k--;
                            j++;
                        }
                    }
                    // captureTime.length = 0; // array set 0 because of i will use it in event:load
                    return data;
                })()
            }]
        });
    }
}

function drawSymbolsSpline() {
    var temperature = [7.0, 6.9, 9.5, 14.5, 18.2, 21.5, 25.2] ,
    light = [23.3, 18.3, 13.9, 9.6, 14.2, 10.3, 6.6],
    sound = [4.2, 5.7, 8.5, 11.9, 15.2, 17.0, 16.6],
    chart;
    $(document).ready(function() {
        chart = new Highcharts.Chart({
            chart: {
                renderTo: 'symbolspline',
                type: 'spline'
            },
            title: {
                text: 'Monthly Average Temperature'
            },
            subtitle: {
                text: 'Source: WorldClimate.com'
            },
            xAxis: {
                // categories: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun',
                //              'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
                categories:['Mon', 'Tue', 'Wed', 'Thu', 'Fir', 'Sat', 'Sun']
            },
            yAxis: {
                title: {
                    text: 'Temperature'
                },
                labels: {
                    formatter: function() {
                        return this.value +'°'
                    }
                }
            },
            tooltip: {
                crosshairs: true,
                    <!-- shared: true -->
            },
            plotOptions: {
                spline: {
                    marker: {
                        radius: 4,
                        lineColor: '#111111',
                        lineWidth: 1
                    }
                }
            },
            series: [{
                name: 'Temperature',
                marker: {
                    symbol: 'square'
                },
                // data: [7.0, 6.9, 9.5, 14.5, 18.2, 21.5, 25.2, {
                //     y: 26.5,
                //     marker: {
                //         symbol: 'url(./imgs/sun.png)'
                //     }
                // }, 23.3, 18.3, 13.9, 9.6]
                
                data:temperature
            }, {
                name: 'Light',
                marker: {
                    symbol: 'diamond'
                },
                // data: [{
                //     y: 3.9,
                //     marker: {
                //         symbol: 'url(./imgs/snow.png)'
                //     }
                // }, 4.2, 5.7, 8.5, 11.9, 15.2, 17.0, 16.6, 14.2, 10.3, 6.6, 4.8]
                data:light
            },  {
                name: 'Sound',
                marker: {
                    symbol: 'diamond'
                },
                // data: [{
                //     y: 3.9,
                //     marker: {
                //         symbol: 'url(./imgs/snow.png)'
                //     }
                // }, 4.2, 5.7, 8.5, 11.9, 15.2, 17.0, 16.6, 14.2, 10.3, 6.6, 4.8]
                data:sound
            }]
        });
    });
}
