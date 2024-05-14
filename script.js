var ws;
var wsm_max_len = 4096; /* bigger length causes uart0 buffer overflow with low speed smart device */

function update_text(text) {

    var tens_haut = document.getElementById("tens_haut");
    var tens_bas = document.getElementById("tens_bas");
    var tens_gauche = document.getElementById("tens_gauche");
    var tens_droite = document.getElementById("tens_droite");
    var chat_messages = document.getElementById("chat-messages");

    var tens_haut_canv = document.getElementById("tens_haut_jauge");
    var ctx_haut = tens_haut_canv.getContext("2d");
    var coef_haut=Number(tmp[0])/32767;

    //tens_haut.innerHTML = text;
    console.log("nouvelle ligne: " + text);

    //séparation des valeurs
    var tmp = text.split('|');
    // tmp[0] tension haut
    // tmp[1] tension bas
    // tmp[2] tension gauche
    // tmp[3] tension droite
    // tmp[4] commentaire

    tens_haut.innerHTML = 'Haut: ' + tmp[0];

    tens_bas.innerHTML = 'Bas: ' + tmp[1];

    tens_gauche.innerHTML = 'Gauche: '+ tmp[2];
  
    tens_droite.innerHTML = 'Droite: ' + tmp[3];
    
    if(tmp[4]!=""){
        chat_messages.innerHTML += tmp[4] + '<br>';
        chat_messages.scrollTop = chat_messages.scrollHeight;
    }
}

function send_onclick() {
  if(ws != null) {
    var message = document.getElementById("message").value;
    
    if (message) {
      document.getElementById("message").value = "";
      ws.send(message + "\n");
      update_text(message);
      //update_text('<span style="color:navy">' + message + '</span>');
      // You can send the message to the server or process it as needed
    }
  }
}

function connect_onclick() {
  if(ws == null) {
    ws = new WebSocket("ws://" + window.location.host + ":81");
    document.getElementById("ws_state").innerHTML = "CONNECTING";
    ws.onopen = ws_onopen;
    ws.onclose = ws_onclose;
    ws.onmessage = ws_onmessage;
  } else
    ws.close();
}

function ws_onopen() {
  document.getElementById("ws_state").innerHTML = "<span style='color:blue'>CONNECTED</span>";
  document.getElementById("bt_connect").innerHTML = "Disconnect";
  document.getElementById("chat-messages").innerHTML = "";
}

function ws_onclose() {
  document.getElementById("ws_state").innerHTML = "<span style='color:gray'>CLOSED</span>";
  document.getElementById("bt_connect").innerHTML = "Connect";
  ws.onopen = null;
  ws.onclose = null;
  ws.onmessage = null;
  ws = null;
}

function ws_onmessage(e_msg) {
  e_msg = e_msg || window.event; // MessageEvent
  //console.log(e_msg.data);
  //update_text('<span style="color:blue">' + e_msg.data + '</span>');
  update_text(e_msg.data);
}