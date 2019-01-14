var gblFramedDiagram = -1; //none
var gblCurrentMove = null;
var gblPlaying=false;
var gblTimeoutId = 0;
var gblTimerRunning=false;
var gblBackwards=false;
var gblBackwardsMove = null;

function setPlayPauseImage()
{        
    if( gblFramedDiagram >= 0 )
    {
        var mvParameters = gblCurrentMove.attr('rel');
        var arr = mvParameters.split(' ');
        var nextMove = parseInt(arr[4],10);
        if( gblPlaying )
            document.images[(gblFramedDiagram+1)*64 + 4].src  = nextMove==-1?'png/blank.png':'png/mpause.png';
        else
            document.images[(gblFramedDiagram+1)*64 + 4].src  = nextMove==-1?'png/blank.png':'png/mplay.png';
    }
}

function stopPlaying()
{
    gblBackwards = false;
    gblPlaying = false;
    if( gblTimerRunning )
        window.clearTimeout( gblTimeoutId );
    gblTimerRunning=false;
    setPlayPauseImage();
}

function changeHighlight( nextMove, iNextMove )
{
    if( gblCurrentMove )
        gblCurrentMove.removeClass('highlight');
    if( nextMove )
        gblCurrentMove = nextMove; 
    else
        gblCurrentMove = $('#mv' + iNextMove ); 
    gblCurrentMove.addClass('highlight');
}

function changeFrame( diagNbr )
{
    //console.log( 'changeFrame(' + diagNbr + ')' );

    // If there is a framed diagram, unframe it
    if( gblFramedDiagram >= 0 ) {
        var buttonsId = "buttons" + gblFramedDiagram;
        $('#' + buttonsId).replaceWith("<div class='buttons' id='" + buttonsId +  "'> </div>");
        var str2 = $('#' + buttonsId).parent().find('.diagram').attr('rel');
        var arr2 = str2.split(' ');
        var posn2    = arr2[0];
        var diagNbr2 = parseInt(arr2[1],10);
        pos(posn2,diagNbr2,'b','Moves are clickable');
    }
    
    var mvParameters = gblCurrentMove.attr('rel');
    var arr = mvParameters.split(' ');
    var prevMove = parseInt(arr[3],10);
    var nextMove = parseInt(arr[4],10);


    // Frame this diagram
    gblFramedDiagram = diagNbr;
    var buttonsClass = '#buttons' + gblFramedDiagram;
    ////console.log(buttonsClass);
    $(buttonsClass).addClass('locateButtons');
    $(buttonsClass).append("<img class='exit' src='png/exit.png' alt=''>"+
                            "<img class='backward' src=" + (prevMove==-1 ? "'png/blank.png'" : "'png/mback.png'") + " alt=''>"+
                            "<img class='rover' src='png/blank.png' alt=''>"+
                            "<img src='png/blank.png' alt=''>"+
                            "<img class='play' src=" + (nextMove==-1 ? "'png/blank.png'" : (gblPlaying?"'png/mpause.png'":"'png/mplay.png'")) + " ' alt=''>"+
                            "<img src='png/blank.png' alt=''>"+
                            "<img src='png/blank.png' alt=''>"+
                            "<img class='forward' src=" + (nextMove==-1 ? "'png/blank.png'" : "'png/mforward.png'") + " alt=''><br>");
    //$(buttonsClass).scrollIntoView(true);

    // Attach a function to the forward button
    $(".forward").click(forwardMovePressed);

    // Attach a function to the backward button
    $(".backward").click(backwardMovePressed);

    // Attach a function to the play button
    $(".play").click(playPausePressed);

    // Attach a function to the exit button
    $(".exit").click(exitPressed);
}

