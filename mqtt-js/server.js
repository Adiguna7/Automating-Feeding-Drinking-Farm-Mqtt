var mqtt = require('mqtt')
var client  = mqtt.connect('mqtt://localhost:1883')
 
client.on('connect', function () {      
    setInterval(pub, 1000);
})

var counter = 0;
let pub = () => {
    var message = "hello mqtt";    
    client.publish('presence', message, (err) => {
        if(!err){
            console.log("sucess send " + message + " " + counter);
            counter += 1;
        }
    });
}