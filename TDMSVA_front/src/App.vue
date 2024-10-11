<template>
  <div class="container">
    <div class="left">
      <div class="left-top">
        <a-space direction="vertical" size="middle">
          <a-button type="primary" @click="showModal">
            <template #icon>
              <SettingOutlined />
            </template>
            Connecting Database
          </a-button>
          <a-modal title="Connecting Database" ok-text="OK" cancel-text="Cancel" v-model:visible="visible_loading_database"
            :confirm-loading="confirmLoading" @ok="get_package_list">
            <a-input addon-before="Port" style="padding-bottom: 20px" v-model:value="p_database_ip" />
            <a-input addon-before="Database Name" v-model:value="p_database_name" />
          </a-modal>
          <dataPicker :p_package_list="p_package_list" :handle-data="handleData" :visualize="visualize" :showSearch="true"></dataPicker>
          <dataPicker :p_package_list="p_package_list" :handle-data="handleData2" :visualize="visualize2" :showSearch="true"></dataPicker>
        </a-space>
      </div>
      <div class="left-bottom">
        <template v-if="currentFileType === 'ims'">
          <div style="margin-top: 10px;"></div>
          <h2>Prsm File</h2>

          <div style="width: 100%; border: 1px solid #ccc; border-radius: 5px; font-size: 16px; width: 600px;">

            <template v-for="(d, index) in data.slice(10 * (current - 1), 10 * current)">
              <div
                style="text-align: center; border-bottom: 1px solid #ccc; padding: 4px; color: #fff; font-weight: bold; background-color: #18b8FF">
                {{ d.attribute }} </div>
              <div style="display: flex; border-bottom: 1px solid #ccc; padding: 4px;">

                <div style="width: 295px; text-align: center; border-right: 1px solid #ccc; word-wrap: break-word;">
                  {{ d.value }}
                </div>

                <div v-if="data2[index]" style="width: 295px; flex: 1; text-align: center; word-wrap: break-word;">
                  {{ data2[(current - 1) * 10 + index].value }}
                </div>
              </div>
            </template>

          </div>

          <div style="margin-top: 10px">
            <a-pagination v-model:current="current" :total="data.length" show-less-items />
          </div>
        </template>
        <template v-else>
        <a-table :columns="columns" :data-source="data" bordered :pagination="{
            defaultPageSize: 11,
          }" size="small" :scroll="{ x: 300, y: '400' }">
          <template #title>Data</template>
        </a-table>
        </template>

      </div>
    </div>
    <div class="right">
      <colorList ref="colorListRef" @handleSplitClick="handleSplitClick"></colorList>

      <div class="visualize-left">
      </div>
      <div class="visualize-right" v-show="peptide_show">
        <a-space>
          <div style="width: 170px;">
            Database protein sequence
          </div>
          <a-input style="min-width: 30%" v-model:value="peptide" />
          <a-button @click="peptideChanged"> Calculate </a-button>
          Mass： {{ peptide_mass }}
        </a-space>
        <br />

        <a-space style="margin-top: 10px">
          <div style="width: 170px;">
            Proteoform sequence  
          </div>
          <a-input style="min-width: 30%" v-model:value="proteform" />
          <a-button @click="proteoformChanged({ isReset: true })"> Calculate </a-button>
          Mass： {{ proteoform_mass }}
        </a-space>
      </div>
      <div class="chartbox">
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
import colorList from "./components/colorList.vue";

const current = ref(1);
/* --------------------变量-------------------- */
var file_symbol = ""; // 用于获取当前的文件类型
var echarts_data = []; // 用于存储 echarts 的数据
const selectedGroup = ref('');
const visible_loading_database = ref(false);
const visible_header_show = ref(true);
const visible_header_info = ref(false);
const confirmLoading = ref(false);

const colorListRef = ref(null);

const peptide_show = ref(false); // 展示计算肽段质量的输入框

const disableResetLabel = ref(false);
const currentFileType = ref('');