function changeFrameAndScroll(fromPosn,fromMoveTxt,fromInitialPosition)
{
    var mvParameters = gblCurrentMove.attr('rel');
    var arr = mvParameters.split(' ');
    var diagNbr = parseInt(arr[0],10);
    var posn = arr[1];
    var moveTxt = arr[2];                     
    var animationElement = $("html, body"); // Nice html/body trick from Stackoverflow
	// var animationElement = $($.browser.webkit?'body':'html'); // old version - has stopped working with modern Chrome?
	//	webkit vs mozilla/ie  only works on JQuery < 1.9, for Chrome we want 'html' for Firefox 'body'
    var topOffset =  animationElement.scrollTop();
    var buttonOffset = gblFramedDiagram>=0 ? $(".forward").offset().top : 0;

    // Is the diagram for this move different from the framed diagam ?
    if( diagNbr == gblFramedDiagram )
    {
        cbAnimateMove();  // no, animate move
    }

    // Yes, change frames then animate scroll
    else
    {
        changeFrame( diagNbr );
        var desc = 'Position after ' + fromMoveTxt;
        if( fromInitialPosition ) //&& fromPosn=='ahff32FFAH' )
            desc = '  Initial position';
        pos( fromPosn, diagNbr, 'y', desc );

        // Scroll
        newButtonOffset = $(".forward").offset().top;
        animationElement.animate({
                 scrollTop: ( topOffset + (newButtonOffset-buttonOffset) )
             }, 'slow', cbPauseAfterScroll );
    }
}

function cbPauseAfterScroll()
{                              
    if( !gblTimerRunning ) {
        gblTimerRunning = true;
        gblTimeoutId = setTimeout( cbAnimateMove, 1000 );
    }
}

function cbAnimateMove()
{
    //console.log( 'cbAnimateMove()' );
    if( gblTimerRunning )
        window.clearTimeout( gblTimeoutId );
    gblTimerRunning=false;
    var err = false;
    var rel = gblCurrentMove.attr('rel');
    var arr = rel.split(' ');
    var nextMove = parseInt(arr[4],10);
    var srcText = arr[5];
    if( gblBackwards )
    {
        srcText = gblBackwardsMove;
    }       
    if( !err )
    {
        //console.log( 'srcText=' + srcText );
        var piece = srcText.charCodeAt(0);
        var srcPng = (piece>=0x60 ? 'b' : 'w'); // islower
        if( piece >= 0x60 )  //islower
            piece -= 0x20; //toupper
        if( piece == 0x50 ) // there must be a better way!
            srcPng += 'p';
        else if( piece == 0x51 )
            srcPng += 'q';
        else if( piece == 0x52 )
            srcPng += 'r';
        else if( piece == 0x4e )
            srcPng += 'n';
        else if( piece == 0x4b )
            srcPng += 'k';
        else //if( piece == 0x42 )
            srcPng += 'b';
        var hexSrcFile = srcText.charCodeAt(gblBackwards?3:1);    // 'a'-'h'
        var hexSrcRank = srcText.charCodeAt(gblBackwards?4:2);    // '1'-'8'
        var hexDstFile = srcText.charCodeAt(gblBackwards?1:3);    // 'a'-'h'
        var hexDstRank = srcText.charCodeAt(gblBackwards?2:4);    // '1'-'8'
        if( 0x61<=hexSrcFile && hexSrcFile <= 0x68 && 0x31<=hexSrcRank && hexSrcRank <= 0x38 && 
            0x61<=hexDstFile && hexDstFile <= 0x68 && 0x31<=hexDstRank && hexDstRank <= 0x38 )
        {
            var dstFile = hexDstFile-0x61;
            var dstRank = hexDstRank-0x31;
            var srcFile = hexSrcFile-0x61;
            var srcRank = hexSrcRank-0x31;
            var srcTop  = 45 + 36*srcRank;
            var srcLeft = -73 + 36*srcFile;
            var dstTop  = 45 + 36*dstRank;
            var dstLeft = -73 + 36*dstFile;
            var strSrcTop  = '-' + srcTop + 'px';
            var strSrcLeft = '' + srcLeft + 'px';
            var strDstTop  = '-' + dstTop + 'px';
            var strDstLeft = '' + dstLeft + 'px';
            var w;
            var i = (7-srcRank)*8 + srcFile; 

            if( i<8 || (16<=i && i<24) || (32<=i && i<40)|| (48<=i && i<56) )
                w = ((i&1)==0);
            else
                w = ((i&1)==1);
            document.images[gblFramedDiagram*64 + i].src = (w?'png/w.png':'png/y.png');
            $(".rover").css( 'top',  strSrcTop  );
            $(".rover").css( 'left', strSrcLeft );
            document.images[(gblFramedDiagram+1)*64 + 2].src  = 'png/' + srcPng + '.png';
            //console.log( 'animating ' + srcText + ' cmMove() will follow' );
            if( srcFile == dstFile )
            {
                $(".rover").animate({
                  top:strDstTop,
                }, gblPlaying?'slow':'fast', cbShowMoveAfterAnimation );
            }
            else if( srcRank == dstRank )
            {
                $(".rover").animate({
                  left:strDstLeft
                }, gblPlaying?'slow':'fast', cbShowMoveAfterAnimation );
            }
            else
            {
                $(".rover").animate({
                  top:strDstTop,
                  left:strDstLeft
                }, gblPlaying?'slow':'fast', cbShowMoveAfterAnimation );
            }
        }
        else
        {
            err = true;
        }
    }
    if( err )
    {
        stopPlaying();
    }   
    //console.log( 'cbAnimateMove() end' );
}

