<template>
  <div class="container">
    <div class="columns">  
      <div class="column">        
        <b-button type="is-primary" style="margin: 50px" @click="setMakan()">Makan</b-button>
        <b-button type="is-primary" style="margin: 50px" @click="setMinum()">Minum</b-button>
        <b-button type="is-primary" style="margin: 50px" @click="setSuhu()">Suhu</b-button>
        <canvas id="planet-chart"></canvas>
        <!-- {{ this.data.datasets[0].data }} -->    
        <p>{{ this.state }}</p>
        <b-button v-if="sub_topic == 'esp8266/srf_makan'" type="is-primary" class="btn" style="margin: 50px 0 50px 0" @click="pubMakan()">Beri Makan</b-button>
        <b-button v-if="sub_topic == 'esp8266/srf_minum'" type="is-primary" class="btn" style="margin: 50px 0 50px 0" @click="pubMinum()">Beri Minum</b-button>
        <b-button v-if="sub_topic == 'esp8266/dht_suhu'" type="is-primary" class="btn mr-5" style="margin: 50px 0 50px 0" @click="pubSuhuActive()">Aktifkan Lampu</b-button>
        <b-button v-if="sub_topic == 'esp8266/dht_suhu'" type="is-primary" class="btn" style="margin: 50px 0 50px 0" @click="pubSuhuDeactive()">Non Aktifkan Lampu</b-button>
      </div>  
    </div>
  </div>
</template>

<script>
import Chart from 'chart.js'

export default {
  name: 'Index',
  data: function(){
    return{
      connection: null,
      // data_sensor: [1],
      state: "Disconnect",
      pub_topic: null,
      sub_topic: null,
      type: "line",
      data: {
        labels: [],
        datasets: [
          {
            label: "Data Sensor",
            data: [],
            backgroundColor: "rgba(71, 183,132,.5)",
            borderColor: "#47b784",
            borderWidth: 3
          }
        ]
      },
      options: {
        responsive: true,
        lineTension: 1,
        scales: {
          yAxes: [
            {
              ticks: {
                beginAtZero: true,
                padding: 25
              }
            }
          ]
        }
      }
    }
  },
  created: function() {        
  },  
  async mounted() {    
    const date = new Date();
    this.sub_topic = 'esp8266/dht_suhu';
    this.data.datasets[0].label = "Data Sensor " + this.sub_topic + date.toLocaleDateString('en-GB', { timeZone: 'Asia/Jakarta' });        
    this.$mqtt.on('connect', () => {  
      this.$mqtt.subscribe(this.sub_topic, (err) => {
        if(err){
            console.log(err);
        }
        else{
            console.log("success subscribe " + this.sub_topic);
            this.state = "connected " + this.sub_topic;
        }
      });
    });

    const ctx = await document.getElementById('planet-chart');
    const chart = await new Chart(ctx, {
      type: this.type,
      data: this.data,
      options: this.options,
    });
    
    this.$mqtt.on('message', (topic, message) => {      
      var json_message = JSON.parse(message.toString());
      console.log(json_message);      
      const date = new Date();      
      let time_now = date.toLocaleTimeString('en-GB', { timeZone: 'Asia/Jakarta' });      
      if(this.data.datasets[0].data.length >= 13 && this.data.labels.length >= 13){        
        this.data.datasets[0].data.shift();
        this.data.labels.shift();
        this.data.datasets[0].data.push(parseInt(json_message.data_sensor));   
        this.data.labels.push(time_now);
      }      
      else{
        this.data.datasets[0].data.push(parseInt(json_message.data_sensor));   
        this.data.labels.push(time_now);
      }
      chart.update();      
      // if(json_message.data_sensor > 50 && json_message.data_sensor < 80){
      if(this.sub_topic == "esp8266/srf_makan"){
        if(json_message.data_sensor >= 15){
          this.state = "Makan Habis"
        }
        else{
          this.state = "Makan Penuh"
        }
      }   
      else if(this.sub_topic == "esp8266/srf_minum"){
        if(json_message.data_sensor <= 1){
          this.state = "Minum Habis"
        }
        else if(json_message.data_sensor >= 3){
          this.state = "Minum Penuh"
        }
      }         
    });
    
  },
  methods: {
    pubMakan: function () {
    // now we have access to the native event
    this.$mqtt.publish('esp8266/servo_makan', 'servo-active', (err) => {
        if(!err){
          this.state = "Berhasil Memberi Makan";
          alert("Berhasil Memberi Makan");
        }
      })
    },
    pubMinum: function () {    
    this.$mqtt.publish('esp8266/servo_minum', 'servo-active', (err) => {
        if(!err){
          this.state = "Berhasil Memberi Minum";
          alert("Berhasil Memberi Minum");
        }
      })
    },    
    pubSuhuActive: function () {    
    this.$mqtt.publish('esp8266/led_suhu', 'led-active', (err) => {
        if(!err){
          this.state = "Berhasil Menyalakan lampu";
          alert("Berhasil Menyalakan lampu");
        }
      })
    },
    pubSuhuDeactive: function () {    
    this.$mqtt.publish('esp8266/led_suhu', 'led-deactive', (err) => {
        if(!err){
          this.state = "Berhasil Mematikan lampu";
          alert("Berhasil Mematikan lampu");
        }
      })
    },    
  },
}
</script>
<style scoped>
h3 {
  margin: 40px 0 0;
}
ul {
  list-style-type: none;
  padding: 0;
}
li {
  display: inline-block;
  margin: 0 10px;
}
a {
  color: #42b983;
}
.btn{
}
</style>
