var app = function() {
	//Init UI
	var ip = "192.168.1.8"

	// Walking
	$("#bt-walk-front").click(()=>{
		alert("front");
	})
	$("#bt-walk-stop").click(()=>{
		alert("front");
	})
	$("#bt-walk-left").click(()=>{
		alert("front");
	})
	$("#bt-walk-right").click(()=>{
		alert("front");
	})

	//Sounds
	$(".bt-sound").click((e)=> {
    	// Do something /voice/play
    	$.get( "http://" + ip + "/voice/play?track=" + e.target.name );
	});

}();
