function drawRotatedColumn() {
    var chart;
    $(document).ready(function() {
        chart = new Highcharts.Chart({
            chart: {
                renderTo: 'rotatedcolumn',
                type: 'column',
                // margin: [ 50, 50, 100, 80]
            },
            title: {
                text: 'World\'s largest cities per 2008'
            },
            xAxis: {
                categories: [
                    // 'Tokyo',
                    // 'Jakarta',
                    // 'New York',
                    // 'Seoul',
                    // 'Manila',
                    // 'Mumbai',
                    // 'Sao Paulo',
                    // 'Mexico City',
                    // 'Dehli',
                    // 'Osaka',
                    // 'Cairo',
                    // 'Kolkata',
                    // 'Los Angeles',
                    // 'Shanghai',
                    // 'Moscow',
                    // 'Beijing',
                    // 'Buenos Aires',
                    // 'Guangzhou',
                    // 'Shenzhen',
                    // 'Istanbul'
                    'Moday',
                    'Tuesday',
                    'Wednesday',
                    'Thusday',
                    'Friday',
                    'Saturday',
                    'Sunday'
                ],
                labels: {
                    rotation: -45,
                    align: 'right',
                    style: {
                        fontSize: '13px',
                        fontFamily: 'Verdana, sans-serif'
                    }
                }
            },
            yAxis: {
                min: 0,
                title: {
                    text: 'Population (millions)'
                }
            },
            // legend: {
            //     enabled: false
            // },
            tooltip: {
                formatter: function() {
                    return '<b>'+ this.x +'</b><br/>'+
                        'Population in 2008: '+ Highcharts.numberFormat(this.y, 1) +
                        ' millions';
                }
            },
            plotOptions: {
                column: {
                    stacking: 'percent'
                }
            },
            series: [{
                name: 'Max',
                // data: [34.4, 21.8, 20.1, 20, 19.6, 19.5, 19.1, 18.4, 18,
                //        17.3, 16.8, 15, 14.7, 14.5, 13.3, 12.8, 12.4, 11.8,
                //        11.7, 11.2],
                data: [34.4, 21.8, 20.1, 20, 19.6, 19.5, 19.1],
                dataLabels: {
                    enabled: true,
                    rotation: -90,
                    color: '#FFFFFF',
                    align: 'right',
                    x: -3,
                    y: 10,
                    formatter: function() {
                        return this.y;
                    },
                    style: {
                        fontSize: '13px',
                        fontFamily: 'Verdana, sans-serif'
                    }
                }
            }, {
                name: 'Min',
                // data: [34.4, 21.8, 20.1, 20, 19.6, 19.5, 19.1, 18.4, 18,
                //        17.3, 16.8, 15, 14.7, 14.5, 13.3, 12.8, 12.4, 11.8,
                //        11.7, 11.2],
                data:[17.3, 16.8, 15, 14.7, 14.5, 13.3, 12.8],
                dataLabels: {
                    enabled: true,
                    rotation: -90,
                    color: '#FFFFFF',
                    align: 'right',
                    x: -3,
                    y: 10,
                    formatter: function() {
                        return this.y;
                    },
                    style: {
                        fontSize: '13px',
                        fontFamily: 'Verdana, sans-serif'
                    }
                }
            }]
        });
    });
}


function drawRotatedColumn2() {
    var chart;
    $(document).ready(function() {
        chart = new Highcharts.Chart({
            chart: {
                renderTo: 'rotatedcolumn',
                type: 'column'
            },
            title: {
                text: 'Stacked column chart'
            },
            xAxis: {
                categories: ['Apples', 'Oranges', 'Pears', 'Grapes', 'Bananas']
            },
            yAxis: {
                min: 0,
                title: {
                    text: 'Total fruit consumption'
                }
            },
            tooltip: {
                formatter: function() {
                    return ''+
                        this.series.name +': '+ this.y +' ('+ Math.round(this.percentage) +'%)';
                }
            },
            plotOptions: {
                column: {
                    stacking: 'percent'
                }
            },
            series: [{
                name: 'John',
                data: [5, 3, 4, 7, 2]
            }, {
                name: 'Jane',
                data: [2, 2, 3, 2, 1]
            }, {
                name: 'Joe',
                data: [3, 4, 4, 2, 5]
            }]
        });
    });
    
};



