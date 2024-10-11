<template>
    <div style="display: flex; align-items: center; padding: 10px;" v-for="(inputControl, i) in inputControls" :key="inputControl">
        <div style="margin-right: 20px;">
            Pacakage: 
            <a-input v-model:value="packages[i]" style="width: 240px" placeholder="Please input" />
        </div>

        <div style="margin-right: 20px;">
            Protein :
            <a-input v-model:value="proteins[i]" style="width: 240px" placeholder="Please input" />
        </div>

        <div style="margin-right: 20px;">
            Precursor Mass:
            <a-input v-model:value="mods[i]" style="width: 240px" placeholder="Please input" />
        </div>

        <div style="margin-right: 20px;">
            <a-button type="primary" @click="loadData(i)">Display</a-button>
        </div>
    </div>
    

    <div style="display: flex; height: 90vh;">
        <div style="flex: 1">
            <div ref="chart1" style="height: 80vh;"></div>
        </div>
        <div style="flex: 1;">
            <div ref="chart2" style="height: 80vh;"></div>
        </div>
    </div>
</template>

<script setup>
import * as echarts from 'echarts';
import { onMounted, ref } from 'vue';
import axios from "axios";

const inputControls = ref(['input1', 'input2', 'input3']);
const packages = ref(['VL', 'PLN', 'SOL'])
const proteins = ref(['P04466', 'P04466', 'P08733'])
const mods = ref(['18868.37', '18868.37', '18779.40'])


// 使用 ref 来获取 DOM 元素
const chart1 = ref(null);
const chart2 = ref(null);

const chartOption1 = {
    "title": {
        // "text": "figure1"
    },
    "xAxis": {
        "name": "Retention Time",
        "type": "value",
        nameTextStyle: {
            color: 'black',
            fontSize: 30, // 大小
            padding: [0,0,-67,-550],
            verticalAlign:"bottom",
          },
        axisLabel: {
          inside: false,
          textStyle: {
              color: '#000',
              fontSize:'20',
              itemSize:''

          }
        },
    },
    "yAxis": {
        "name": "P-Total",
        "type": "value",
        nameTextStyle: {
          color: 'black',
          fontSize: 30, // 大小
          padding: [0,0,0,-0],
          verticalAlign:"bottom",
        },
        axisLabel: {
          inside: false,
          textStyle: {
              color: '#000',
              fontSize:'20',
              itemSize:''
              
          }
        },
    },
    "series": [
        {
            "symbolSize": 10,
            "data": [], // 第一组数据
            "type": "scatter",
            "itemStyle": {
                "color": "red" // 设置第一组数据的颜色为红色
            },
            label: {
                show: false,
                formatter: function (params) {
                    const data = params.data[2];
                    return `
                            {a|packageName: ${data.packageName}}\n
                            {b|modTotal: ${data.modTotal}}\n
                            {b|retentionTime: ${data.retentionTime}}\n
                            {b|scans: ${data.scans}}\n
                            {b|noModIntensity: ${data.noModIntensity}}\n
                            {b|ModIntensity: ${data.ModIntensity}}
                        `;
                },
                rich: {
                        a: {
                            fontWeight: 'bold',
                            color: '#333'
                        },
                        b: {
                            color: '#666'
                        }
                    }
            },
            emphasis: {
                    label: {
                        show: true,
                        position: 'top',
                        // backgroundColor: 'rgba(255, 255, 255, 0.8)',
                        // borderColor: '#ccc',
                        borderWidth: 1,
                        borderRadius: 4,
                        padding: 0,
                        shadowBlur: 5,
                        // shadowColor: 'rgba(0, 0, 0, 0.3)'
                    }
            }
        },
        {
            "symbolSize": 10,
            "data": [], // 第二组数据
            "type": "scatter",
            "itemStyle": {
                "color": "purple" // 设置第二组数据的颜色为绿色
            },            label: {
                show: false,
                formatter: function (params) {
                    const data = params.data[2];
                    return `
                            {a|packageName: ${data.packageName}}\n
                            {b|modTotal: ${data.modTotal}}\n
                            {b|retentionTime: ${data.retentionTime}}\n
                            {b|scans: ${data.scans}}\n
                            {b|noModIntensity: ${data.noModIntensity}}\n
                            {b|ModIntensity: ${data.ModIntensity}}
                        `;
                },
                rich: {
                        a: {
                            fontWeight: 'bold',
                            color: '#333'
                        },
                        b: {
                            color: '#666'
                        }
                    }
            },
            emphasis: {
                    label: {
                        show: true,
                        position: 'top',
                        // backgroundColor: 'rgba(255, 255, 255, 0.8)',
                        // borderColor: '#ccc',
                        borderWidth: 1,
                        borderRadius: 4,
                        padding: 0,
                        shadowBlur: 5,
                        // shadowColor: 'rgba(0, 0, 0, 0.3)'
                    }
            }
        },
        {
            "symbolSize": 10,
            "data": [], // 第三组数据
            "type": "scatter",
            "itemStyle": {
                "color": "DeepSkyBlue" // 设置第三组数据的颜色为蓝色
            },
            label: {
                show: false,
                formatter: function (params) {
                    const data = params.data[2];
                    return `
                            {a|packageName: ${data.packageName}}\n
                            {b|modTotal: ${data.modTotal}}\n
                            {b|retentionTime: ${data.retentionTime}}\n
                            {b|scans: ${data.scans}}\n
                            {b|noModIntensity: ${data.noModIntensity}}\n
                            {b|ModIntensity: ${data.ModIntensity}}
                        `;
                },
                rich: {
                        a: {
                            fontWeight: 'bold',
                            color: '#333'
                        },
                        b: {
                            color: '#666'
                        }
                    }
            },
            emphasis: {
                    label: {
                        show: true,
                        position: 'top',
                        // backgroundColor: 'rgba(255, 255, 255, 0.8)',
                        // borderColor: '#ccc',
                        borderWidth: 1,
                        borderRadius: 4,
                        padding: 0,
                        shadowBlur: 5,
                        // shadowColor: 'rgba(0, 0, 0, 0.3)'
                    }
            }
        }
    ]
}
const chartOption2 = {
  title: {
    left: 'left'
  },
  legend: {
    top: '10%'
  },
  dataset: [
    {
      source: [[]]
    },
    {
      source: [[]]
    },
    {
      source: [[]]
    },
    {
      fromDatasetIndex: 0,
      transform: { type: 'boxplot', config: { itemNameFormatter: 'MS' } }
    },
    {
      fromDatasetIndex: 1,
      transform: { type: 'boxplot', config: { itemNameFormatter: 'MS' } }
    },
    {
      fromDatasetIndex: 2,
      transform: { type: 'boxplot', config: { itemNameFormatter: 'MS' } }
    }
  ],
  tooltip: {
    trigger: 'item',
    axisPointer: {
      type: 'shadow'
    }
  },
  xAxis: {
    type: 'category',
    boundaryGap: true,
    splitArea: {
      show: true
    },
    splitLine: {
      show: false
    },
    axisLabel: {
      inside: false,
      textStyle: {
          color: '#000',
          fontSize:'20',
          itemSize:''
      }
    },
  },
  yAxis: {
    type: 'value',
    name: 'P-Total',
    nameTextStyle: {
      color: 'black',
      fontSize: 30, // 大小
      padding: [0,0,0,0],
      verticalAlign:"bottom",
    },
    splitArea: {
      show: false
    },
    axisLabel: {
      inside: false,
      textStyle: {
          color: '#000',
          fontSize:'20',
          itemSize:''
      }
    },
  },
  series: [
    {
      name: 'category0',
      type: 'boxplot',
      datasetIndex: 3,
      itemStyle: {
        borderColor: 'red'
      }
    },
    {
      name: 'category1',
      type: 'boxplot',
      datasetIndex: 4,
      itemStyle: {
        borderColor: 'purple'
      }
    },
    {
      name: 'category2',
      type: 'boxplot',
      datasetIndex: 5,
      itemStyle: {
        borderColor: 'DeepSkyBlue'
      }
    }
  ]
};
const legendAfter = ['\u200B', '\u200B\u200B', '\u200B\u200B\u200B']

