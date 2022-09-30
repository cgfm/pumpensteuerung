#ifndef _WEBSITEPAGES_CPP
#define _WEBSITEPAGES_CPP
#include <pgmspace.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP8266 %HOSTNAMEPLACEHOLDER%</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    body {margin:0px auto;}
    h1 {font-size:1.5em;}
    h2 {font-size:1.3em;}
    .switch {position: relative; display: inline-block; width: 94px; height: 52px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 40px; width: 40px; left: 6px; bottom: 6px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(40px); -ms-transform: translateX(40px); transform: translateX(40px)}
    .log {text-align: left;}
    .states{padding-left: 50px; padding-right: 50px;}
    @media (min-width: 576px) { 
      .states{float:left;}
      #logcontent {overflow:scroll;height:calc(100vh - 120px);}
    }
    .inline{display:inline-block; min-width: 140px;}
    .left{text-align:left;}
    .error{color:#666;background-color:#fc9e9e;}
    .info{color:#666;background-color:#b9ffd7;}
    .debug{color:#666;background-color:#cde4fa;}
    .time{color:#666;background-color:#ffebff;}
  </style>
  <script>
    function toggleCheckbox(element) {
      var xmlhttp = new XMLHttpRequest();

      if(element.checked){ xmlhttp.open("GET", "/setState?ventil="+element.id+"&state=ON", true); }
      else { xmlhttp.open("GET", "/setState?ventil="+element.id+"&state=OFF", true); }
      xmlhttp.send();    
      xmlhttp.onreadystatechange = function(){
        if (this.readyState == 4 && this.status == 200){
            window.location.href="/";
        }
      };
    }
    
    document.addEventListener('DOMContentLoaded', () => {
      var objDiv = document.getElementById("logcontent");
      objDiv.innerHTML=objDiv.innerHTML.replaceAll(/\[([\d- :]*)\]/g, '<span class="time">$1</span>');
      objDiv.innerHTML=objDiv.innerHTML.replaceAll(/\[([\w]*)\]/g, '<span class="$1">$&</span>');
      objDiv.scrollTop = objDiv.scrollHeight;
      setTimeout(function() {window.location.href="/"}, 60000);
    });
    
    function gotoClearLog() {
      var answer = confirm ("Soll das Log wirklich gel\xF6scht werden?")
      if (answer)
      window.location="/clearLog";
    }
  </script>
</head>
<body>
  <h1>ESP8266 %HOSTNAMEPLACEHOLDER%</h1>
  <div class="states">
    %BUTTONPLACEHOLDER%
    <div class="left"><a href="/publishState">Publish State</a></div>
    <div class="left"><a href="javascript:gotoClearLog();" onClick>clearLog</a></div>
    <div class="left"><a href="/update" target="_blank">OTA Update</a></div>
  </div>
  <div class="log">
    <h2>Log</h2>
    <div id="logcontent">
      %LOGFILEPLACEHOLDER%
    </div>
  </div>

</body>
</html>
)rawliteral";

const char log_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP8266 %HOSTNAMEPLACEHOLDER%</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block;}
    body {margin:0px auto;}
    .error{color:#666;background-color:#fc9e9e;}
    .info{color:#666;background-color:#b9ffd7;}
    .debug{color:#666;background-color:#cde4fa;}
    .time{color:#666;background-color:#ffebff;}
  </style>
  <script>
    document.addEventListener('DOMContentLoaded', () => {
      var objDiv = document.getElementById("logcontent");
      var logCont = objDiv.innerHTML.trim().split("<br>\n").reverse();
      innerHTML = "<b>"+logCont.shift()+"</b>";
      logCont.forEach(element => console.log(innerHTML+=element+"<br>\n"));
      objDiv.innerHTML=innerHTML;
      objDiv.innerHTML=objDiv.innerHTML.replaceAll(/\[([\d- :]*)\]/g, '<span class="time">$1</span>');
      objDiv.innerHTML=objDiv.innerHTML.replaceAll(/\[([\w]*)\]/g, '<span class="$1">$&</span>');
      objDiv.scrollTop = objDiv.scrollHeight;
      setTimeout(function() {window.location.reload(true)}, 60000);
    });
  </script>
</head>
<body>
  <div id="logcontent">
    %LOGFILEPLACEHOLDER%
  </div>
</body>
</html>
)rawliteral";
#endif