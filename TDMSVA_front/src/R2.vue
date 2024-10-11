<template>
    <div class="container">
      <div class="left">
        <div class="left-top">
          <a-space direction="vertical" size="middle">
            <a-button type="primary" @click="showModal">
              <template #icon>
                <SettingOutlined />
              </template>
              Connect database
            </a-button>
            <a-modal title="Connect Database" ok-text="OK" cancel-text="Cancel" v-model:visible="visible_loading_database"
              :confirm-loading="confirmLoading" @ok="get_package_list">
              <a-input addon-before="IP" style="padding-bottom: 20px" v-model:value="p_database_ip" />
              <a-input addon-before="Database" v-model:value="p_database_name" />
            </a-modal>
          </a-space>

          <div style="display: flex; align-items: center; padding: 10px;margin-top: 20px;">
            <div style="margin-right: 20px;">
                Pacakage : 
                <a-input v-model:value="packageName" style="width: 120px" placeholder="Please input" />
            </div>

            <div style="margin-right: 20px;">
                Protein : 
                <a-input v-model:value="proteinName" style="width: 120px" placeholder="Please input" />
            </div>
            <div style="margin-right: 20px;">
              Precursor Mass: 
                <a-input v-model:value="proteinMass" style="width: 120px" placeholder="Please input" />
            </div>



            <div style="margin-right: 20px;margin-top: 0px;">
                <a-button type="primary" @click="getData">Display</a-button>
            </div>

          </div>
            


          <div style="display: flex; align-items: center; padding: 5px; margin-top: 40px;">
            <a-table :dataSource="dataSource" :columns="columns" >
              <template #bodyCell="{ column, record }">
                <template v-if="column.key === 'operation'">
                  <div>
                    <a-select
                      ref="select"
                      style="width: 50px; margin-right: 5px;"
                      placeholder=""
                      @select="handleColumnChange(record, index, $event)"
                    >
                      <a-select-option :value="1">1</a-select-option>
                      <a-select-option :value="2">2</a-select-option>
                      <a-select-option :value="3">3</a-select-option>
                      <a-select-option :value="4">4</a-select-option>
                      <a-select-option :value="5">5</a-select-option>
                      <a-select-option :value="6">6</a-select-option>
                    </a-select>
                    <!-- <a-button type="primary" @click="loadData2Map(record)">Display</a-button> -->
                  </div>
                </template>
              </template>
            </a-table>
          </div>

        </div>
      </div>
      <div class="right">
        <div class="chartbox">
          <h1>质谱数据</h1>
          <!-- 组合式 API，引用将存储在与名字匹配的 ref 里 -->
          <div class="chart" ref="chart"></div>
        </div>
      </div>
    </div>
  </template>
  
  <script setup>
  import { ref, onMounted, computed, nextTick } from "vue";
  import axios from "axios";
  import * as echarts from "echarts";
  import "ant-design-vue/es/message/style/css";
  import "ant-design-vue/es/notification/style/css";
  import { SettingOutlined } from "@ant-design/icons-vue";
  import { message, notification } from "ant-design-vue";
  import * as ms from "./ms/ms";
  import dataPicker from "./components/dataPicker.vue";
  
  const current = ref(1);
  /* --------------------变量-------------------- */
  var file_symbol = ""; // 用于获取当前的文件类型
  var echarts_data = []; // 用于存储 echarts 的数据
  const visible_loading_database = ref(false);
  const confirmLoading = ref(false);
  
  const colorListRef = ref(null);
  
  const peptide_show = ref(false); // 展示计算肽段质量的输入框
  
  const disableResetLabel = ref(false);
  const currentFileType = ref('');
  

  const packageName = ref("PLN1");
  const proteinName = ref("P04466");

  const proteinMass = ref("18868.37");
  const modMass = ref("79.966331");

  const ptotal = ref("");


  const showModal = () => {
    visible_loading_database.value = true;
  };

  const dataSource = ref([])

  const columns = [
    {
      title: 'Package',
      dataIndex: 'packageName',
      key: 'packageName',
      mapIndex: 0,
    },
	{
      title: 'PrSM ID',
      dataIndex: 'prsmId',
      key: 'prsmId',
      mapIndex: 0,
    },
    {
      title: 'Protein',
      dataIndex: 'proteinName',
      key: 'proteinName',
      mapIndex: 0,
    },
	{
      title: 'Retention Time',
      dataIndex: 'retentionTime',
      key: 'retentionTime',
      mapIndex: 0,
    },
    {
      //width:180,
      title: 'Operation',
      dataIndex: 'operation',
      key: 'operation',
    }
  ]

  // 表格数据
  const dataMap = [
      [],
      [],
      [],
      [],
      [],
      [],
  ]
  const p_database_ip = ref(localStorage.ip || "");
  const p_database_name = ref(localStorage.db_name || "");
  const p_package_list = ref([]);
  
  const p_current_package = ref([]);
  const p_current_package2 = ref([]);

  // const handleColumnChange = async (record, index, value) => {
  //   record.mapIndex = value;
  //   await loadData2Map(record);
  // }

  // const loadData2Map = async (record) => {
  //   await get_data(record)
  // }

  const handleColumnChange = async (record, index, value) => {
    record.mapIndex = value;
    await get_data(record);
  }

  async function initEchartData(dataSource) {
    let len;  
    if( dataSource.value.length < 6)
      len = dataSource.length;
    else
      len =6;
  
    const firstSixRows = dataSource.value.slice(0, len);
    
    let i=1;
    firstSixRows.forEach((record) => {
      record.mapIndex = i;
      get_data(record);
      i++;
    });
  }


  // 从服务器中获取所有的包
  async function get_package_list() {
    axios.post("database").then(
      (res) => {
        confirmLoading.value = true;
        p_package_list.value = res.data.package_list.map((item) => {
          return { label: item[0], value: item[0] };
        });
        confirmLoading.value = false;
        visible_loading_database.value = false;
  
        localStorage.ip = p_database_ip.value;
        localStorage.db_name = p_database_name.value;
      },
      (error) => {
        notification["error"]({
          message: "网络错误",
          description: "请检查网络地址或者数据库名是否正确",
        });
        visible_loading_database.value = false;
      }
    );
  }
  const getData = async (type) => {
    // return testData;
    const res = await axios.get('http://localhost:8888/database/idMs1Vec/'+packageName.value+'/'+proteinName.value);
    if(res.data.dm1_list.length > 0){
        dataSource.value = res.data.dm1_list;
        dataSource.value.sort(function(a, b) {
          return a.retentionTime - b.retentionTime;
        });
    }else{
        return []
    }
    // initEchartData(dataSource);
  }
  async function get_data(record) {
      axios
          .post(
              //"database/" +
              //record.packageName +
              //"/dm1/"+
              //record.Ms1Scan

              "dm1range/" +
              record.packageName + "/" +
              record.Ms1Scan
              + "/" + record.prsmId
          )
          .then((res) => {
            dataMap[record.mapIndex - 1] = res.data.peaks.map((item, index) => {
              return {
                mass: item[0],
                itensity: item[1],
                charge: item[2],
                index: index + 1,
              };
            });

            visualize(record.mapIndex - 1);
          });
  }
  
  async function get_ims_data(id) {
    axios
      .post("database/" + p_current_package.value + "/" + "dms" + "/" + id)
      .then((res) => {
        echarts_data = res.data.peaks.map((row, index) => {
          return [row[0], row[1], index + 1];
        });
        // echarts_data 排序
        echarts_data.sort((row1, row2) => row1[0] - row2[0]);
        option.series[0].data = echarts_data;
        //console.log('option', option);
        // myChart.setOption(option);
      });
  }
  async function get_ims_data2(id) {
    axios
      .post("database/" + p_current_package2.value + "/" + "dms" + "/" + id)
      .then((res) => {
        echarts_data = res.data.peaks.map((row, index) => {
          return [row[0], row[1], index + 1];
        });
        // echarts_data 排序
        echarts_data.sort((row1, row2) => row1[0] - row2[0]);
        option.series[1].data = echarts_data;
        disableResetLabel.value = true;
        //console.log('option', option, peptide2.value);
        nextTick(() => {
          setDisplay2(proteform2.value, true)
          myChart.setOption(option);
        })
      })
  }
  /* -------------------- EChart 配置 -------------------- */
  const chart = ref(null);
  var myChart = null;
  
  // 设置 x 轴最小长度，确保图表显示正确
  // 这里先固定为质谱数据中 mass的最大值，根据实际更改
  var xAxis_min = 200;
  const peptide = ref("");
  const peptide2 = ref("");
  const proteform = ref("");
  const proteform2 = ref("");
  
  var proteoform_mass = ref(0);
  
  var bionsMasses = [];
  var yionsMasses = [];
  
  var bionsMasses2 = [];
  var yionsMasses2 = [];
  // 图表配置
  var option = {
    // 图表标题
    title: {
    },
    // 提示框
    // tooltip: {
    //   trigger: "axis",
    // },
    tooltip: {
      // 显示自定义内容
      trigger: "axis",
      formatter: function (params) {
        // 定义一个空字符串用于保存处理结果
        var html = "";
        // 遍历数据项
        for (var i = 0; i < params.length; i++) {
          // 使用占位符显示系列名和数据项名称
          // 使用占位符显示数值
          html += "<div>Peak: {c0}</div>";
          html += "<div>Mass: {c1}</div>";
          html += "<div>Itensity: {c2}</div>";
          // 将占位符替换为实际数值
          html = html
            .replace("{c0}", params[i].value[2])
            .replace("{c1}", params[i].value[0])
            .replace("{c2}", params[i].value[1]*2000000);
        }
        // 返回提示信息
        return html;
      },
    },
    // 标签
    legend: {
      data: [""],
      left: "6%",
    },
    // 工具
    toolbox: {
      top:'0%',
      right:'5%',
      feature: {
        dataZoom: {
          yAxisIndex: "none",
        },
        restore: {},
        saveAsImage: {},
      },
    },
    //缩放组件
    dataZoom: [
      {
        show: true,
        realtime: true,
        start: 0,
        end: 100,
        xAxisIndex: [0, 1,2,3,4,5],
      },
      {
        type: "inside",
        realtime: true,
        start: 30,
        end: 70,
        xAxisIndex: [0, 1,2,3,4,5],
      },
    ],
    grid: [
      {
        gridIndex: 0,
        left: 80,
        right: 50,
        top: '0%',
        height: '15%'
      },
      {
        gridIndex: 1,
        left: 80,
        right: 50,
        top: '15%',
        height: '15%'
      },
      {
        gridIndex: 2,
        left: 80,
        right: 50,
        top: '30%',
        height: '15%'
      },
      {
        gridIndex: 3,
        left: 80,
        right: 50,
        top: '45%',
        height: '15%'
      },
      {
        gridIndex: 4,
        left: 80,
        right: 50,
        top: '60%',
        height: '15%'
      },
      {
        gridIndex: 5,
        left: 80,
        right: 50,
        top: '75%',
        height: '15%'
      },
    ],
    xAxis: [
      {
        gridIndex: 0,
        position: "top",
        type: "value",
        position: 'center',
        min: 0,
        max: 40000,
        //interval:4000,
        boundaryGap: false,
        axisLine: { onZero: true }
      },
      {
        gridIndex: 1,
        type: "value",
        min: 0,
        max: 40000,
        boundaryGap: false,
        axisLine: { onZero: true },
      },

      {
        gridIndex: 2,
        type: "value",
        min: 0,
        max: 40000,
        boundaryGap: false,
        axisLine: { onZero: true },
      },

      {
        gridIndex: 3,
        type: "value",
        min: 0,
        max: 40000,
        boundaryGap: false,
        axisLine: { onZero: true },
      },


      {
        gridIndex: 4,
        type: "value",
        min: 0,
        max: 40000,
        boundaryGap: false,
        axisLine: { onZero: true },
      },


      {
        gridIndex: 5,
        type: "value",
        min: 0,
        max: 40000,
        boundaryGap: false,
        axisLine: { onZero: true },
      },


    ],
    yAxis: [
      {
        gridIndex: 0,
        min: 0,
        max: 1,
        name: "",
        type: "value",
        // Y 轴标题
        nameTextStyle: {
          padding: [0, -180, -105, 0],
          fontSize: 25,
          color: "#000000"
        },
        axisLabel: {
            show: false // 隐藏Y轴的数字
        }
      },
      {
        gridIndex: 1,
        min: 0,
        max: 1,
        // name: "MS2-co",
        type: "value",
        // Y 轴标题
        nameTextStyle: {
          padding: [0, -180, -105, 0],
          fontSize: 25,
          color: "#000000"
        },
        axisLabel: {
            show: false // 隐藏Y轴的数字
        }
        // inverse: true
      },
      {
        gridIndex: 2,
        min: 0,
        max: 1,
        //name: "MS2-co",
        type: "value",
        // Y 轴标题
        nameTextStyle: {
          padding: [0, -180, -105, 0],
          fontSize: 25,
          color: "#000000"
        },
        axisLabel: {
            show: false // 隐藏Y轴的数字
        }
      },
      {
        gridIndex: 3,
        min: 0,
        max: 1,
        //name: "MS2-co",
        type: "value",
        // Y 轴标题
        nameTextStyle: {
          padding: [0, -180, -105, 0],
          fontSize: 25,
          color: "#000000"
        },
        axisLabel: {
            show: false // 隐藏Y轴的数字
        }
      },
      {
        gridIndex: 4,
        min: 0,
        max: 1,
        //name: "MS2-co",
        type: "value",
        // Y 轴标题
        nameTextStyle: {
          padding: [0, -180, -105, 0],
          fontSize: 25,
          color: "#000000"
        },
        axisLabel: {
            show: false // 隐藏Y轴的数字
        }
      },
      {
        gridIndex: 5,
        min: 0,
        max: 1,
       // name: "MS2-co",
        type: "value",
        // Y 轴标题
        nameTextStyle: {
          padding: [0, -180, -105, 0],
          fontSize: 25,
          color: "#000000"
        },
        axisLabel: {
            show: false // 隐藏Y轴的数字
        }
      },
    ],
    series: [
      // ms2 数据
      {
        name: "MS2",
        xAxisIndex: 0,
        yAxisIndex: 0,
        gridIndex: 0,
        type: "bar",
        barWidth: "2px",
        itemStyle: {
          color: "#003366",
        },
        label: {
          show: true,
          color: "#323232",
          position: "top",
          formatter: function (d) {
            return formatlab(d);
          },
          rich: {
            a: {
              align: "center",
              color: "blue",
            },
            b: {
              align: "center",
              color: "red",
            },
          },
        },
        data: [],
        markArea: {
          itemStyle: {
            color: 'rgba(255, 173, 177, 0.4)'
          },
          data: [
          ]
        },
      },
      // ms2 对比数据
      {
        name: "MS2-co",
        xAxisIndex: 1,
        yAxisIndex: 1,
        gridIndex: 1,
        type: "bar",
        barWidth: "2px",
        itemStyle: {
          color: "#003366",
        },
        label: {
          show: true,
          color: "#323232",
          position: "bottom",
          formatter: function (d) {
            return formatlab2(d);
          },
          rich: {
            a: {
              align: "center",
              color: "blue",
            },
            b: {
              align: "center",
              color: "red",
            },
          },
        },
        data: [],
        markArea: {
          itemStyle: {
            color: 'rgba(255, 173, 177, 0.4)'
          },
          data: [
          ]
        },
      },
      {
        name: "MS2-co",
        xAxisIndex: 2,
        yAxisIndex: 2,
        gridIndex: 2,
        type: "bar",
        barWidth: "2px",
        itemStyle: {
          color: "#003366",
        },
        label: {
          show: true,
          color: "#323232",
          position: "bottom",
          formatter: function (d) {
            return formatlab2(d);
          },
          rich: {
            a: {
              align: "center",
              color: "blue",
            },
            b: {
              align: "center",
              color: "red",
            },
          },
        },
        data: [],
        markArea: {
          itemStyle: {
            color: 'rgba(255, 173, 177, 0.4)'
          },
          data: [
          ]
        },
      },
      {
        name: "MS2-co",
        xAxisIndex: 3,
        yAxisIndex: 3,
        gridIndex: 3,
        type: "bar",
        barWidth: "2px",
        itemStyle: {
          color: "orange",
        },
        label: {
          show: true,
          color: "#323232",
          position: "bottom",
          formatter: function (d) {
            return formatlab2(d);
          },
          rich: {
            a: {
              align: "center",
              color: "blue",
            },
            b: {
              align: "center",
              color: "red",
            },
          },
        },
        data: [],
        markArea: {
          itemStyle: {
            color: 'rgba(255, 173, 177, 0.4)'
          },
          data: [
          ]
        },
      },
      {
        name: "MS2-co",
        xAxisIndex: 4,
        yAxisIndex: 4,
        gridIndex: 4,
        type: "bar",
        barWidth: "2px",
        itemStyle: {
          color: "orange",
        },
        label: {
          show: true,
          color: "#323232",
          position: "bottom",
          formatter: function (d) {
            return formatlab2(d);
          },
          rich: {
            a: {
              align: "center",
              color: "blue",
            },
            b: {
              align: "center",
              color: "red",
            },
          },
        },
        data: [],
        markArea: {
          itemStyle: {
            color: 'rgba(255, 173, 177, 0.4)'
          },
          data: [
          ]
        },
      },
      {
        name: "MS2-co",
        xAxisIndex: 5,
        yAxisIndex: 5,
        gridIndex: 5,
        type: "bar",
        barWidth: "2px",
        itemStyle: {
          color: "orange",
        },
        label: {
          show: true,
          color: "#323232",
          position: "bottom",
          formatter: function (d) {
            return formatlab2(d);
          },
          rich: {
            a: {
              align: "center",
              color: "blue",
            },
            b: {
              align: "center",
              color: "red",
            },
          },
        },
        data: [],
        markArea: {
          itemStyle: {
            color: 'rgba(255, 173, 177, 0.4)'
          },
          data: [
          ]
        },
      },
    ],
  };
  onMounted(() => {
    // 基于准备好的dom，初始化echarts实例
    myChart = echarts.init(chart.value);
    // option.xAxis[0].max = xAxis_min + 50;
    // 绘制图表
    myChart.setOption(option);
  });
  
  function setDisplay(val, isProteform) {
    // 情况顶部列表
    disableResetLabel.value = false;
    // res 包含肽段质量，肽段长度和 b 粒子和 y 离子
    var res = ms.calculatePeptide(val, isProteform);
    var len = res.peptide_len;
  
    bionsMasses = new Array(len - 1);
    yionsMasses = new Array(len - 1);
  
    for (var i = 0; i < len - 1; i++) {
      bionsMasses[i] = new Array(2);
      bionsMasses[i][0] = res.b_ion_arr[i];
      bionsMasses[i][1] = i + 1;
  
      yionsMasses[i] = new Array(2);
      yionsMasses[i][0] = res.y_ion_arr[i];
      yionsMasses[i][1] = i + 1;
    }
  
    if (option && typeof option === "object") {
      // 由于 x 轴的最大值跟随 partial_y 轴变化
      // partial_y 又取决于肽段的长度，所有需要设定一个最小值
      var x_max = Math.max(xAxis_min, res.peptide_mass);
      // option.xAxis[0].max = Math.floor(x_max * 1.1);
      myChart.setOption(option, true);
      return res.peptide_mass;
    }
  }
  function setDisplay2(val, isProteform) {
    // 情况顶部列表
    disableResetLabel.value = true;
    // res 包含肽段质量，肽段长度和 b 粒子和 y 离子
    var res = ms.calculatePeptide(val, isProteform);
    var len = res.peptide_len;
    //console.log("setDisplay", val, res);
  
    bionsMasses2 = new Array(len - 1);
    yionsMasses2 = new Array(len - 1);
  
    for (var i = 0; i < len - 1; i++) {
      bionsMasses2[i] = new Array(2);
      bionsMasses2[i][0] = res.b_ion_arr[i];
      bionsMasses2[i][1] = i + 1;
  
      yionsMasses2[i] = new Array(2);
      yionsMasses2[i][0] = res.y_ion_arr[i];
      yionsMasses2[i][1] = i + 1;
    }
  
    if (option && typeof option === "object") {
      // 由于 x 轴的最大值跟随 partial_y 轴变化
      // partial_y 又取决于肽段的长度，所有需要设定一个最小值
      var x_max = Math.max(xAxis_min, res.peptide_mass);
      // option.xAxis[1].max = Math.floor(x_max * 1.1);
      myChart.setOption(option);
      return res.peptide_mass;
    }
  }
  
  function formatlab(d) {
    if (bionsMasses.length == 0 || yionsMasses.length == 0) {
      return "";
    }
  
    // d.data[0] 表示质量
    // ppm 匹配误差
    let b_pos = isMatched(bionsMasses, d.data[0], 1.5e-5);
    let y_pos = isMatched(yionsMasses, d.data[0], 1.5e-5);
  
    const bLabel = b_pos >= 0 ? "B" + (b_pos + 1) : "";
    const yLabel = y_pos >= 0 ? "Y" + (y_pos + 1) : "";
    if (!disableResetLabel.value) {
      colorListRef.value.insertValue(bLabel, d);
      colorListRef.value.insertValue(yLabel, d);
    }
  
    const re = (
      "{a|\n" +
      (b_pos >= 0 ? "B" + (b_pos + 1) : "") +
      "}" +
      "{b|\n" +
      (y_pos >= 0 ? "Y" + (y_pos + 1) : "") +
      "}"
    );
    return re
  }
  function formatlab2(d) {
    if (bionsMasses2.length == 0 || yionsMasses2.length == 0) {
      return "";
    }
  
    // d.data[0] 表示质量
    // ppm 匹配误差
    let b_pos = isMatched(bionsMasses2, d.data[0], 1.5e-5);
    let y_pos = isMatched(yionsMasses2, d.data[0], 1.5e-5);
  
    const bLabel = b_pos >= 0 ? "B" + (b_pos + 1) : "";
    const yLabel = y_pos >= 0 ? "Y" + (y_pos + 1) : "";
  
  
    if (!disableResetLabel.value) {
      colorListRef.value.insertValue(bLabel, d);
      colorListRef.value.insertValue(yLabel, d);
    }
  
    const re = (
      "{a|\n" +
      (b_pos >= 0 ? "B" + (b_pos + 1) : "") +
      "}" +
      "{b|\n" +
      (y_pos >= 0 ? "Y" + (y_pos + 1) : "") +
      "}"
    );
    //console.log("formatlab2 -by", d, bLabel, yLabel, re);
    return re
  }
   

  function visualize(index) {
      disableResetLabel.value = false;
      // 根据当前文件类型，将 data.value 进行相应的转换并传递给 echarts
      let max = 0;

      let rightIndex = null;
      let leftIndex = null;
      let modRightIndex = null;
      let modLeftIndex = null;
      let proIntensity = null;
      let modIntensity = null;
      let mass_tol = 2;

      echarts_data = dataMap[index].map(({ mass, itensity, index }) => {
          if (mass > max) max = mass;
          return [mass, itensity, index];
      });
      echarts_data.sort((row1, row2) => row1[0] - row2[0]);
      for(var i=0;i<echarts_data.length;i++){
        //确定标识区间
        if (leftIndex === null && parseFloat(echarts_data[i][0]) > parseFloat(Number(proteinMass.value)-Number(mass_tol))) {
          leftIndex = i;
        }
        if (rightIndex === null || parseFloat(echarts_data[i][0]) < parseFloat(Number(proteinMass.value)+Number(mass_tol))) {
          rightIndex = i;
        }
        if (modLeftIndex === null && parseFloat(echarts_data[i][0]) > 
          parseFloat(Number(proteinMass.value)+Number(modMass.value)-Number(mass_tol))) {
          modLeftIndex = i;
        }
        if (modRightIndex === null || parseFloat(echarts_data[i][0]) <
         parseFloat(Number(proteinMass.value)+Number(modMass.value)+Number(mass_tol))) {
          modRightIndex = i;
        }
        //分别误差范围内找到pro和p-pro丰度最大的峰
        let diff = difference(proteinMass.value, echarts_data[i][0]);
        if(diff < mass_tol){
          if(proIntensity === null || parseFloat(echarts_data[i][1]) > parseFloat(proIntensity)){
            proIntensity = echarts_data[i][1];
          }
        }
        let modDiff = difference(Number(proteinMass.value)+Number(modMass.value), echarts_data[i][0]);
        if(modDiff < mass_tol){
          if(modIntensity === null || parseFloat(echarts_data[i][1]) > parseFloat(modIntensity)){
            modIntensity = echarts_data[i][1];
          }
        }
      }
      modIntensity = modIntensity*2000000.0;
      proIntensity = proIntensity*2000000.0;

      let pt = '';
      if(modIntensity===0 || proIntensity===0){
        pt = "0.0";
      }else{
        pt = Number(calPtotal(modIntensity,proIntensity)).toFixed(2);
      }
      // console.log("modI："+modIntensity+"  proI:"+proIntensity+"  pt:"+pt);
      

     
      option.yAxis[index].name ="PTotal_" + Number(Number(index)+1) + "=" + pt;
      option.xAxis[index].max =  parseInt(parseInt(max - 350)/50)*50;
      option.xAxis[index].min =  parseInt(parseInt(max - 550)/50)*50;
      option.series[index].data = echarts_data;


      // 清除现有的 markArea 数据
      option.series[index].markArea.data = [];
      // 添加无修饰的 markArea 数据
      if (leftIndex !== null) {
          option.series[index].markArea.data.push([{
              name: proteinName.value,
              label: {
                show: true,
                offset: [45, 40],
                color: '#000000',
                fontSize: 18,
                zIndex: 10,
              },
              xAxis: echarts_data[leftIndex][0], // 标记区域的起始位置（左侧）
              itemStyle: {
                  color: "rgba(255, 173, 177, 0.4)",
                  // borderColor: "#00A5C4",
                  // borderWidth: 2,
                  // borderType: 'dotted'
              },
          },
          {
              xAxis: echarts_data[rightIndex][0], // 标记区域的结束位置（右侧）
          }]);
      }
      // 添加磷酸化修饰的 markArea 数据
      if (modLeftIndex !== null) {
          option.series[index].markArea.data.push([{
              name: "p-"+proteinName.value,
              label: {
                show: true,
                offset: [55, 40],
                color: '#ff0000',
                fontSize: 18,
                zIndex: 10,
              },
              xAxis: echarts_data[modLeftIndex][0], // 标记区域的起始位置（左侧）
              itemStyle: {
                  color: "rgba(255, 173, 177, 0.4)",
                  // borderColor: "#00A5C4",
                  // borderWidth: 2,
                  // borderType: 'dotted'
              },
          },
          {
              xAxis: echarts_data[modRightIndex][0], // 标记区域的结束位置（右侧）
          }]);
      }


      // console.log(option);
      myChart.setOption(option);
  }
  
  function difference(a, b) {
    return Math.abs(parseFloat(a) - parseFloat(b));
  }

  function calPtotal(a, b) {
    if(a === null || b === null){
      return 0;
    }else{
      return parseFloat(a)/(parseFloat(a)+parseFloat(b));
    }
  }

  </script>
  
  <style scoped>
  :deep().ant-radio {
    display: none;
  }
  
  .container {
    display: flex;
    height: 100vh;
  }
  
  .left {
    flex-basis: 36%;
    padding: 20px;
    border-right: 1px solid #ccc;
    display: flex;
    flex-direction: column;
  }
  
  .left-top {
    flex-basis: 25%;
  }
  
  .left-bottom {
    flex-basis: 75%;
  }
  
  .right {
    flex-basis: 70%;
    padding: 20px;
    background-color: #fff;
  }
  
  .chartbox {
    padding: 20px;
    /*垂直居中*/
    align-items: center;
    /*水平居中*/
    justify-self: center;
    height: 100%;
  }
  
  .chart {
    margin-top: 50px;
    margin-left: 50px;
    width: 90%;
    height: 90%;
  }
  
  .visualize-left {
    position: absolute;
  }
  
  .visualize-right {
    position: absolute;
    right: 100px;
  }
  
  .activePos {
    border: 1px solid skyblue;
  }
  
  .activePosB {
    border-right: 1px solid blue;
  }
  
  .activePosY {
    border-left: 1px solid red;
  }
  
  .highlightBg {
    background-color: skyblue;
  }
  </style>
  