function cbShowMoveAfterAnimation()
{
    //console.log( 'cbShowMoveAfterAnimation()' );
    var mvParameters = gblCurrentMove.attr('rel');
    var arr = mvParameters.split(' ');
    var diagNbr = parseInt(arr[0],10);
    var posn = arr[1];
    var moveTxt = arr[2];
    var prevMove = parseInt(arr[3],10);
    var nextMove = parseInt(arr[4],10);
    var desc = 'Position after ' + moveTxt;
    if( prevMove==-1 ) //&& posn=='ahff32FFAH' )
        desc = '  Initial position';
    pos( posn, diagNbr, 'y', desc );
    
    // Show forward back buttons only if apprpriate
    document.images[(gblFramedDiagram+1)*64 + 1].src  = (prevMove==-1 ? 'png/blank.png' : 'png/mback.png');
    document.images[(gblFramedDiagram+1)*64 + 7].src  = (nextMove==-1 ? 'png/blank.png' : 'png/mforward.png');
    
    // Clear the roving image - fixes the long-standing promotion bug
    document.images[(gblFramedDiagram+1)*64 + 2].src  = 'png/blank.png';
    $(".rover").css( 'top',  '0px'  );
    $(".rover").css( 'left', '0px' );
    setPlayPauseImage();
    if( gblPlaying )
    {
        if( !gblTimerRunning ) {
            gblTimerRunning = true;
            gblTimeoutId = setTimeout( cbPauseDuringAutoPlay, 500 );
        }
    }
}

function cbPauseDuringAutoPlay()
{
    if( gblTimerRunning )
        window.clearTimeout( gblTimeoutId );
    gblTimerRunning=false;
    forwardMoveBody( true );
}

function forwardMoveBody( autoRepeat )
{
    gblBackwards = false;
    if( gblCurrentMove )
    {
        gblPlaying = autoRepeat;
        var mvParameters = gblCurrentMove.attr('rel');
        var arr = mvParameters.split(' ');
        var posn = arr[1];
        var moveTxt = arr[2];
        var nextMove = parseInt(arr[4],10);
        if( nextMove == -1 )
        {
            stopPlaying();
        }
        else
        {       
            changeHighlight( null, nextMove );
            var prevMove = parseInt(arr[3],10);
            var fromInitialPosition = (prevMove==-1);
            changeFrameAndScroll(posn,moveTxt,fromInitialPosition);
        }
    }
}

