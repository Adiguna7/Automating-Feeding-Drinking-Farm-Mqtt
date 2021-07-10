var mqtt = require('mqtt')
const options = {
    username: "kelompokwsn",
    password: "kelompokwsn123"
}
var client  = mqtt.connect('mqtt://localhost:1883', options)

client.on('connect', function () {  
    client.subscribe('esp8266/srf_minum', (err) => {
        if(err){
            console.log(err);
        }
    });
});

client.on('message', function (topic, message) {
    // message is Buffer
    var json_message = JSON.parse(message.toString());
    console.log(json_message);
    // client.end()
})