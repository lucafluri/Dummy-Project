


//var urlBase = "http://82.197.171.254/"; // used when hosting the site somewhere other than the ESP8266 (handy for testing without waiting forever to upload to SPIFFS)
var urlBase = ""; // used when hosting the site on the ESP8266

$(document).ready( function() {

  updateAngles();

});

var angles = [1, 1, 95, 65, 70, 1];
var buffer = [1, 1, 95, 65, 70, 1];
sync();

function send(data){
  $.post(urlBase + data);

}

function readAngles(){
  try{
    $.get("/js/angles.dummy", function(data){
      var lines = data.split("\n");
      var len = angles.length;
      for(var i = 0; i < len; i++){
        //displayText(lines[i]);
        buffer[i] = parseInt(lines[i]);
      }
    });
    return true;
  }
  catch(err){
    return false;
  }
}

function displayText(text){
  $("#log").text(text);
}

function sync(){
  angles = buffer.slice(0);
}

function displayAngles(){

  if(buffer.toString() == angles.toString()){
    displayText(" ");
    readAngles();

  }
  else{
    //displayText("Adjusting...");

    sync();
    //alert(angles);
    $("#servo0").val(angles[0].toString());
    $("#servo1").val(angles[1].toString());
    $("#servo2").val(angles[2].toString());
    $("#servo3").val(angles[3].toString());
    $("#servo4").val(angles[4].toString());
    $("#servo5").val(angles[5].toString());
    displayText("Adjusting...");
    readAngles();

  }

}


function updateAngles(){
  var timer = setInterval(function() {
      try{
        displayAngles();
        updateDeg();
      }
      catch(err){displayText(err.message);}
  }, 250);
}

function updateDeg(){
  $("#servo0val").text($("#servo0").val());
  $("#servo1val").text($("#servo1").val());
  $("#servo2val").text($("#servo2").val());
  $("#servo3val").text($("#servo3").val());
  $("#servo4val").text($("#servo4").val());
  $("#servo5val").text($("#servo5").val());
}

function servo0(){
  $.post(urlBase + "servo0?value=" + $("#servo0").val());
  buffer[0] = $("#servo0").val();
}
function servo1(){
  $.post(urlBase + "servo1?value=" + $("#servo1").val());
  buffer[1] = $("#servo1").val();
}
function servo2(){
  $.post(urlBase + "servo2?value=" + $("#servo2").val());
  buffer[2] = $("#servo2").val();
}
function servo3(){
  $.post(urlBase + "servo3?value=" + $("#servo3").val());
  buffer[3] = $("#servo3").val();
}
function servo4(){
  $.post(urlBase + "servo4?value=" + $("#servo4").val());
  buffer[4] = $("#servo4").val();
}
function servo5(){
  $.post(urlBase + "servo5?value=" + $("#servo5").val());
  buffer[5] = $("#servo5").val();
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
  $("#servo2").val("95");
  $("#servo3").val("65");
  $("#servo4").val("70");
  $("#servo5").val("1");

}
