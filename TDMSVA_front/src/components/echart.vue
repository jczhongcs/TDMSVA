<template>
  <div class="chartbox">
    <div class="chart" ref="chart"></div>
  </div>
</template>

<script setup>
import { ref, onMounted } from "vue";
import * as echarts from "echarts";

// 图表必须要指定固定的高度和宽度
const chart = ref(null);
var myChart = null;
var option = {
  legend: {},
  tooltip: {},
  dataset: {
    // 提供一份数据。
    source: [
      ["product", "2015", "2016", "2017"],
      ["Matcha Latte", 43.3, 85.8, 93.7],
      ["Milk Tea", 83.1, 73.4, 55.1],
      ["Cheese Cocoa", 86.4, 65.2, 82.5],
      ["Walnut Brownie", 72.4, 53.9, 39.1],
    ],
  },
  // 声明一个 X 轴，类目轴（category）。默认情况下，类目轴对应到 dataset 第一列。
  xAxis: { type: "category" },
  // 声明一个 Y 轴，数值轴。
  yAxis: { type: "value" },
  // 系列，这是一个数组，其中的不同的对象里面表示不同的数据集
  // 可以参考这个 https://echarts.apache.org/handbook/zh/concepts/dataset
  // 声明多个 bar 系列，默认情况下，每个系列会自动对应到 dataset 的每一列。
  series: [{ type: "bar" }, { type: "bar" }, { type: "bar" }],
};

onMounted(() => {
  // 基于准备好的 dom，初始化 echarts 实例
  myChart = echarts.init(chart.value);
  myChart.setOption(option);
});
</script>

<style scoped>
.chartbox {
  padding: 20px;
  /* display: flex; */
  /*水平居中*/
  /* justify-content: center; */
  /*垂直居中*/
  /* align-items: start; */
  /* height: 100vh; */
}
.chart {
  width: 800px;
  height: 400px;
}
</style>
