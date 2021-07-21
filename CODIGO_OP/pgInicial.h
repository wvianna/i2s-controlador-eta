R"rawTxt(
<!DOCTYPE html>
<html>
    <head>
        <title>Esta&ccedil;&atilde;o de Tratamento de &Aacute;gua</title>
    </head>
    <meta http-equiv='Content-Type' content='text/html; charset='utf-8' />
    <body> 
        <h1><center>PICG - I2S</center></h1>
        <h2>ESTA&Ccedil;&Atilde;O DE TRATAMENTO DE &Aacute;GUA:</h2>
        <fieldset>
            <legend><b>Informa&ccedil;&otilde;es:</b></legend>
            <p id='data'>Data: </p>
            <p id='hora'>Hora: </p>
        </fieldset>
        <fieldset><legend><b>Monitoramento: </b></legend>
        <table>
            <tr><td Width=200>N&iacute;vel da &Aacute;gua Captada (LT-200): </td><td Align=Middle width=100 id='valuelt200'>Carregando</td></tr>
            <tr><td Width=200>N&iacute;vel da &Aacute;gua Tratada (LT-205):</td><td Align=Middle width=100 id='valuelt205'>Carregando</td></tr>
            <tr><td Width=200>Captando &Aacute;gua:</td><td Align=Middle width=300 id='statusCaptando'>Carregando</td></tr>
            <tr><td Width=200>Tratando &Aacute;gua:</td><td Align=Middle width=100 id='statusProcessando'>Carregando</td></tr>
            <tr><td Width=300>Abastecendo Bloco A:</td><td Align=Middle width=100 id='statusDistribuindo'>Carregando</td></tr>
            <tr><td Width=300>Estado de Opera&ccedil;&atilde;o:</td><td Align=Middle width=100 id='statusAutoMan'>Carregando</td></tr></fieldset>
        </table>
    <script>
        var z = setInterval(function() {loadData2('lt200',updateData2)},2504);
        function loadData2(url, callback){
            var zhttp = new XMLHttpRequest();
            zhttp.onreadystatechange = function(){
            if(this.readyState == 4 && this.status == 200){
                callback.apply(zhttp);}
            };
            zhttp.open('GET', url, true);
            zhttp.send();
        }
        function updateData2(){
            document.getElementById('valuelt200').innerHTML = this.responseText;
        }

        var x = setInterval(function() {loadData3('lt205',updateData3)},2704);
        function loadData3(url, callback){
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function(){
                if(this.readyState == 4 && this.status == 200){
                    callback.apply(xhttp);
                }
            };
        xhttp.open('GET', url, true);
        xhttp.send();
        }
        function updateData3(){
            document.getElementById('valuelt205').innerHTML = this.responseText;
        }

        var q = setInterval(function() {loadData4('statusCaptacao',updateData4)},2104);
        function loadData4(url, callback){
            var qhttp = new XMLHttpRequest();
            qhttp.onreadystatechange = function(){
                if(this.readyState == 4 && this.status == 200){
                    callback.apply(qhttp);
                }
            };
        qhttp.open('GET', url, true);
        qhttp.send();
        }
        function updateData4(){
            document.getElementById('statusCaptando').innerHTML = this.responseText;
        }

        var w = setInterval(function() {loadData5('statusProcesso',updateData5)},2004);
        function loadData5(url, callback){
            var whttp = new XMLHttpRequest();
            whttp.onreadystatechange = function(){
                if(this.readyState == 4 && this.status == 200){
                    callback.apply(whttp);
                }
            };
        whttp.open('GET', url, true);
        whttp.send();
        }
        function updateData5(){
            document.getElementById('statusProcessando').innerHTML = this.responseText;
        }

        var e = setInterval(function() {loadData6('statusDistribuicao',updateData6)},2002);
        function loadData6(url, callback){
            var ehttp = new XMLHttpRequest();
            ehttp.onreadystatechange = function(){
                if(this.readyState == 4 && this.status == 200){
                    callback.apply(ehttp);
                }
            };
        ehttp.open('GET', url, true);
        ehttp.send();
        }
        function updateData6(){
            document.getElementById('statusDistribuindo').innerHTML = this.responseText;
        }

        var k = setInterval(function() {loadData7('statusAutoManual',updateData7)},3001);
        function loadData7(url, callback){
            var khttp = new XMLHttpRequest();
            khttp.onreadystatechange = function(){
                if(this.readyState == 4 && this.status == 200){
                    callback.apply(khttp);
                }
            };
        khttp.open('GET', url, true);
        khttp.send();
        }
        function updateData7(){
            document.getElementById('statusAutoMan').innerHTML = this.responseText;
        }
    function updateDayTime(){
        var d = new Date();
        document.getElementById('data').innerHTML = "Data: "+d.getDate()+"/"+(d.getMonth()+1)+"/"+d.getFullYear();
    
        var h = new Date();
        document.getElementById('hora').innerHTML = "Hora: "+h.getHours()+":"+h.getMinutes()+":"+h.getSeconds();
        setTimeout(updateDayTime, 1000);
    }
    updateDayTime();
    </script>


    <p><fieldset>
        <legend><b>Para acessar a p&aacute;gina de configura&ccedil;&otilde;es, fa&ccedil;a login:</b></legend>
        <form method='POST' action='/login'>
            <p>Usuario:<input type=text size=10  name=login value=admin /></p>
            <p>Senha:<input type=password size=10 name=senha /></p>
            <p><input type=submit name=enviar value=Enviar /></p>
        </form>
    </fieldset></p>
    </body>
</html>
)rawTxt"
