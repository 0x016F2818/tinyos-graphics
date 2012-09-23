// $(document).ready(function() {
//     drawSpline('temperature-spline', nodeSlot[2]);
//     // drawSpline('photo-spline');
//     // drawSpline('sound-spline');
// })
// $(document).ready(function() {
function drawDynamicSpline() {
    var vector =  [10,10,10,10,10,10,101,121,11,11,11,11,1,1,113,141,15,116,117,20,1,23,4,3,2,1,5,4,3,44,3,2,1,3,3,4,54,4,3,3,2,3,,4,5,4,6,7,8,23,9,123,34,32,1,2,32,14,5,32,123,34,253,2,123,14,523,234,123,32,123,123,324,5,34,2342,34,213,123,23,52,25,132];
    var name = "temperaturespline";
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
                type: 'spline',
                marginRight: 10,
                zoomType: 'x',
                events: {
                    load: function() {
                        // set up the updating of the chart each second
                        var series = this.series[0];
                        var i = 21;
                        // var time = (new Date()).getTime();

                        // var y_vector = [];
                        // switch(name) {
                        // case 't':
                        //     y_vector = node.temp;
                        //     break;
                        // case 'p':
                        //     y_vector = node.photo;
                        //     break;
                        // case 's':
                        //     y_vector = node.sound;
                        //     break;
                        // default: break;
                        // }
                        // alert(y_vector);

                        setInterval(function() {
                            var x = (new Date()).getTime(), // current time
                            // var x = time + i * 100,
                            y = vector[i];
                            i++;
                            series.addPoint([x, y], true, true);
                        }, 1000);
                    }
                }
            },
            title: {
                text: name
            },
            xAxis: {
                type: 'datetime',
                tickPixelInterval: 150
                // plotBands: [{
                //     id: 'mask-before',
                //     from: Date.UTC(2006, 0, 1),
                //     to: Date.UTC(2008, 7, 1),
                //     color: 'rgba(0, 0, 0, 0.2)'
                // }],
                // title: {
                //     text: null
                // }
            },
            yAxis: {
                title: {
                    text: 'Value'
                },
                plotLines: [{
                    value: 2,
                    width: 1,
                    color: '#808080'
                }]
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
                    time = (new Date()).getTime(),
                    i, j = 0;
                    // var y_vector = [];

                    // switch(name) {
                    // case 't':
                    //     y_vector = node.temp;
                    //     break;
                    // case 'p':
                    //     y_vector = node.photo;
                    //     break;
                    // case 's':
                    //     y_vector = node.sound;
                    //     break;
                    // default: break;
                    // }
                    for (i = -19; i <= 0; i++) {
                        data.push({
                            x: time + i * 1000,
                            y: vector[j]
                        });
                        j++;
                    }
                    return data;
                })()
            }]
        });
    }
}
// })


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