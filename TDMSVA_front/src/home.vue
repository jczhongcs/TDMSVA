<template>
    <div class="container">
      <div class="card">
        <div class="cards-container">
          <button class="small-card" @click="goToPage('/r1')">
            <i class="card-icon">A</i>
            页面1
          </button>
          <button class="small-card" @click="goToPage('/r2')">
            <i class="card-icon">B</i>
            页面2
          </button>
          <button class="small-card" @click="goToPage('/r3')">
            <i class="card-icon">C</i>
            页面3
          </button>
        </div>
      </div>
    </div>
  </template>
  
  <script setup>
  import { useRouter } from 'vue-router';
  
  const router = useRouter();
  
  function goToPage(path) {
    router.push(path);
  }
  </script>
  
  <style scoped>
  .container {
    display: flex;
    justify-content: center;
    align-items: center;
    min-height: 100vh;
    margin: 0;
    padding: 0;
    box-sizing: border-box;
  }
  
  .card {
    width: 90%;
    max-width: 600px;
    background-color: rgb(230, 230, 230);
    box-shadow: 2px 6px 6px rgba(0, 0, 0, 0.1);
    border-radius: 10px;
    padding: 20px;
    text-align: center;
  }
  
  .cards-container {
    display: flex;
    justify-content: space-around;
    align-items: center;
    margin-top: 20px;
  }
  
  .small-card {
    background: #f0f0f0;
    border: none;
    border-radius: 5px;
    padding: 15px 30px;
    margin: 0 10px;
    font-size: 16px;
    cursor: pointer;
    transition: background-color 0.3s;
  }
  
  .small-card:hover {
    background-color: rgb(84,137,197);
  }
  
  .card-icon {
    display: block;
    font-style: normal;
    font-size: 24px;
    margin-bottom: 5px;
  }
  </style>