const getData = async (type) => {
    // return testData;
    const res = await axios.get('http://localhost:8888/database/boxplot/'+packages.value[type]+'/'+proteins.value[type]+'/'+mods.value[type]);
    if(res.data.mtp_list.length > 0){
        return res.data;
    }else{
        return []
    }
}

const loadData = async (type) => {
    const data = await getData(type);
    // 散点图初始化
    const scatterData = await getScatterData(data);
    chartOption1.series[type].data = scatterData;
    const chartInstance1 = echarts.init(chart1.value);
    chartInstance1.setOption(chartOption1);

    // 箱线图初始化
    const boxplotData = await getBoxplotData(data);
    chartOption2.dataset[type].source[0] = boxplotData;
    chartOption2.series[type].name = packages.value[type] + legendAfter[type];
    console.log("packages", chartOption2);
    const chartInstance2 = echarts.init(chart2.value);
    chartInstance2.setOption(chartOption2);
    
}

const getScatterData = async (data) => {
    const scatterData = data.mtp_list.map(item => {
        return [
            parseFloat(item.retentionTime), 
            parseFloat(item.modTotal),
            item // 这里需要一个闭合的圆括号来完成返回数组的定义
        ];
    });
    return scatterData;
}

const getBoxplotData = async (data) => {
    const modTotalList = data.mtp_list.map(item => {
        return Number(item.modTotal);
    })
    return modTotalList;
    return calculateQuartiles(modTotalList);
}

const calculateQuartiles = (arr) => {
    // 首先对数组进行排序
    const sortedArr = arr.slice().sort((a, b) => a - b);

    // 计算中位数
    const midIndex = Math.floor((sortedArr.length - 1) / 2);
    const median = (sortedArr.length % 2 !== 0) ? sortedArr[midIndex] : (sortedArr[midIndex] + sortedArr[midIndex + 1]) / 2;

    // 计算四分位数
    const Q1 = (sortedArr.length % 2 !== 0) ? sortedArr[Math.floor(sortedArr.length / 4)] : (sortedArr[Math.floor(sortedArr.length / 4)] + sortedArr[Math.floor(sortedArr.length / 4) - 1]) / 2;
    const Q3 = (sortedArr.length % 2 !== 0) ? sortedArr[Math.floor(3 * sortedArr.length / 4)] : (sortedArr[Math.floor(3 * sortedArr.length / 4)] + sortedArr[Math.floor(3 * sortedArr.length / 4) - 1]) / 2;

    // 返回结果
    // return {
    //     min: sortedArr[0],
    //     max: sortedArr[sortedArr.length - 1],
    //     median: median,
    //     Q1: Q1,
    //     Q3: Q3
    // };

    return [ sortedArr[0], sortedArr[sortedArr.length - 1], median, Q1, Q3]
}

onMounted( async () => {
    const chartInstance1 = echarts.init(chart1.value);
    chartInstance1.setOption(chartOption1);

    const chartInstance2 = echarts.init(chart2.value);
    chartInstance2.setOption(chartOption2);
});
</script>


<style>

</style>