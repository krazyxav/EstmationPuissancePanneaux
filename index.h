/*
 * https://newbiely.com/tutorials/esp8266/esp8266-websocket
 */

const char *HTML_CONTENT = R"=====(
<!DOCTYPE html>

<html>
  <head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1252">
  <title>Capteur Solaire</title>
  <meta name="viewport" content="width=device-width, initial-scale=0.7">
  <style>
    body { background-color:#555888; font-family:sans-serif; color:#fff;  font-size: 16px;text-align: center}
    h1 { text-align: center;}
    footer{ text-align: center; margin: 50px;}
    .chat-container { width: 400px; margin: 0 auto; padding: 10px; }
    .chat-messages { height: 250px; overflow-y: auto; border: 1px solid #444; padding: 5px; margin-bottom: 5px;}
    .user-input {width: 400px; display: flex; margin-bottom: 20px; margin-left: auto; margin-right: auto; }
    .user-input input { flex: 1; border: 1px solid #444; padding: 5px; }
    .user-input button { margin-left: 5px; background-color: #007bff; color: #fff;  border: none;  padding: 5px 10px;  cursor: pointer; }
    .websocket { display: flex; align-items: center; margin-bottom: 5px;}
    .websocket button { background-color: #007bff; color: #fff; border: none; padding: 5px 10px; cursor: pointer;}
    .websocket .label { margin-left: auto;}

    .capteurs {display: flex; align-items: center; justify-content: center;}
    .vert {display: flex; flex-direction: column; align-items: center; justify-content: center;}
    .horiz {display: flex; flex-direction: row;}

    .ChartGauge  { width:200px; height:150px; margin:20px;text-align:center; }
    .ChartBackground { position:relative; height:100px; margin-bottom:10px; background-color:#fff; border-radius:150px 150px 0 0; overflow:hidden; text-align:center; }
    .ChartMask { position:absolute; top:20px; right:20px; left:20px; height:80px; background-color:#555888; border-radius:150px 150px 0 0 }
    .ChartPercentage { position:absolute; top:100px; left:-200%; width:400%; height:400%; margin-left:100px; background-color:#00aeef; }
    .ChartPercentage { transform-origin:top center;}
    #ChartPercentage_haut, #ChartPercentage_gauche, #ChartPercentage_droite, #ChartPercentage_bas {transition:transform 0.5s; }
    /*.ChartMask { transform:rotate(120deg); transform-origin:top center; transition:transform 0.5s;}*/
    .ChartMin { float:left; font-size:12px; }
    .ChartMax { float:right; font-size:12px;}
    .ChartValue { position:absolute; top:70%; left:0; width:100%; font-size:30px; font-weight:700; }
    .ChartValue2 { position:absolute; top:50%; left:0; width:100%; font-size:15px; font-weight:700; }
    .chartTitre {text-align:center;}
    
  </style>
  
  <script>
    var ws;
    var wsm_max_len = 4096; /* bigger length causes uart0 buffer overflow with low speed smart device */

    function update_text(text) {

      var tension_CAN = text.split('|');
      //séparation des valeurs
      // tension_CAN[0] tension haut
      // tension_CAN[1] tension bas
      // tension_CAN[2] tension gauche
      // tension_CAN[3] tension droite
      // tension_CAN[4] commentaire
      
      ///////////////////////////////////////////////////
      //Déclaration des variables
      //Si non spécifié: haut, bas, gauche, droite

      var red = [0,0,0,0];
      var blue = [0,0,0,0];
      var green = [0,0,0,0];
      var max_niv_tens = 24000;
      
      var ChartValue = ["ChartValue_haut", "ChartValue_bas", "ChartValue_gauche", "ChartValue_droite"];
      var ChartValue2 = ["ChartValue2_haut", "ChartValue2_bas", "ChartValue2_gauche", "ChartValue2_droite"];
      var ChartPercentage = ["ChartPercentage_haut", "ChartPercentage_bas", "ChartPercentage_gauche", "ChartPercentage_droite"];
      var chat_messages = document.getElementById("chat-messages");
      var ChartAngle = [0,0,0,0];
      var tension_Volts = 0;
      var niv_tens =0;

      //i 0 haut, 1 bas, 2 gauche, 3 droite
      for (let i = 0; i < 4; i++) {

        ChartAngle[i]=Number(tension_CAN[i])*180/max_niv_tens;
        niv_tens=Number(tension_CAN[i]);

        tension_Volts=Number(tension_CAN[i])*4.096/32767;
        tension_Volts=tension_Volts.toFixed(3);
        
        document.getElementById(ChartValue[i]).innerHTML=tension_Volts;
        document.getElementById(ChartValue2[i]).innerHTML=tension_CAN[i];
        document.getElementById(ChartPercentage[i]).style.transform = "rotate(" + String(ChartAngle[i]) + "deg)";
        console.log("Element: " + ChartPercentage[i] + " / Niveau tension: " + tension_CAN[i] )
        
        //calcul de couleurs
        //Evolution: bleu/vert/jaune/rouge/violet
        if (niv_tens >= (max_niv_tens*4/5)){
          red[i]=255;
          green[i]= 0;
          blue[i]=Math.round((niv_tens-19200)*255/4800);
        } else {
          if (niv_tens >= (max_niv_tens*3/5)) {
            red[i]=255;
            green[i]=Math.round(255-((niv_tens-14400)*255/4800));
            blue[i]=0;
          } else { 
            if (niv_tens >= (max_niv_tens*2/5)) {
              red[i]=Math.round((niv_tens-9600)*255/4800);
              green[i]=255;
              blue[i]=0;
            } else {
              if (niv_tens >= (max_niv_tens/5)) {
                red[i]=0;
                green[i]=255;
                blue[i]=Math.round(255-((niv_tens-4800)*255/4800));
              } else {
                red[i]=0;
                green[i]=Math.round(niv_tens*255/4800);
                blue[i]=255;
              }
            }
          }
        }

      document.getElementById(ChartPercentage[i]).style.background="rgb("+String(red[i])+","+String(green[i])+","+String(blue[i])+")";
      console.log("couleurs rgb: " + String(red[i]) + " / " + String(green[i]) + " / " + String(blue[i]));
              
      }
      //ne pas faire de comparaison sur null pour détection de chaine de caractère
      if(tension_CAN[4]!= ""){
          chat_messages.innerHTML += tension_CAN[4] + '<br>';
          chat_messages.scrollTop = chat_messages.scrollHeight;
      }
    } //End update_text(text)

    function send_onclick() {
      if(ws != null) {
        var message = document.getElementById("message").value;
        
        if (message) {
          document.getElementById("message").value = "";
          ws.send(message + "\n");
          update_text(message);
          //update_text('<span style="color:navy">' + message + '</span>');
          // You can send the message to the server or process it as needed
        } //endif (message)
      } //endif (ws != null)
    } //end send_onclick()

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
      update_text(e_msg.data);
    }
  
  </script>

  </head>

  <body>
    <h1>Capteur Solaire</h1>

      <div class="capteurs">
        <div class="vert">
         
            <div class="ChartGauge">
              <div class="ChartBackground">
                <div class="ChartPercentage" id="ChartPercentage_haut"></div>
                <div class="ChartMask"></div>
                <span class="ChartValue" id="ChartValue_haut">0</span>
                <span class="ChartValue2" id="ChartValue2_haut">0 V</span>
              </div>
              <span class="ChartMin">0.0</span>
              <span class="ChartMax">3.0</span>
              <span class="ChartTitre">Haut</span>
            </div>
            
            <div class="horiz">
            
              <div class="ChartGauge">
                <div class="ChartBackground">
                  <div class="ChartPercentage" id="ChartPercentage_gauche"></div>
                  <div class="ChartMask"></div>
                  <span class="ChartValue" id="ChartValue_gauche">0</span>
                  <span class="ChartValue2" id="ChartValue2_gauche">0 V</span>
                </div>
                <span class="ChartMin">0.0</span>
                <span class="ChartMax">3.0</span>
                <span class="ChartTitre">Gauche</span>
              </div>

              <div class="ChartGauge">
                <div class="ChartBackground">
                  <div class="ChartPercentage" id="ChartPercentage_droite"></div>
                  <div class="ChartMask"></div>
                  <span class="ChartValue" id="ChartValue_droite">0</span>
                  <span class="ChartValue2" id="ChartValue2_droite">0 V</span>
                </div>
                <span class="ChartMin">0.0</span>
                <span class="ChartMax">3.0</span>
                <span class="ChartTitre">Droite</span>
              </div>
              
            </div>
            
            <div class="ChartGauge">
              <div class="ChartBackground">
                <div class="ChartPercentage" id="ChartPercentage_bas"></div>
                <div class="ChartMask"></div>
                <span class="ChartValue" id="ChartValue_bas">0</span>
                <span class="ChartValue2" id="ChartValue2_bas">0 V</span>
              </div>
              <span class="ChartMin">0.0</span>
              <span class="ChartMax">3.0</span>
              <span class="ChartTitre">Bas</span>
            </div>
            
        </div>
      </div>

      <div class="user-input">
        <input type="text" id="message" placeholder="Type your message...">
        <button onclick="send_onclick()">Send</button>
      </div>
      
      <div class="chat-container">
        <div class="websocket">
          <button class="connect-button" id="bt_connect" onclick="connect_onclick()">Connect</button>
          <span class="label">WebSocket: <span id="ws_state"><span style="color:blue">CLOSED</span></span></span>
        </div>
        <div class="chat-messages" id="chat-messages"></div>

      </div>

  </body>

  <footer>
    Gain 1: 0 -> 4.096 v, limite par programme a 3,0 v
  </footer>




</html>
)=====";
