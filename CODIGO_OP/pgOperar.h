R"rawTxt(
<!DOCTYPE html>
 <html><head><title>Estação de Tratamento de Água</title></head>
 <h1><center>Operação da ETA</center></h1>
 <fieldset>

<p><form method='POST' action='/auto-man'>Automático/Manual:
<input type="hidden" id='autoManr' value=""></input>
<input type="hidden" id='remotoLocal' value=""></input>
<div id='statusAutoMan'></div></form></p>

<p><form method='POST' action='/MB-500'>Bomba de Captação 1(MB-500):
<input type="hidden" id='hs500r' value=""></input>
<div id='statusMB500'></div></form></p>

<p><form method='POST' action='/MB-501'>Bomba de Captação 2(MB-501):
<input type="hidden" id='hs501r' value=""></input>
<div id='statusMB501'></div></form></p>

<p><form method='POST' action='/XV-502'>Válvula solenoide de Saída de Água Bruta(XV-502):
<input type="hidden" id='hs502r' value=""></input>
<div id='statusXV502'></div></form></p>

<p><form method='POST' action='/MB-504'>Bombas Peristálticas(MB-504):
<input type="hidden" id='hs504r' value=""></input>
<div id='statusMB504'></div></form></p>

<p><form method='POST' action='/MB-505'>Motor do Agitador para Floculação(MB-505):
<input type="hidden" id='hs505r' value=""></input>
<div id='statusMB505'></div></form></p>

<p><form method='POST' action='/XV-503'>Válvula solenoide de Saída de Água Suja(XV-503):
<input type="hidden" id='hs503r' value=""></input>
<div id='statusXV503'></div></form></p>

<p><form method='POST' action='/MB-507'>Bomba para Filtragem de Água em Tratamento(MB-507):
<input type="hidden" id='hs507r' value=""></input>
<div id='statusMB507'></div></form></p>

<p><form method='POST' action='/MB-506'>Bomba de Descarte de Água Suja(MB-506):
<input type="hidden" id='hs506r' value=""></input>
<div id='statusMB506'></div></form></p>

<p><form method='POST' action='/MB-508'>Bomba para Filtragem de Água em Tratamento(MB-508):
<input type="hidden" id='hs508r' value=""></input>
<div id='statusMB508'></div></form></p>
<i>Dependente de ter volume suficiente no Tanque de Água Limpa</i>

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

var caregaMb500 = loadData('estadoMb500',updateDataMb500);
var caregaMb501 = loadData('estadoMb501',updateDataMb501);
var caregaXv502 = loadData('estadoXv502',updateDataXv502);
var caregaMb504 = loadData('estadoMb504',updateDataMb504);
var caregaMb505 = loadData('estadoMb505',updateDataMb505);
var caregaXv503 = loadData('estadoXv503',updateDataXv503);
var caregaMb507 = loadData('estadoMb507',updateDataMb507);
var caregaMb506 = loadData('estadoMb506',updateDataMb506);
var caregaMb508 = loadData('estadoMb508',updateDataMb508);
var caregaAutoMan = loadData('estadoAutoMan',updateAutoMan);
var caregaRemotoLocal = loadData('estadoRemotoLocal',updateRemotoLocal);

function updateAutoMan(){
    document.getElementById('autoManr').value = this.responseText;
    atualizaTextoAutoMan();
}

function updateRemotoLocal(){
    document.getElementById('remotoLocal').value = this.responseText;
    atualizaTextoAutoMan();
    atualizaTextoRemotoLocal();
}

function updateDataMb500(){
    document.getElementById('hs500r').value = this.responseText;
    atualizaTextoMb500();
}

function updateDataMb501(){
    document.getElementById('hs501r').value = this.responseText;
    atualizaTextoMb501();
}

function updateDataXv502(){
    document.getElementById('hs502r').value = this.responseText;
    atualizaTextoXv502();
}

function updateDataMb504(){
    document.getElementById('hs504r').value = this.responseText;
    atualizaTextoMb504();
}

function updateDataMb505(){
    document.getElementById('hs505r').value = this.responseText;
    atualizaTextoMb505();
}

function updateDataXv503(){
    document.getElementById('hs503r').value = this.responseText;
    atualizaTextoXv503();
}

function updateDataMb507(){
    document.getElementById('hs507r').value = this.responseText;
    atualizaTextoMb507();
}

function updateDataMb506(){
    document.getElementById('hs506r').value = this.responseText;
    atualizaTextoMb506();
}

function updateDataMb508(){
    document.getElementById('hs508r').value = this.responseText;
    atualizaTextoMb508();
}


function atualizaTextoAutoMan(){
    if (document.getElementById('autoManr').value =="0"){
        document.getElementById('statusAutoMan').innerHTML="<b>Manual </b><input name='autoMan' id='autoMan' type='submit' value='Automático' />";
    }
    else if(document.getElementById('autoManr').value =="1"){
        document.getElementById('statusAutoMan').innerHTML="<b>Automático </b><input name='autoMan' id='autoMan' type='submit' value='Manual' />";
        document.getElementById('statusMB500').innerHTML="<b>As operações não estão disponíveis porque o controlador está em modo Automático</b>";
        document.getElementById('statusMB501').innerHTML="";
        document.getElementById('statusXV502').innerHTML="";
        document.getElementById('statusMB504').innerHTML="";
        document.getElementById('statusMB505').innerHTML="";
        document.getElementById('statusXV503').innerHTML="";
        document.getElementById('statusMB507').innerHTML="";
        document.getElementById('statusMB506').innerHTML="";
        document.getElementById('statusMB508').innerHTML="";
    }
    else{
        loadData('estadoAutoMan',updateAutoMan);     
    }
}

