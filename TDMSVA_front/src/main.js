import { createApp } from 'vue'
import App from './AppMain.vue'
import axios from 'axios'
import router from './router/router'

const app = createApp(App)
app.use(router)

axios.defaults.baseURL = 'http://localhost:8888'

app.mount('#app')