function forwardMovePressed()
{
    stopPlaying();
    forwardMoveBody( false );
}

function backwardMovePressed()
{
    stopPlaying();
    if( gblCurrentMove )
    {
        var mvParameters = gblCurrentMove.attr('rel');
        var arr = mvParameters.split(' ');
        var posn = arr[1];
        var moveTxt = arr[2];
        var prevMove = parseInt(arr[3],10);
        if( prevMove != -1 )
        {
            gblBackwards = true;
            gblBackwardsMove = arr[5];
            changeHighlight( null, prevMove );
            changeFrameAndScroll(posn,moveTxt,false);
        }
    }
}

function playPausePressed()
{
    // Pause ?
    if( gblPlaying )
    {
        stopPlaying();
    }

    // Play ?
    else
    {
        forwardMoveBody( true );
    }
}

function exitPressed()
{
    stopPlaying();
    var str = $(this).parent().parent().find(".diagram").attr("rel");
    var arr = str.split(' ');
    var posn    = arr[0];
    var diagNbr = parseInt(arr[1],10);
    if( gblCurrentMove )
        gblCurrentMove.removeClass('highlight');
    if( diagNbr >= 0 ) {   // unframe
        var buttonsId = "buttons" + diagNbr;
        $('#' + buttonsId).replaceWith("<div class='buttons' id='" + buttonsId +  "'> </div>");
    }               
    gblFramedDiagram = -1;
    pos(posn,diagNbr,'b','Moves are clickable');
}

function buildPosn( compressedPosn )
{
    var posn = expandPosition(compressedPosn);
    ////console.log(posn);
    var str="";
    for( i=0; i<64; i++ )
    {
        str += "<img src='png/";
        var square_colour = (i&1)?'w':'b';
        if( i<8 || (16<=i && i<24) || (32<=i && i<40) || (48<=i && i<56) )
            square_colour = (i&1)?'b':'w';
        var piece = posn.charCodeAt(i);
        var colour = square_colour;
        var empty=true;
        if( piece >= 0x40 ) // isalpha
        {
            empty = false;
            colour = (piece>=0x60 ? 'b' : 'w'); // islower
            if( piece >= 0x60 )  //islower
                piece -= 0x20; //toupper
        }
        if( !empty )
        {
            str += colour;
            if( piece == 0x50 ) // there must be a better way!
                str += 'p';
            else if( piece == 0x51 )
                str += 'q';
            else if( piece == 0x52 )
                str += 'r';
            else if( piece == 0x4e )
                str += 'n';
            else if( piece == 0x4b )
                str += 'k';
            else if( piece == 0x42 )
                str += 'b';
            else
                str += '?';
        }
        str += square_colour;
        str += ".png' alt=''>";
        if( (i&7) == 7 )
            str += "<br>\n";
    }
    ////console.log(str);
    return str;
}

// Click on a move
function doMove( move )
{                                  
    stopPlaying();
    changeHighlight( move, 0 );
    var mvParameters = gblCurrentMove.attr('rel');
    var arr = mvParameters.split(' ');
    var diagNbr = parseInt(arr[0],10);
    var posn = arr[1];
    var moveTxt = arr[2];
    changeFrame( diagNbr );
    pos(posn,diagNbr,'y','Position after ' + moveTxt);
}