function atualizaTextoMb500(){
    if (document.getElementById('hs500r').value =="0"){
        document.getElementById('statusMB500').innerHTML="<b>Desligada </b><input name='HS-500' id='HS-500' type='submit' value='Ligar' />";
    }
    else if(document.getElementById('hs500r').value =="1"){
        document.getElementById('statusMB500').innerHTML="<b>Ligada </b><input name='HS-500' id='HS-500' type='submit' value='Desligar' />";
    }
    else{
        loadData('estadoMb500',updateDataMb500);
    }
}

function atualizaTextoMb501(){
    if (document.getElementById('hs501r').value =="0"){
        document.getElementById('statusMB501').innerHTML="<b>Desligada </b><input name='HS-501' id='HS-501' type='submit' value='Ligar' />";
    }
    else if(document.getElementById('hs501r').value =="1"){
        document.getElementById('statusMB501').innerHTML="<b>Ligada </b><input name='HS-501' id='HS-501' type='submit' value='Desligar' />";
    }
    else{
        loadData('estadoMb501',updateDataMb501);        
    }
}

function atualizaTextoXv502(){
    if (document.getElementById('hs502r').value =="0"){
        document.getElementById('statusXV502').innerHTML="<b>Desligada </b><input name='HS-502' id='HS-502' type='submit' value='Ligar' />";
    }
    else if(document.getElementById('hs502r').value =="1"){
        document.getElementById('statusXV502').innerHTML="<b>Ligada </b><input name='HS-502' id='HS-502' type='submit' value='Desligar' />";
    }
    else{
        loadData('estadoXv502',updateDataXv502);     
    }
}
function atualizaTextoMb504(){
    if (document.getElementById('hs504r').value =="0"){
        document.getElementById('statusMB504').innerHTML="<b>Desligada </b><input name='HS-504' id='HS-504' type='submit' value='Ligar' />";
    }
    else if(document.getElementById('hs504r').value =="1"){
        document.getElementById('statusMB504').innerHTML="<b>Ligada </b><input name='HS-504' id='HS-504' type='submit' value='Desligar' />";
    }
    else{
        loadData('estadoMb504',updateDataMb504);     
    }
}
function atualizaTextoMb505(){
    if (document.getElementById('hs505r').value =="0"){
        document.getElementById('statusMB505').innerHTML="<b>Desligada </b><input name='HS-505' id='HS-505' type='submit' value='Ligar' />";
    }
    else if(document.getElementById('hs505r').value =="1"){
        document.getElementById('statusMB505').innerHTML="<b>Ligada </b><input name='HS-505' id='HS-505' type='submit' value='Desligar' />";
    }
    else{
        loadData('estadoMb505',updateDataMb505);     
    }
}

function atualizaTextoXv503(){
    if (document.getElementById('hs503r').value =="0"){
        document.getElementById('statusXV503').innerHTML="<b>Desligada </b><input name='HS-503' id='HS-503' type='submit' value='Ligar' />";
    }
    else if(document.getElementById('hs503r').value =="1"){
        document.getElementById('statusXV503').innerHTML="<b>Ligada </b><input name='HS-503' id='HS-503' type='submit' value='Desligar' />";
    }
    else{
        loadData('estadoXv503',updateDataXv503);     
    }
}

function atualizaTextoMb507(){
    if (document.getElementById('hs507r').value =="0"){
        document.getElementById('statusMB507').innerHTML="<b>Desligada </b><input name='HS-507' id='HS-507' type='submit' value='Ligar' />";
    }
    else if(document.getElementById('hs507r').value =="1"){
        document.getElementById('statusMB507').innerHTML="<b>Ligada </b><input name='HS-507' id='HS-507' type='submit' value='Desligar' />";
    }
    else{
        loadData('estadoMb507',updateDataMb507);     
    }
}

function atualizaTextoMb506(){
    if (document.getElementById('hs506r').value =="0"){
        document.getElementById('statusMB506').innerHTML="<b>Desligada </b><input name='HS-506' id='HS-506' type='submit' value='Ligar' />";
    }
    else if(document.getElementById('hs506r').value =="1"){
        document.getElementById('statusMB506').innerHTML="<b>Ligada </b><input name='HS-506' id='HS-506' type='submit' value='Desligar' />";
    }
    else{
        loadData('estadoMb506',updateDataMb506);     
    }
}

function atualizaTextoMb508(){
    if (document.getElementById('hs508r').value =="0"){
        document.getElementById('statusMB508').innerHTML="<b>Desligada </b><input name='HS-508' id='HS-508' type='submit' value='Ligar' />";
    }
    else if(document.getElementById('hs508r').value =="1"){
        document.getElementById('statusMB508').innerHTML="<b>Ligada </b><input name='HS-508' id='HS-508' type='submit' value='Desligar' />";
    }
    else{
        loadData('estadoMb508',updateDataMb508);     
    }
}

function atualizaRemotoLocal(){
    if (document.getElementById('remotoLocal').value =="0"){
        document.getElementById('statusMB500').innerHTML="<b>As operações não estão disponíveis porque o controlador está em modo Local</b>";
        document.getElementById('statusMB501').innerHTML="";
        document.getElementById('statusXV502').innerHTML="";
        document.getElementById('statusMB504').innerHTML="";
        document.getElementById('statusMB505').innerHTML="";
        document.getElementById('statusXV503').innerHTML="";
        document.getElementById('statusMB507').innerHTML="";
        document.getElementById('statusMB506').innerHTML="";
        document.getElementById('statusMB508').innerHTML="";  
    }
}


}

</script>
<script>  
    
</script>

</script>
  <p><center><a href=/>Pagina Inicial</a></center></p>
  </html>
)rawTxt"