const DESCTYPEINFO = ref({
  'Acetyl': { color: '#C7EDCC', label: '' },
  'Methyl': { color: '#FFD700', babel: '' },
  'Dimethyl': { color: '#FDE6E0', label: '' },
  'Trimethyl': { color: '#DCE2F1', label: '' },
  'Phospho': { color: '#E3EDCD', label: '' },
  'Default': { color: 'orange', label: '' },
  'Multiple': { color: '#C0C0C0', label: '' },
  '-': 'Unknown'
})

const handleSplitClick = (item, type) => {
  console.log("handleSplitClick");
  if (type === 'Y') {
    myChart.dispatchAction({
      type: 'dataZoom',
      startValue: item.yData.data[0] - 750,
      endValue: item.yData.data[0] + 750
    });

    myChart.dispatchAction({
      type: 'showTip',
      seriesIndex: 0,
      dataIndex: item.yData.dataIndex
    })
  }

  if (type === 'B') {
    myChart.dispatchAction({
      type: 'dataZoom',
      startValue: item.bData.data[0] - 750,
      endValue: item.bData.data[0] + 750
    });

    myChart.dispatchAction({
      type: 'showTip',
      seriesIndex: 0,
      dataIndex: item.bData.dataIndex
    })
  }


}

const getDescType = (descString) => {
  const typeList = Object.keys(DESCTYPEINFO.value);
  let finalType = 'Default';
  const matchedTypes = typeList.filter(type => descString.includes(type));
  console.log("matchedTypes", matchedTypes)

  if (matchedTypes.length >= 2) {
    finalType = 'Multiple';
  } else if (matchedTypes.length === 1) {
    finalType = matchedTypes[0];
  }

  return finalType;
}
const selectValue = ref([]);
const showModal = () => {
  visible_loading_database.value = true;
};
const showHeaderInfo = () => {
  visible_header_info.value = true;
};
// 表格列属性
const columns = ref([
  {
    title: "Index",
    dataIndex: "index",
    width: 70,
    sorter: (row1, row2) => row1.index - row2.index,
    align: "center",
  },
  {
    title: "Mass",
    dataIndex: "mass",
    sorter: (row1, row2) => row1.mass - row2.mass,
  },
  {
    title: "Intensity",
    dataIndex: "itensity",
    sorter: (row1, row2) => row1.itensity - row2.itensity,
  },
]);
const columns_header = ref([
  {
    title: "Attribute",
    dataIndex: "attribute",
  },
  {
    title: "Value",
    dataIndex: "value",
  },
]);
const header_source = ref([]);
// 表格数据
const data = ref([]);
const data2 = ref([]);
const p_database_ip = ref(localStorage.ip || "");
const p_database_name = ref(localStorage.db_name || "");
const p_package_list = ref([]);

const p_current_package = ref([]);
const p_current_package2 = ref([]);