$(function(){
    $('.main').css('cursor', 'pointer');
    $('.variation').css('cursor', 'pointer');
    $(".main").hover( function() {
        $(this).addClass('move_hover');
    }, function() {
        $(this).removeClass('move_hover');
    });    
    $(".variation").hover( function() {
        $(this).addClass('move_hover');
    }, function() {
        $(this).removeClass('move_hover');
    });    
    $('.diagram').
        each(function(){
            var str = $(this).attr('rel');
            var posn = str.split(' ')[0];
            var posText = $(this).text();
            if( !posText )
	            $(this).append( buildPosn(posn) );
        });

    // Move clicked
    $('.main').click(function(){
        doMove( $(this) );
    });
    $('.variation').click(function(){
        doMove( $(this) );
    });
	$(document).keydown(function(e) {
		switch(e.which) {
			case 37: // left
			if( gblFramedDiagram >= 0 )
			{
				backwardMovePressed();
			}
			break;

			//case 38: // up
			//break;

			case 39: // right
			if( gblFramedDiagram >= 0 )
			{
				forwardMovePressed();
			}
			break;

			//case 40: // down
			//break;

			default: return; // exit this handler for other keys
		}
		e.preventDefault(); // prevent the default action (scroll / move caret)
	});
});
            
function expandPosition( str )
{
    var s="";
    var n=0;
    for( i=0; i<str.length; i++ )
    {
        var hex = str.charCodeAt(i);
        if( 0x30<=hex && hex <= 0x39 )
            n = 10*n + (hex-0x30);
        else
        {
            while( n-- )
                s += ' ';
            n = 0;
            var c = str.charAt(i);
            if( c == 'c' )
                s += " rk ";
            else if( c == 'C' )
                s += " RK ";
            else if( c == 'a' )
                s += "rnbq"; 
            else if( c == 'u' )
                s += "r bq";
            else if( c == 's' )
                s += "rn q";
            else if( c == 'e' )
                s += "r  q";
            else if( c == 'h' )
                s += "kbnr";
            else if( c == 'g' )
                s += "kb r";
            else if( c == 'm' )
                s += "k nr";
            else if( c == 'v' )
                s += "k  r";
            else if( c == 'f' )
                s += "pppp";
            else if( c == 't' )
                s += "ppp";
            else if( c == 'd' )
                s += "pp";
            else if( c == 'A' )
                s += "RNBQ";
            else if( c == 'U' )
                s += "R BQ";
            else if( c == 'S' )
                s += "RN Q";
            else if( c == 'E' )
                s += "R  Q";
            else if( c == 'H' )
                s += "KBNR";
            else if( c == 'G' )
                s += "KB R";
            else if( c == 'M' )
                s += "K NR";
            else if( c == 'V' )
                s += "K  R";
            else if( c == 'F' )
                s += "PPPP";
            else if( c == 'T' )
                s += "PPP";
            else if( c == 'D' )
                s += "PP";
            else
                s += c;
        }
    }
    while( s.length < 64 )
        s += ' ';
    return s;
}

function pos( compressedStr, diagIdx, darkColour, label )
{
    var str = expandPosition(compressedStr);
    var diagId = "diagram" + diagIdx;
    document.getElementById(diagId).innerHTML=label;
    var darkSquare = '' + darkColour + '.png';
    for( i=0; i<64; i++ )
    {                    
        var w;
        if( i<8 || (16<=i && i<24) || (32<=i && i<40)|| (48<=i && i<56) )
            w = ((i&1)==0);
        else
            w = ((i&1)==1);
        var background = (w ? 'w.png' : darkSquare);
        var c = str[i];
        if( c == 'r' )
            fn = 'png/br' + background;
        else if( c == 'n' )
            fn = 'png/bn' + background;
        else if( c == 'b' )
            fn = 'png/bb' + background;
        else if( c == 'q' )
            fn = 'png/bq' + background;
        else if( c == 'k' )
            fn = 'png/bk' + background;
        else if( c == 'p' )
            fn = 'png/bp' + background;
        else if( c == 'R' )
            fn = 'png/wr' + background;
        else if( c == 'N' )
            fn = 'png/wn' + background;
        else if( c == 'B' )
            fn = 'png/wb' + background;
        else if( c == 'Q' )
            fn = 'png/wq' + background;
        else if( c == 'K' )
            fn = 'png/wk' + background;
        else if( c == 'P' )
            fn = 'png/wp' + background;
        else
            fn = 'png/' + background;
        document.images[diagIdx*64 + i].src  = fn;
    }
}

