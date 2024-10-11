<template>
  <n-layout>
    <n-layout-header bordered>
      <n-input-group>
        <n-input-group-label> 肽段序列 </n-input-group-label>
        <n-input
          style="min-width: 30%"
          v-model:value="peptide"
          autosize
          size="small"
        />
        <n-button @click="peptideChanged"> 计算 </n-button>
        <n-input-group-label>
          肽段质量： {{ peptide_mass }}
        </n-input-group-label>
      </n-input-group>

      <n-input-group>
        <n-input-group-label> 变体序列 </n-input-group-label>
        <n-input
          style="min-width: 30%"
          v-model:value="proteform"
          autosize
          size="small"
        />
        <n-popover trigger="hover">
          <template #trigger>
            <n-button @click="proteoformChanged"> 计算 </n-button>
          </template>
          <span>请确保两侧存在正确的截断</span>
        </n-popover>

        <n-input-group-label>
          变体质量： {{ proteoform_mass }}
        </n-input-group-label>
      </n-input-group>
    </n-layout-header>

    <!-- 改变图形 -->
    <!-- <n-space justify="center">
      <n-radio-group v-model:value="value">
        <n-radio value="bar" @click="changeChartType">直线图</n-radio>
        <n-radio value="line" @click="changeChartType">折线图</n-radio>
      </n-radio-group>
    </n-space> -->

    <n-layout-content>
      <!-- 组合式 API，引用将存储在与名字匹配的 ref 里 -->
      <div class="chart" ref="chart"></div>
    </n-layout-content>
  </n-layout>
</template>

<script setup>
import * as ms from "./ms/ms";
import * as echarts from "echarts";
import { ms2data } from "./ms/data";
import { ref, onMounted } from "vue";

const chart = ref(null);
var myChart = null;
// 设置 x 轴最小长度，确保图表显示正确
// 这里先固定为质谱数据中 mass 这最大值，根据实际更改
const xAxis_min = ms2data[ms2data.length - 1][0];

// var value = "bar"

var differTwoIons = 0.0;

const peptide = ref("LKDLEALLNSK");
const proteform = ref("");

var peptide_mass = ref(ms.PeptideMass);
var proteoform_mass = ref(ms.PeptideMass);