const handleVisualize = () => {
  visualize();
  visualize2();
}
/* --------------------函数-------------------- */
function getSelectValue() {
  // console.log(selectValue.value);
  get_data_by_select(selectValue.value);
}
// 从服务器中获取所有的包
async function get_package_list() {
  axios.post("database").then(
    (res) => {
      confirmLoading.value = true;
      console.log(res.data);
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
        message: "Database Connection Error",
        description: "Check that the Port and Database name are correct",
      });
      visible_loading_database.value = false;
    }
  );
}
async function get_data_by_select(val) {
  if (val.length != 3) {
    message.error("Data Format Error,参数不满足（文件+包+id）格式");
  } else {
    axios
      .post("database/" + val[0] + "/" + val[1] + "/" + val[2])
      .then((res) => {
        console.log(res.data);
        handleData(val[1], res.data);
      });
  }
}
function handleData(index,file_type, value, currentPackage) {
  p_current_package.value = currentPackage;
  peptide_show.value = false;
  currentFileType.value = file_type;
  peptide.value = "";
  proteform.value = "";
  if (file_type == "rms") {
    file_symbol = "rms";
    columns.value = [
      {
        title: "Index",
        dataIndex: "index",
        width: 50,
        sorter: (row1, row2) => row1.index - row2.index,
        align: "center",
      },
      {
        title: "M/Z",
        dataIndex: "mass",
        width: 70,
        sorter: (row1, row2) => row1.mass - row2.mass,
      },
      {
        title: "Intensity",
        dataIndex: "itensity",
        width: 70,
        sorter: (row1, row2) => row1.itensity - row2.itensity,
      },
    ];
    data.value = value.peaks.map((item, index) => {
      return {
        mass: item[0],
        itensity: item[1],
        index: index + 1,
      };
    });
    console.log("data", data.value);

    header_source.value = [];
    for (let prop in value.header) {
      header_source.value.push({
        attribute: prop,
        value: value.header[prop],
      });
    }
  }

  if (file_type == "dms" || file_type == "dm1") {
    file_symbol = "dms";
    columns.value = [
      {
        title: "Index",
        dataIndex: "index",
        width: 50,
        sorter: (row1, row2) => row1.index - row2.index,
        align: "center",
      },
      {
        title: "Mass",
        dataIndex: "mass",
        width: 50,
        sorter: (row1, row2) => row1.mass - row2.mass,
      },
      {
        title: "Intensity",
        dataIndex: "itensity",
        width: 50,
        sorter: (row1, row2) => row1.itensity - row2.itensity,
      },
      {
        title: "Charge",
        dataIndex: "charge",
        sorter: (row1, row2) => row1.charge - row2.charge,
        width: 40,
      },
    ];

    data.value = value.peaks.map((item, index) => {
      return {
        mass: item[0],
        itensity: item[1],
        charge: item[2],
        index: index + 1,
      };
    });

    header_source.value = [];
    for (let prop in value.header) {
      header_source.value.push({
        attribute: prop,
        value: value.header[prop],
      });
    }
  }

  if (file_type == "ims") {
    file_symbol = "ims";
    peptide_show.value = true;
    columns.value = [
      {
        title: "Index",
        dataIndex: "index",
        align: "center",
        width: 70,
      },
      {
        title: "Attribute",
        dataIndex: "attribute",
        width: 100,
      },
      {
        title: "Value",
        dataIndex: "value",
        width: 100,
      },
    ];
    peptide_show.value = true;
    data.value = [];
    header_source.value = [];

    peptide.value = value["Database protein sequence"];
    proteform.value = value["Proteoform"];

    let index = 1;
    for (let key in value) {
      data.value.push({
        attribute: key,
        value: value[key],
        index: index++,
      });
    }
  }
}
function handleData2(index,file_type, value, currentPackage) {
  p_current_package2.value = currentPackage;
  // peptide_show.value = false;
  peptide2.value = "";
  proteform2.value = "";
  if (file_type == "rms") {
    file_symbol = "rms";
    columns.value = [
      {
        title: "Index",
        dataIndex: "index",
        width: 50,
        sorter: (row1, row2) => row1.index - row2.index,
        align: "center",
      },
      {
        title: "M/Z",
        dataIndex: "mass",
        width: 70,
        sorter: (row1, row2) => row1.mass - row2.mass,
      },
      {
        title: "Intensity",
        dataIndex: "itensity",
        width: 70,
        sorter: (row1, row2) => row1.itensity - row2.itensity,
      },
    ];
    data.value = value.peaks.map((item, index) => {
      return {
        mass: item[0],
        itensity: item[1],
        index: index + 1,
      };
    });

    header_source.value = [];
    for (let prop in value.header) {
      header_source.value.push({
        attribute: prop,
        value: value.header[prop],
      });
    }
  }

  if (file_type == "dms" || file_type == "dm1") {
    file_symbol = "dms";
    columns.value = [
      {
        title: "Index",
        dataIndex: "index",
        width: 50,
        sorter: (row1, row2) => row1.index - row2.index,
        align: "center",
      },
      {
        title: "Mass",
        dataIndex: "mass",
        width: 50,
        sorter: (row1, row2) => row1.mass - row2.mass,
      },
      {
        title: "Intensity",
        dataIndex: "itensity",
        width: 50,
        sorter: (row1, row2) => row1.itensity - row2.itensity,
      },
      {
        title: "Charge",
        dataIndex: "charge",
        sorter: (row1, row2) => row1.charge - row2.charge,
        width: 40,
      },
    ];

    data.value = value.peaks.map((item, index) => {
      return {
        mass: item[0],
        itensity: item[1],
        charge: item[2],
        index: index + 1,
      };
    });

    header_source.value = [];
    for (let prop in value.header) {
      header_source.value.push({
        attribute: prop,
        value: value.header[prop],
      });
    }
  }
  data2.value = [];

  if (file_type == "ims") {
    file_symbol = "ims";
    peptide2.value = value["Database protein sequence"];
    proteform2.value = value["Proteoform"];
    let index = 1;
    for (let key in value) {
      data2.value.push({
        attribute: key,
        value: value[key],
        index: index++,
      });
    }
  }
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
      console.log('option', option);
      nextTick(() => {
        proteoformChanged();
        colorListRef.value.addDataDesc(proteform.value);
        disableResetLabel.value = true;
      })
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
      console.log('option', option, peptide2.value);
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
var differTwoIons = 0.0;
const peptide = ref("");
const peptide2 = ref("");
const proteform = ref("");
const proteform2 = ref("");

var peptide_mass = ref(0);
var proteoform_mass = ref(0);

var bionsMasses = [];
var yionsMasses = [];

var bionsMasses2 = [];
var yionsMasses2 = [];
// 图表配置
var option = {
  // 图表标题
  title: {
    text: "Mass Spectrum Peak",
    left: "center",
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
          .replace("{c2}", params[i].value[1]);
      }
      // 返回提示信息
      return html;
    },
  },
  // 标签
  legend: {
    data: ["MS2"],
    left: "6%",
  },
  // 工具
  toolbox: {
    feature: {
      dataZoom: {
        yAxisIndex: "none",
      },
      restore: {},
      saveAsImage: {},
    },
  },
  // 缩放组件
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
      top: '8%',

      height: '40%'
    },
    {
      left: 80,
      right: 50,
      top: '48%',
      height: '40%'
    }
  ],
  xAxis: [
    {
      position: "top",
      type: "value",
      position: 'center',
      min: 0,
      max: 100,
      boundaryGap: false,
      axisLine: { onZero: true },
    },
    {
      gridIndex: 1,
      type: "value",
      min: 0,
      max: 100,
      boundaryGap: false,
      axisLine: { onZero: true },
    },
  ],
  yAxis: [
    {
      name: "Mass",
      type: "value",
      // Y 轴标题
      nameTextStyle: {
        padding: [0, 70, 5, 0],
      },
    },

    {
      gridIndex: 1,
      name: "MS2-co",
      type: "value",
      // Y 轴标题
      nameTextStyle: {
        padding: [0, 70, 5, 0],
      },
      inverse: true
    },
  ],
  series: [
    // ms2 数据
    {
      name: "MS2",
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
    },
  ],
};
onMounted(() => {
  // 基于准备好的dom，初始化echarts实例
  myChart = echarts.init(chart.value);
  option.xAxis[0].max = xAxis_min + 50;
  // 绘制图表
  myChart.setOption(option);
});
// 计算肽段
function peptideChanged() {
  let val = setDisplay(peptide.value, false);
  copyToClipboard(peptide.value);
  peptide_mass.value = val.toFixed(3);
}
// 计算变体肽段
function proteoformChanged(option) {
  if (option && option.isReset) {
    displayPetideList(proteform.value);
    nextTick(() => {
      // addDataDesc();
      colorListRef.value.addDataDesc(proteform.value);
      disableResetLabel.value = true;
    })
  }

  let val = setDisplay(proteform.value, true);
  copyToClipboard(proteform.value);
  proteoform_mass.value = val.toFixed(3);

}
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
    option.xAxis[0].max = Math.floor(x_max * 1.1);
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
  console.log("setDisplay", val, res);

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
    option.xAxis[1].max = Math.floor(x_max * 1.1);
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
  console.log("formatlab2 -by", d, bLabel, yLabel, re);
  return re
}
// 在前缀离子质量中判断 x 质量在误差内是否存在
function isMatched(arr, x, diff) {
  let start = 0;
  let end = arr.length - 1;
  let isMatch = false;
  while (start <= end) {
    let mid = Math.floor((start + end) / 2);
    isMatch = (Math.abs(arr[mid][0] - x) / arr[mid][0] < diff) || (Math.abs(arr[mid][0] - x + ms.HMass) / arr[mid][0] < diff) || ((Math.abs(arr[mid][0] - x - ms.HMass) / arr[mid][0] < diff));
    if (isMatch) {
      return mid;
    } else if (arr[mid][0] < x) {
      start = mid + 1;
    } else {
      end = mid - 1;
    }
  }

  return -1;
}
const displayPetideList = (proteform) => {
  // 从变体序列中获取肽段序列
  const match = proteform.match(/\.(.*)\./);
  console.log("match", match);

  const regex = /(?:\[.*?\])|./g;
  const proteformPura = match[1].match(regex).filter(item => ((/[a-zA-Z]/.test(item)) && item.length == 1));

  console.log("regex", match[1].match(regex), proteformPura)
  // proteformPura
  if (!peptide.value) {
    peptide.value = proteformPura.join('');
  }
  colorListRef.value.displayPetideList(proteform);
}
// 可视化
function visualize() {
  disableResetLabel.value = false;
  // 根据当前文件类型，将 data.value 进行相应的转换并传递给 echarts
  if (file_symbol == "rms") {
    echarts_data = data.value.map(({ mass, itensity, index }) => {
      return [mass, itensity, index];
    });

    option.xAxis[0].max = echarts_data[echarts_data.length - 1][0] * 1.1;
    option.series[0].data = echarts_data;
    myChart.setOption(option);
  }
  if (file_symbol == "dms" || file_symbol == "dm1") {
    let max = 0;
    echarts_data = data.value.map(({ mass, itensity, index }) => {
      if (mass > max) max = mass;
      return [mass, itensity, index];
    });
    console.log(echarts_data);
    option.xAxis[0].max = max * 1.1;
    option.series[0].data = echarts_data;
    myChart.setOption(option);
  }
  if (file_symbol == "ims") {
    peptide.value = data.value[20].value;
    proteform.value = data.value[21].value;
    let id = data.value[4].value;
    displayPetideList(proteform.value);
    console.log('id 1', id);
    get_ims_data(id);
  }
}