function drawMaxandMinColumn() {
    var chart;
    var colors = Highcharts.getOptions().colors,
    categories = ['MSIE', 'Firefox', 'Chrome', 'Safari', 'Opera'],
    name = 'Browser brands',
    data = [{
        y: 55.11,
        color: colors[0],
        drilldown: {
            name: 'MSIE versions',
            categories: ['MSIE 6.0', 'MSIE 7.0', 'MSIE 8.0', 'MSIE 9.0'],
            data: [10.85, 7.35, 33.06, 2.81],
            color: colors[0]
        }
    }, {
        y: 21.63,
        color: colors[1],
        drilldown: {
            name: 'Firefox versions',
            categories: ['Firefox 2.0', 'Firefox 3.0', 'Firefox 3.5', 'Firefox 3.6', 'Firefox 4.0'],
            data: [0.20, 0.83, 1.58, 13.12, 5.43],
            color: colors[1]
        }
    }, {
        y: 11.94,
        color: colors[2],
        drilldown: {
            name: 'Chrome versions',
            categories: ['Chrome 5.0', 'Chrome 6.0', 'Chrome 7.0', 'Chrome 8.0', 'Chrome 9.0',
                         'Chrome 10.0', 'Chrome 11.0', 'Chrome 12.0'],
            data: [0.12, 0.19, 0.12, 0.36, 0.32, 9.91, 0.50, 0.22],
            color: colors[2]
        }
    }, {
        y: 7.15,
        color: colors[3],
        drilldown: {
            name: 'Safari versions',
            categories: ['Safari 5.0', 'Safari 4.0', 'Safari Win 5.0', 'Safari 4.1', 'Safari/Maxthon',
                         'Safari 3.1', 'Safari 4.1'],
            data: [4.55, 1.42, 0.23, 0.21, 0.20, 0.19, 0.14],
            color: colors[3]
        }
    }, {
        y: 2.14,
        color: colors[4],
        drilldown: {
            name: 'Opera versions',
            categories: ['Opera 9.x', 'Opera 10.x', 'Opera 11.x'],
            data: [ 0.12, 0.37, 1.65],
            color: colors[4]
        }
    }];
    
    function setChart(name, categories, data, color) {
        chart.xAxis[0].setCategories(categories, false);
        chart.series[0].remove(false);
        chart.addSeries({
            name: name,
            data: data,
            color: color || 'white'
        }, false);
        chart.redraw();
    }
    
    chart = new Highcharts.Chart({
        chart: {
            renderTo: 'maxandmincolumn',
            type: 'column'
        },
        title: {
            text: 'Browser market share, April, 2011'
        },
        subtitle: {
            text: 'Click the columns to view versions. Click again to view brands.'
        },
        xAxis: {
            categories: categories
        },
        yAxis: {
            title: {
                text: 'Total percent market share'
            }
        },
        plotOptions: {
            column: {
                cursor: 'pointer',
                point: {
                    events: {
                        click: function() {
                            var drilldown = this.drilldown;
                            if (drilldown) { // drill down
                                setChart(drilldown.name, drilldown.categories, drilldown.data, drilldown.color);
                            } else { // restore
                                setChart(name, categories, data);
                            }
                        }
                    }
                },
                dataLabels: {
                    enabled: true,
                    color: colors[0],
                    style: {
                        fontWeight: 'bold'
                    },
                    formatter: function() {
                        return this.y +'%';
                    }
                }
            }
        },
        tooltip: {
            formatter: function() {
                var point = this.point,
                s = this.x +':<b>'+ this.y +'% market share</b><br/>';
                if (point.drilldown) {
                    s += 'Click to view '+ point.category +' versions';
                } else {
                    s += 'Click to return to browser brands';
                }
                return s;
            }
        },
        series: [{
            name: name,
            data: data,
            color: 'white'
        }],
        exporting: {
            enabled: false
        }
    });
}
