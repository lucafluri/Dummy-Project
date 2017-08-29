


//var urlBase = "http://82.197.171.254/"; // used when hosting the site somewhere other than the ESP8266 (handy for testing without waiting forever to upload to SPIFFS)
var urlBase = ""; // used when hosting the site on the ESP8266

//$(document).ready( function() {

//});



function servo0(){
  $.post(urlBase + "servo0?value=" + $("#servo0").val());
}
function servo1(){
  $.post(urlBase + "servo1?value=" + $("#servo1").val());
}
function servo2(){
  $.post(urlBase + "servo2?value=" + $("#servo2").val());
}
function servo3(){
  $.post(urlBase + "servo3?value=" + $("#servo3").val());
}
function servo4(){
  $.post(urlBase + "servo4?value=" + $("#servo4").val());
}
function servo5(){
  $.post(urlBase + "servo5?value=" + $("#servo5").val());
}



function nod(){
  $.post(urlBase + "nod");
}
function no(){
  $.post(urlBase + "no");
}
function reset(){
  $.post(urlBase + "reset");
  $("#servo0").val("1");
  $("#servo1").val("1");
  $("#servo2").val("90");
  $("#servo3").val("65");
  $("#servo4").val("70");
  $("#servo5").val("1");

}
