$(function() {
    if($.browser.msie || $.browser.mozilla) {
        $("#clockSVG").addClass("hide");
        $("#clockIEFF").removeClass("hide");
        $("#clock2").load(displayTime);
    } else {
        $("#clockIEFF").addClass("hide");
        $("#clock").load(updateTime);
    }
})