// 图表配置
var option = {
  title: {
    text: "BYIons vs ALL PEAKs_",
    left: "center",
  },
  tooltip: {
    trigger: "axis",
    axisPointer: {
      animation: true,
    },
  },
  // 标签
  legend: {
    data: ["MS2Trail", "B", "B+", "Y", "Y+"],
    left: "6%",
  },
  toolbox: {
    feature: {
      dataZoom: {
        yAxisIndex: "none",
      },
      restore: {},
      saveAsImage: {},
    },
  },
  axisPointer: {
    link: [
      {
        xAxisIndex: "all",
      },
    ],
  },
  dataZoom: [
    {
      show: true,
      realtime: true,
      start: 0,
      end: 100,
      xAxisIndex: [0, 1],
    },
    {
      type: "inside",
      realtime: true,
      start: 30,
      end: 70,
      xAxisIndex: [0, 1],
    },
  ],
  grid: [
    {
      left: 80,
      right: 50,
      top: "12%",
      height: "45%",
    },
    {
      left: 80,
      right: 50,
      top: "57%",
      height: "30%",
    },
  ],
  xAxis: [
    {
      position: "top",
      type: "value",
      min: 0,
      max: 100, //2103.8067367,
      //   type: 'category',
      boundaryGap: false,
      axisPointer: {
        label: {
          formatter: function (params) {
            if (
              typeof params.seriesData[0] !== "undefined" &&
              typeof params.seriesData[0].data !== "undefined"
            ) {
              differTwoIons = params.seriesData[0].data[0];
              return (
                "mass  " +
                params.value +
                (typeof params.seriesData[0].data !== "undefined"
                  ? " NO.：" + params.seriesData[0].data[2]
                  : "")
              );
            }
          },
        },
      },
      axisLine: { onZero: true }, //,
      //   data: timeData
    },
    {
      gridIndex: 1,
      //     type: 'category',
      type: "value",
      min: 0,
      max: 100, //2103.8067367,
      boundaryGap: false,
      axisPointer: {
        label: {
          formatter: function (params) {
            return (
              "mass  " +
              params.value +
              "\n" +
              "diff:" +
              Math.abs(params.value - differTwoIons) +
              ((Math.abs(params.value - differTwoIons) * 1000000) /
                params.value <
              10
                ? " Matched"
                : "NoMatch")
            );
          },
        },
      },
      axisLine: { onZero: true }, //,

      //      data: timeData
    },
  ],
  yAxis: [
    {
      // name: 'MS2Trail',
      type: "value",
      nameTextStyle: {
        padding: [0, 70, 0, 0],
      },
    },
    {
      gridIndex: 1,
      // name: 'BYIons',
      type: "value",
      inverse: true, // 一个反转的 Y 轴
      max: 120,
      nameTextStyle: {
        padding: [0, 70, 0, 0],
      },
    },
  ],
  series: [
    // ms2 数据
    {
      name: "MS2Trail",
      type: "bar",
      symbolSize: 8,
      barWidth: "1px",
      itemStyle: {
        color: "#0001ff",
      },
      // label: {
      //   show: true,
      //   color: '#323232',
      //   position: 'top'
      // },
      data: ms2data,
    },
    // B 离子数据
    {
      name: "B",
      type: "bar",
      xAxisIndex: 1,
      yAxisIndex: 1,
      // symbolSize: 8,
      barWidth: "1px",
      label: {
        show: true,
        color: "#323232",
        position: "bottom",
        // formatter: '{a|{a}  \n{@[2]}}', //\n{c}',
        //
        //  function(e){
        //   let data=e.data;
        //   return '{a}${data.value}:+{a|这段文本采用样式a}';
        // },
        formatter: function (d) {
          return formatlab(d);
        },
        rich: {
          a: {
            align: "center",
            color: "blue",
            lineHeight: 10,
          },
          b: {
            backgroundColor: {
              image: "xxx/xxx.jpg",
            },
            height: 40,
          },
          x: {
            // fontSize: 18,
            // fontFamily: 'Microsoft YaHei',
            align: "center",
            borderColor: "#449933",
            // ,
            // borderWidth: 1,
            // borderRadius: 1
          },
        },
      },
      itemStyle: {
        color: "#49a1ff",
      },
      data: [],
    },
    // B+ 离子数据
    {
      name: "B+",
      type: "bar",
      xAxisIndex: 1,
      yAxisIndex: 1,
      barWidth: "1px",

      itemStyle: {
        color: "#B71375",
      },
      label: {
        show: true,
        color: "#323232",
        position: "bottom",
        formatter: function (d) {
          return formatlabC2(d);
        },
        rich: {
          a: {
            align: "center",
            color: "red",
            lineHeight: 10,
          },
          b: {
            backgroundColor: {
              image: "xxx/xxx.jpg",
            },
            height: 40,
          },
          x: {
            align: "center",
            borderColor: "#449933",
          },
        },
      },
      data: [],
    },
    // Y 离子数据
    {
      name: "Y",
      type: "bar",
      xAxisIndex: 1,
      yAxisIndex: 1,
      // symbolSize: 8,
      barWidth: "1px",
      itemStyle: {
        color: "#4ff0a1",
      },
      label: {
        show: true,
        color: "#323232",
        position: "bottom",
        // formatter: '{a|{a}  \n{@[2]}}', //\n{c}',
        //
        //  function(e){
        //   let data=e.data;
        //   return '{a}${data.value}:+{a|这段文本采用样式a}';
        // },

        formatter: function (d) {
          // console.log(d);
          return formatlab(d);
        },
        rich: {
          a: {
            align: "center",
            color: "red",
            lineHeight: 10,
          },
          b: {
            backgroundColor: {
              image: "xxx/xxx.jpg",
            },
            height: 40,
          },
          x: {
            // fontSize: 18,
            // fontFamily: 'Microsoft YaHei',
            align: "center",

            borderColor: "#449933",
            // ,
            // borderWidth: 1,

            // borderRadius: 1
          },
        },
      },
      data: [],
    },
    // Y+ 离子数据
    {
      name: "Y+",
      type: "bar",
      xAxisIndex: 1,
      yAxisIndex: 1,
      // symbolSize: 8,
      barWidth: "1px",
      itemStyle: {
        color: "#ff8f00",
      },
      label: {
        show: true,
        color: "#323232",
        position: "bottom",
        // formatter: '{a|{a}  \n{@[2]}}', //\n{c}',
        //
        //  function(e){
        //   let data=e.data;
        //   return '{a}${data.value}:+{a|这段文本采用样式a}';
        // },

        formatter: function (d) {
          // console.log(d);
          return formatlabC2(d);
        },

        rich: {
          a: {
            align: "center",
            color: "blue",
            lineHeight: 10,
          },
          b: {
            backgroundColor: {
              image: "xxx/xxx.jpg",
            },
            height: 40,
          },
          x: {
            // fontSize: 18,
            // fontFamily: 'Microsoft YaHei',
            align: "center",

            borderColor: "#449933",
            // ,
            // borderWidth: 1,
            // borderRadius: 1
          },
        },
      },
      // prettier-ignore
      //
      data: [],
    },
  ],
};

onMounted(() => {
  console.log(2);
  // 基于准备好的dom，初始化echarts实例
  myChart = echarts.init(chart.value);
  // 绘制图表
  setDisplay(peptide.value, false);
});

// 计算一般肽段
function peptideChanged() {
  peptide_mass.value = setDisplay(peptide.value, false);
}

