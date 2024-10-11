import { createRouter, createWebHistory } from 'vue-router';
import R1 from '../App.vue';
import R2 from '../R2.vue';
import Home from '../home.vue';
import R3 from '../R3.vue';
import R4 from '../R4.vue';

const router = createRouter({
  history: createWebHistory(),
  routes: [
    {
      path: '/',
      name: 'Home',
      component: Home
    },
    {
      path: '/r2',
      name: 'r2',
      component: R2
    },
    {
      path: '/r1',
      name: 'r1',
      component: R1
    },
    {
      path: '/r3',
      name: 'r3',
      component: R3
    },
    {
      path: '/r4',
      name: 'r4',
      component: R4
    }
  ]
});

export default router;