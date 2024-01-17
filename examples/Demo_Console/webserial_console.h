// webserial_console.h
#ifndef _webserial_console_h
#define _webserial_console_h


// This is a simple and minimalistic web page which illustrates
// the websocket / WebSerial mechanism
const char WEBSERIAL_CONSOLE_HTML[]  PROGMEM = R"text(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Console</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <!-- Stylesheet --------------------------------------- -->
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.5rem;
    color: white;
  }
  .header {
    overflow: hidden;
    background-color: #049eb3;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .section {
    padding: 10px;
    max-width: 600px;
    margin: 0 auto;
  }
	.button{
		border-radius: 10px;
	  background-color: #049eb3;
		font-size: 1rem;
		padding: 5px 10px;
		color: white;
		text-decoration: none;
	}
  </style>
  <!-- --------------------------------------------------- -->
</head>
<body>
	<div class="header">
    <h1>ESP Web Console</h1>
	</div>
	<div class="content">
    <div id="cmddiv" class="section" align="left" style="display:none">
			<form action="javascript:sendCommandClick();" accept-charset="utf-8">
				<label for="command">Command</label>
				<input type="text" id="command" name="command" size="50">
				<input class="button"  type="submit" value="Send">
			</form> 
			<p><i>Hint: No need to use the above Command field.<br>
			      Commands may typed into the console window below, followed by Enter</i></p>
		</div>
    <div class="section" >
			<textarea id="console"  cols="80" rows="25"   
								onkeydown = "if (event.keyCode == 13)  handleEnter();">
			</textarea> 
		</div>
		<div class="section" align="left" >		
			<input type="checkbox" id="autoscroll" name="autoscroll" value="Autoscroll" checked ="true">
			<label for="autoscroll"> Autoscroll</label>
			
			<input type="checkbox" id="cmdhide" name="cmdhide" checked ="true" onclick="hideCmdField()">
			<label for="cmdhide">Hide command field</label>
			
			<button class="button" type="button" onclick="clearArea()">Clear Databuffer</button>
		</div>
		<div class="section" >
			<hr>
			<input class="button"  type="button" onclick="location.href='./';" value="Home" />
	  </div>
  </div>
  <!-- Javascript ---------------------------------------- -->
  <script>
  var server = `ws://${window.location.hostname}/webserialws`;
  var websocket;
  window.addEventListener('load', onLoad);
  
  // ---------------------------------------------------
  function initWebSocket() {
    console.log('Connect to ws server');
    websocket = new WebSocket(server);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; 
  }
  // ---------------------------------------------------  
  function onOpen(event) {
    console.log('Connection opened');
    wsSend("+-+-+-"); // Trigger to server
  }
  // ---------------------------------------------------
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  
  // ---------------------------------------------------
  function onMessage(event) {
    console.log('Msg: ' + event.data); 
    var area = document.getElementById('console');
    area.value += event.data;
    
    clipHistory();
    scroll();
  }
 
  // ---------------------------------------------------  
  function onLoad(event) {
    initWebSocket();
    clearArea();
    document.getElementById('autoscroll').checked = true;
    document.getElementById('cmdhide').checked = true;
  }
  // ---------------------------------------------------  
  function sendCommandClick(){
    var cmd = document.getElementById('command');
    wsSend(cmd.value);
    cmd.value = "";
  }
  // ---------------------------------------------------  
  function wsSend(text){
    console.log('wsSend: ' + text); 
    websocket.send(text);
  }

	// ---------------------------------------------------  
	function handleEnter(){
		var area = document.getElementById('console');
		var lines = area.value.split('\n');    // array of strings
		var text = lines.pop();
		console.log('handleEnter: ' + text);
		wsSend(text);
	}

  // ---------------------------------------------------  
	function clipHistory(){
		const maxLines = 5000;
	
		var area = document.getElementById('console');
		var lines = area.value.split('\n');
		for(var i = (lines.length - maxLines); i > 0; i--)
			lines.shift();
		var text = "";
		for(var i = 0; i < lines.length; i++){
			if(i > 0)
				text += '\n';
			text += lines[i];
		}
		area.value = text;
	}
  // NOTICE:
  // For whatever reason, area.scrolltop works only while we do this
  // in a separate function?
  // ---------------------------------------------------
  function scroll() {
    console.log("SCROLL: " + document.getElementById('autoscroll').checked );
    
		if(document.getElementById('autoscroll').checked != true)
			return;

    var area = document.getElementById('console');
    area.scrollTop = area.scrollHeight;
  }
  // ---------------------------------------------------
  function clearArea(){
	  console.log("Clear");
	  document.getElementById('console').value = "";
  }
  // ---------------------------------------------------
  function hideCmdField(){
	  if(document.getElementById('cmdhide').checked){
			console.log("hideCmdField() HIDE");
			document.getElementById('cmddiv').style.display = "none";
		}
	  else {
			console.log("hideCmdField() SHOW");
			document.getElementById('cmddiv').style.display = "block";
		}
  }
  
  </script>
  <!-- --------------------------------------------------- -->
</body>
</html>
)text";


#endif