// 计算变体肽段
function proteoformChanged() {
  proteoform_mass.value = setDisplay(proteform.value, true);
}

function setDisplay(val, isProteoform) {
  var mass_arr = ms.calculatePeptide(val, isProteoform);

  var len = mass_arr.length;

  // 不同电荷数
  var bionsMasses = new Array(len);
  var b2ChargeionsMasses = new Array(len);
  var yionsMasses = new Array(len);
  var y2ChargeionsMasses = new Array(len);

  var peptideMasssum = ms.PeptideMass;
  var partial_y = ms.H2OMass + ms.ProtonMass;
  var partial_b = ms.ProtonMass;

  for (var i = 0; i < len - 1; i++) {
    peptideMasssum += mass_arr[i];
    partial_y += mass_arr[len - i - 1];
    partial_b += mass_arr[i];

    bionsMasses[i] = new Array(3);
    bionsMasses[i][0] = partial_b;
    bionsMasses[i][1] = 100;
    bionsMasses[i][2] = i + 1;

    b2ChargeionsMasses[i] = new Array(3);
    b2ChargeionsMasses[i][0] = (partial_b + ms.ProtonMass) / 2;
    b2ChargeionsMasses[i][1] = 90;
    b2ChargeionsMasses[i][2] = i + 1;

    yionsMasses[i] = new Array(3);
    yionsMasses[i][0] = partial_y;
    yionsMasses[i][1] = 100;
    yionsMasses[i][2] = i + 1;

    y2ChargeionsMasses[i] = new Array(3);
    y2ChargeionsMasses[i][0] = (partial_y + ms.ProtonMass) / 2;
    y2ChargeionsMasses[i][1] = 90;
    y2ChargeionsMasses[i][2] = i + 1;
  }

  if (option && typeof option === "object") {
    option.title.text = "ALL PEAKs & BYIons";

    // 由于 x 轴的最大值跟随 partial_y 轴变化
    // partial_y 又取决于肽段的长度，所有需要设定一个最小值
    option.xAxis[0].max = Math.max(partial_y, xAxis_min) + 50;
    option.xAxis[1].max = Math.max(partial_y, xAxis_min) + 50;

    option.series[1].data = bionsMasses;
    option.series[2].data = b2ChargeionsMasses;
    option.series[3].data = yionsMasses;
    option.series[4].data = y2ChargeionsMasses;

    myChart.setOption(option);

    return peptideMasssum;
  }
}

function formatlabC2(d) {
  var match_h2o = isMatched(d.data[0] - 18.0105647 / 2);
  var match_nh3 = isMatched(d.data[0] - 17.026549105 / 2);
  var match_h = isMatched(d.data[0] - 1.007825035 / 2);

  return (
    "{a|" +
    d.seriesName +
    " \n" +
    d.data[2] +
    "}{x|" +
    (isMatched(d.data[0]) > 0 ? "\nMatch" : "") +
    (match_h2o > 0 ? "\nM-H2O pos:" + (match_h2o + 1) : "") +
    // +((match_h>0)?('\nM-H pos:'+(match_h+1)):'')
    (match_nh3 > 0 ? "\nM-NH3 pos:" + (match_nh3 + 1) : "") +
    "}"
  );
}

function formatlab(d) {
  var match_h2o = isMatched(d.data[0] - 18.0105647);
  var match_h = isMatched(d.data[0] - 1.007825035);
  var match_nh3 = isMatched(d.data[0] - 17.026549105);

  return (
    "{a|" +
    d.seriesName +
    " \n" +
    d.data[2] +
    "}{x|" +
    (isMatched(d.data[0]) > 0 ? "\nMatch" : "") +
    (match_h2o > 0 ? "\nM-H2O pos:" + (match_h2o + 1) : "") +
    // +((match_h>0)?('\nM-H pos:'+(match_h+1)):'')
    (match_nh3 > 0 ? "\nM-NH3 pos:" + (match_nh3 + 1) : "") +
    "}"
  );
}

function isMatched(dataMass) {
  var dinteralDiffThreshold = (dataMass * 10) / 1000000;
  var bMatched = iterativeFunction(ms2data, dataMass, dinteralDiffThreshold);
  return bMatched;
}

function iterativeFunction(arr, x, interalvalue) {
  var start = 0,
    end = arr.length - 1;
  // Iterate while start not meets end
  while (start <= end) {
    // Find the mid index
    var mid = Math.floor((start + end) / 2);

    // serial data is 3 array, If element is present at mid, return True
    // if (arr[0][mid]===x) return mid;
    if (Math.abs(arr[mid][0] - x) < interalvalue) return mid;
    // Else look in left or right half accordingly
    else if (arr[mid][0] < x) start = mid + 1;
    else end = mid - 1;
  }

  return -start;
}

</script>

<style scoped>
.chart {
  width: 80%;
  height: 600px;
  margin: 0 auto;
}

.n-layout-header,
.n-layout-content {
  padding: 10px;
}
</style>
