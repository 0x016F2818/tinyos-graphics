function setNextVisible() {

    if($.browser.chrome) {
        $("#clockIEFF").addClass("hide");
        $("#clockSVG").ready(updateTime);
    } else {
        $("#clockSVG").addClass("hide");
        $("#clockIEFF").removeClass("hide");
        $("#clockIEFF").load(displayTime);
    }

    var blocks = document.getElementsByTagName('p');
    for (var i = 0; i < blocks.length; i++) {
        var block = blocks[i];
        if (!block.style) continue;
        if (block.style.display == 'block') continue;
        block.style.display = 'block';
        setTimeout(setNextVisible, 3000);
        if (block.id == 'fade-trigger')
            fadeout();
        return;
    }
}

function fadeout() {
    var e = document.getElementById('fade');
    if (e.style.opacity > 0) {
        e.style.opacity -= .01;
        setTimeout(fadeout, 40);
    }
}
