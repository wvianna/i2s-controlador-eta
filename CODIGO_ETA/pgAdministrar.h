R"rawTxt(
<!DOCTYPE html>
<html>
<meta http-equiv='Content-Type' content='text/html; charset='utf-8' />
<head><title>Estação de Tratamento de Água</title></head>
<body onload='updateInfo()'><h1><center>PICG I2S</center></h1>
<h2>CONFIGURAÇÕES DA ETA</h2>
<form method='POST' action='/trocarconfig'>
<fieldset><legend><b>Autenticação</b></legend>
<p>Login:</p><input type='text' name='login' id='login' >
<p>Senha:</p><input type='text' name='authpass' id='authpass' value=''>
</fieldset>
<fieldset><legend><b>Rede WiFi</b></legend>
<p>SSID:</p><input type='text' name='ssid' id='ssid' value='' />
<p>Senha:</p><input type='password' name='wifipass' id='wifipass' value=''/>
<p>IP:</p><input type='text' name='serverip' id='serverip' value=''/>
<p>Mascara:</p><input type='text' name='servermask' id='servermask' value=''/>
<p>Gateway:</p><input type='text' name='servergate' id='servergate' value=''/>
<p>DNS:</p><input type='text' name='serverdns' id='serverdns' value=''/>
</fieldset><fieldset><legend><b>Operação</b></legend>
<p>Bomba de Captação:</p><select name='bombasconfig' id='bombasconfig'/>
<script>
  if(document.getElementById('bombasconfig').value=="0"){
    document.write("<option value='0' selected>Apenas MB-500</option>");
    document.write("<option value='1'>Apenas MB-501</option>");
    document.write("<option value='2'>Ambas as Bombas</option>");
  }
  else if(document.getElementById('bombasconfig').value=="1"){
    document.write("<option value='0'>Apenas MB-500</option>");
    document.write("<option value='1' selected>Apenas MB-501</option>");
    document.write("<option value='2'>Ambas as Bombas</option>");
  }
  else{
    document.write("<option value='0'>Apenas MB-500</option>");
    document.write("<option value='1'>Apenas MB-501</option>");
    document.write("<option value='2' selected>Ambas as Bombas</option>");
  }

</script></select>
<p>Nível Baixo para Tanque de Água Bruta</p><input name ='lt200baixo' id='lt200baixo' type='range' min='20' max='80' value='' oninput='display.value=value' onchange='display.value=value'>
<input type='text' id='display' value='String(lt200Baixo)'' readonly>
<p>Nível Alto para Tanque de Água Bruta</p><input name ='lt200alto' id='lt200alto' type='range' min='20' max='80' value='String(lt200Alto)' oninput='display2.value=value' onchange='display.value=value'>
<input type='text' id='display2' value='String(lt200Alto)' readonly>
<p>Nível Baixo para Tanque de Água Limpa</p>
<input name ='lt205baixo' id='lt205baixo' type='range' min='20' max='80' value='String(lt205Baixo)' oninput='display3.value=value' onchange='display3.value=value'>
<input type='text' id='display3' value='String(lt205Baixo)' readonly>
<p>Nível Alto para Tanque de Água Limpa</p>
<input name ='lt205alto' id='lt205alto' type='range' min='20' max='80' value='String(lt205Alto)' oninput='display4.value=value' onchange='display4.value=value'>
<input type='text' id='display4' value='String(lt205Alto)' readonly></fieldset>
<p><center><input type=submit name=botao1 value=Salvar /></center></form>
<form method='POST' action='/operacao'><p><center><input type=submit name='botao2' value='Operar ETA' /></center></form>
<p><center><a href=/>Pagina Inicial</a></center></p>

