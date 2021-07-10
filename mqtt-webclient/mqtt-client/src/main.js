import Vue from 'vue'
import App from './App.vue'
import VueMqtt from 'vue-mqtt'
import Buefy from 'buefy'
import 'buefy/dist/buefy.css'
import VueRouter from 'vue-router'

Vue.use(VueRouter)

Vue.config.productionTip = false

const mqttoptions = {
  username: 'kelompokwsn',
  password: 'kelompokwsn123'
}

Vue.use(VueMqtt, 'ws://13.94.35.89:9001', mqttoptions);
Vue.use(Buefy)

new Vue({
  render: h => h(App),
}).$mount('#app')