function visualize2() {
  disableResetLabel.value = false;
  // 根据当前文件类型，将 data.value 进行相应的转换并传递给 echarts
  if (file_symbol == "rms") {
    echarts_data = data.value.map(({ mass, itensity, index }) => {
      return [mass, itensity, index];
    });

    option.xAxis[1].max = echarts_data[echarts_data.length - 1][0] * 1.1;
    option.series[1].data = echarts_data;
    myChart.setOption(option);
  }
  if (file_symbol == "dms" || file_symbol == "dm1") {
    let max = 0;
    echarts_data = data.value.map(({ mass, itensity, index }) => {
      if (mass > max) max = mass;
      return [mass, itensity, index];
    });
    console.log(echarts_data);
    option.xAxis[1].max = max * 1.1;
    option.series[1].data = echarts_data;
    myChart.setOption(option);
  }
  if (file_symbol == "ims") {
    peptide2.value = data2.value[20].value;
    proteform2.value = data2.value[21].value;
    let id = data2.value[4].value;
    // displayPetideList(proteform.value);
    get_ims_data2(id);
  }
}

function copyToClipboard(text) {
  navigator.clipboard
    .writeText(text)
    .then(function () {
      console.log("Text copied to clipboard");
    })
    .catch(function (error) {
      console.error("Error copying text: ", error);
    });
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
  flex-basis: 30%;
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
  display: flex;
  /*垂直居中*/
  align-items: center;
  /*水平居中*/
  justify-self: center;
  height: 100%;
}

.chart {
  margin-top: 80px;
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