<script>
  function loadData(url, callback){
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function(){
      if(this.readyState == 4 && this.status == 200){
        callback.apply(xhttp);
      }
    };
    xhttp.open('GET', url, true);
    xhttp.send();
  }

  var loginADM =loadData('loginInfo',updateDataLoginADM);
  var senhaAdm =loadData('senhaInfo',updateDataSenhaADM);
  var ssidRede =loadData('ssidRede',updateDataSsidRede);
  var senhaRede =loadData('senhaRede',updateDataSenhaRede);
  var ipRede =loadData('ipRede',updateDataIpRede);
  var maskRede =loadData('maskRede',updateDataMaskRede);
  var gatewayRede =loadData('gatewayRede',updateDataGateRede);
  var maskRede =loadData('dnsRede',updateDataDnsRede);
  var bombasOp =loadData('bombaOp',updateDataBombaOp);
  var lt200BaixoOp =loadData('baixo200Op',updateDataLt200BaixoOp);
  var lt200AltoOp =loadData('alto200Op',updateDataLt200AltoOp);
  var lt205BaixoOp =loadData('baixo205Op',updateDataLt205BaixoOp);
  var lt205AltoOp =loadData('alto205Op',updateDataLt205AltoOp);
  

  function updateDataLoginADM(){
    document.getElementById('login').value = this.responseText;
  }

  function updateDataSenhaADM(){
    document.getElementById('authpass').value = this.responseText;
  }

  function updateDataSsidRede(){
    document.getElementById('ssid').value = this.responseText;
  }

  function updateDataSenhaRede(){
    document.getElementById('wifipass').value = this.responseText;
  }
  function updateDataIpRede(){
    document.getElementById('serverip').value = this.responseText;
  }

  function updateDataMaskRede(){
    document.getElementById('servermask').value = this.responseText;
  }
  function updateDataGateRede(){
    document.getElementById('servergate').value = this.responseText;
  }

  function updateDataDnsRede(){
    document.getElementById('serverdns').value = this.responseText;
  }

  function updateDataBombaOp(){
    document.getElementById('bombasconfig').value = this.responseText;
  }
  
  function updateDataLt200BaixoOp(){
    document.getElementById('lt200baixo').value = this.responseText;
    document.getElementById('display').value = this.responseText;
  }

  function updateDataLt200AltoOp(){
    document.getElementById('lt200alto').value = this.responseText;
    document.getElementById('display2').value = this.responseText;
  }

  function updateDataLt205BaixoOp(){
    document.getElementById('lt205baixo').value = this.responseText;
    document.getElementById('display3').value = this.responseText;
  }

  function updateDataLt205AltoOp(){
    document.getElementById('lt205alto').value = this.responseText;
    document.getElementById('display4').value = this.responseText;
  }

  function atualiza(){
    if(document.getElementById('login').value=="" && document.getElementById('login')!=document.activeElement){
      loadData('loginInfo',updateDataLoginADM);
    }
    if(document.getElementById('authpass').value=="" && document.getElementById('authpass')!=document.activeElement){
      loadData('senhaInfo',updateDataSenhaADM);
      }
    if(document.getElementById('ssid').value=="" && document.getElementById('ssid')!=document.activeElement){
      loadData('ssidRede',updateDataSsidRede);
      }
    if(document.getElementById('wifipass').value=="" && document.getElementById('wifipass')!=document.activeElement){
      loadData('senhaRede',updateDataSenhaRede);
      }
    if(document.getElementById('serverip').value=="" && document.getElementById('serverip')!=document.activeElement){
      loadData('ipRede',updateDataIpRede);
      }
    if(document.getElementById('servermask').value=="" && document.getElementById('servermask')!=document.activeElement){
      loadData('maskRede',updateDataMaskRede);
      }
    if(document.getElementById('servergate').value=="" && document.getElementById('servergate')!=document.activeElement){
      loadData('gatewayRede',updateDataGateRede);
      }
    if(document.getElementById('serverdns').value=="" && document.getElementById('serverdns')!=document.activeElement){
      loadData('dnsRede',updateDataDnsRede);
      }
    if(document.getElementById('bombasconfig').value=="" && document.getElementById('bombasconfig')!=document.activeElement){
      loadData('bombaOp',updateDataBombaOp);
      }
    if(document.getElementById('lt200baixo').value=="" && document.getElementById('lt200baixo')!=document.activeElement){
      loadData('baixo200Op',updateDataLt200BaixoOp);
      }
    if(document.getElementById('lt200alto').value=="" && document.getElementById('lt200alto')!=document.activeElement){
      loadData('alto200Op',updateDataLt200AltoOp);
      }
    if(document.getElementById('lt205baixo').value=="" && document.getElementById('lt205baixo')!=document.activeElement){
      loadData('baixo205Op',updateDataLt205BaixoOp);
      }
    if(document.getElementById('lt205alto').value=="" && document.getElementById('lt205alto')!=document.activeElement){
      loadData('alto205Op',updateDataLt205AltoOp);
      }
    }
  var check = setInterval(function() {atualiza()},600)
  
</script>

</html>
)rawTxt"
