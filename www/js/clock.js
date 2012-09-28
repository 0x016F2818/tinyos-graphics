/**
 *This is a clock control
 *
 */
var updateTime = function() { // Update the SVG clock graphic to show current time
    var now = new Date();                       // Current time
    var min = now.getMinutes();                 // Minutes
    var hour = (now.getHours() % 12) + min/60;  // Fractional hours
    var minangle = min*6;                       // 6 degrees per minute
    var hourangle = hour*30;                    // 30 degrees per hour

    // Get SVG elements for the hands of the clock
    var minhand = document.getElementById("minutehand");
    var hourhand = document.getElementById("hourhand");

    // Set an SVG attribute on them to move them around the clock face
    minhand.setAttribute("transform", "rotate(" + minangle + ",50,50)");
    hourhand.setAttribute("transform", "rotate(" + hourangle + ",50,50)");

    // Update the clock again in 1 minute
    setTimeout(updateTime, 60000);
}
// window.onload = updateTime;
/**
 *This is for the IE or firefox
 */
// A script of js code
// Define a function to display the current time
var displayTime = function() {
    var elt = document.getElementById("clock2");  // Find element with id="clock"
    var now = new Date();                        // Get current time
    elt.innerHTML = now.toLocaleTimeString();    // Make elt display it
    setTimeout(displayTime, 1000);               // Run again in 1 second
}
// window.onload = displayTime;  // Start displaying the time when document loads.

clock = function () {
    
    /**
     * Get the current time
     */
    function getNow () {
        var now = new Date();
        
        return {
            hours: now.getHours() + now.getMinutes() / 60,
            minutes: now.getMinutes() * 12 / 60 + now.getSeconds() * 12 / 3600,
            seconds: now.getSeconds() * 12 / 60
        };
    };
    
    var now = getNow();
    
    // Create the chart
    var chart = new Highcharts.Chart({
        
        chart: {
            renderTo: 'container',
            type: 'gauge',
            plotBackgroundColor: null,
            plotBackgroundImage: null,
            plotBorderWidth: 0,
            plotShadow: false,
            height: 200
        },
        
        credits: {
            enabled: false
        },
        
        title: {
            text: 'The Highcharts clock'
        },
        
        pane: {
            background: [{
                // default background
            }, {
                // reflex for supported browsers
                backgroundColor: Highcharts.svg ? {
                    radialGradient: {
                        cx: 0.5,
                        cy: -0.4,
                        r: 1.9
                    },
                    stops: [
                        [0.5, 'rgba(255, 255, 255, 0.2)'],
                        [0.5, 'rgba(200, 200, 200, 0.2)']
                    ]
                } : null
            }]
        },
        
        yAxis: {
            labels: {
                distance: -20
            },
            min: 0,
            max: 12,
            lineWidth: 0,
            showFirstLabel: false,
            
            minorTickInterval: 'auto',
            minorTickWidth: 1,
            minorTickLength: 5,
            minorTickPosition: 'inside',
            minorGridLineWidth: 0,
            minorTickColor: '#666',
            
            tickInterval: 1,
            tickWidth: 2,
            tickPosition: 'inside',
            tickLength: 10,
            tickColor: '#666',
            title: {
                text: 'Powered by<br/>Highcharts',
                style: {
                    color: '#BBB',
                    fontWeight: 'normal',
                    fontSize: '8px',
                    lineHeight: '10px'                
                },
                y: 10
            }       
        },
        
        series: [{
            data: [{
                id: 'hour',
                y: now.hours,
                dial: {
                    radius: '60%',
                    baseWidth: 4,
                    baseLength: '95%',
                    rearLength: 0
                }
            }, {
                id: 'minute',
                y: now.minutes,
                dial: {
                    baseLength: '95%',
                    rearLength: 0
                }
            }, {
                id: 'second',
                y: now.seconds,
                dial: {
                    radius: '100%',
                    baseWidth: 1,
                    rearLength: '20%'
                }
            }],
            animation: false,
            dataLabels: {
                enabled: false
            }
        }]
    }, 
    // Move
    function (chart) {
        setInterval(function () {
            var hour = chart.get('hour'),
            minute = chart.get('minute'),
            second = chart.get('second'),
            now = getNow(),
            // run animation unless we're wrapping around from 59 to 0
            animation = now.seconds == 0 ? 
                false : 
                {
                    easing: 'easeOutElastic'
                };
            
            hour.update(now.hours, true, animation);
            minute.update(now.minutes, true, animation);
            second.update(now.seconds, true, animation);
            
        }, 1000);
        
    });
};

// Extend jQuery with some easing (copied from jQuery UI)
$.extend($.easing, {
    easeOutElastic: function (x, t, b, c, d) {
        var s=1.70158;var p=0;var a=c;
        if (t==0) return b;  if ((t/=d)==1) return b+c;  if (!p) p=d*.3;
        if (a < Math.abs(c)) { a=c; var s=p/4; }
        else var s = p/(2*Math.PI) * Math.asin (c/a);
        return a*Math.pow(2,-10*t) * Math.sin( (t*d-s)*(2*Math.PI)/p ) + c + b;
    }
});
