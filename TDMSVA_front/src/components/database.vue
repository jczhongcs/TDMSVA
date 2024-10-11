<template>
  <div class="container">
    <div class="left">
      <div class="left-top">
        <a-space direction="vertical" size="middle">
          <a-button type="primary" @click="showModal">
            <template #icon><SettingOutlined /></template>
            连接数据库
          </a-button>
          <a-modal
            title="连接数据库"
            ok-text="确认"
            cancel-text="取消"
            v-model:visible="visible_loading_database"
            :confirm-loading="confirmLoading"
            @ok="get_package_list"
          >
            <a-input
              addon-before="网络地址"
              style="padding-bottom: 20px"
              v-model:value="p_network_address"
            />
            <a-input addon-before="数据库名" v-model:value="p_database_name" />
          </a-modal>

          <a-space>
            <a-select
              show-search
              v-model:value="p_current_package"
              placeholder="选择包"
              style="width: 120px"
              :options="p_package_list"
              @change="get_ms_file_list"
            ></a-select>
            <a-select
              v-model:value="p_current_ms_file"
              style="width: 120px"
              placeholder="选择文件"
              :options="p_ms_file_list"
              @change="get_id_list"
            ></a-select>
            <a-select
              show-search
              v-model:value="p_current_id"
              style="width: 120px"
              placeholder="id"
              :options="p_ms_id_list"
              @change="get_data"
            ></a-select>
          </a-space>

          <a-space>
            <a-tooltip>
              <template #title>输入格式为：包名+文件类型+id</template>
              <a-select
                v-model:value="selectValue"
                mode="tags"
                style="width: 250px"
                :token-separators="['+']"
                placeholder="输入完整 key"
                :options="selectOptions"
              ></a-select>
            </a-tooltip>

            <a-button @click="getSelectValue"> 获取 </a-button>
          </a-space>
          <a-button v-show="visible_header_show" @click="showHeaderInfo"
            >头部信息</a-button
          >
          <a-drawer
            v-model:visible="visible_header_info"
            title="头部信息"
            placement="right"
            width="500"
          >
            <a-table
              :data-source="header_source"
              :columns="columns_header"
              size="small"
              :pagination="false"
              bordered
            >
            </a-table>
          </a-drawer>
        </a-space>
      </div>
      <div class="left-bottom">
        <a-table
          :columns="columns"
          :data-source="data"
          bordered
          :pagination="{
            defaultPageSize: 5,
          }"
          size="small"
          :scroll="{ x: 300, y: 400 }"
        >
          <template #title>质谱数据</template>
        </a-table>
      </div>
    </div>
    <div class="right">
      <a-button class="visualize-button" @click="visualize">可视化</a-button>
      <div class="chartbox">
        <!-- 组合式 API，引用将存储在与名字匹配的 ref 里 -->
        <div class="chart" ref="chart"></div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from "vue";
import axios from "axios";
import * as echarts from "echarts";
import { SettingOutlined } from "@ant-design/icons-vue";
import "ant-design-vue/es/message/style/css";
import { message } from "ant-design-vue";
/* --------------------变量-------------------- */

const visible_loading_database = ref(false);
const visible_header_show = ref(true);
const visible_header_info = ref(false);
const confirmLoading = ref(false);

const selectOptions = ref([
  {
    label: "质谱数据",
    value: "rms",
  },
  {
    label: "解卷积后数据",
    value: "dms",
  },
  {
    label: "鉴定后数据",
    value: "ims",
  },
]);

const selectValue = ref([]);

const showModal = () => {
  visible_loading_database.value = true;
};
const showHeaderInfo = () => {
  visible_header_info.value = true;
};

// const handleOk = () => {
//   confirmLoading.value = true;
//   setTimeout(() => {
//     visible.value = false;
//     confirmLoading.value = false;
//   }, 2000);
// };

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
// 表格测试数据
data.value = [
  {
    index: 1,
    mass: 100,
    itensity: 100,
  },
  {
    index: 2,
    mass: 200,
    itensity: 200,
  },
];

const p_network_address = ref("");
const p_database_name = ref("");
const p_package_list = ref([]);
const p_current_package = ref([]);
const p_current_ms_file = ref([]);
const p_ms_file_list = ref([
  {
    value: "rms",
    label: "质谱数据",
    disabled: true,
  },
  {
    value: "dms",
    label: "解卷积后数据",
    disabled: true,
  },
  {
    value: "ims",
    label: "鉴定后数据",
    disabled: true,
  },
]);
const p_current_id = ref([]);
const p_ms_id_list = ref([]);

// 图表必须要指定固定的高度和宽度
const chart = ref(null);
var myChart = null;
// 使用数据集的方式初始化
var option = {
  legend: {},
  tooltip: {},
  dataset: {
    // 提供一份数据。
    source: [],
  },
  // 声明一个 X 轴，类目轴（category）。默认情况下，类目轴对应到 dataset 第一列。
  xAxis: { type: "value" },
  // 声明一个 Y 轴，数值轴。
  yAxis: { type: "value" },
  // 系列，这是一个数组，其中的不同的对象里面表示不同的数据集
  // 可以参考这个 https://echarts.apache.org/handbook/zh/concepts/dataset
  series: [
    {
      type: "bar",
      barWidth: "1px",
      encode: {
        x: 1,
        y: 2,
      },
    },
  ],
};

onMounted(() => {
  // 基于准备好的 dom，初始化 echarts 实例
  myChart = echarts.init(chart.value);
  myChart.setOption(option);
});

/* --------------------函数-------------------- */
// 可视化
function visualize() {
  option.dataset.source = data.value;
  myChart.setOption(option);
}

function getSelectValue() {
  console.log(selectValue.value);
  get_data_by_select(selectValue.value);
}

// 从服务器中获取所有的包
async function get_package_list() {
  axios.post("database").then((res) => {
    confirmLoading.value = true;
    console.log(res.data);
    p_package_list.value = res.data.package_list.map((item) => {
      return { label: item[0], value: item[0] };
    });
    confirmLoading.value = false;
    visible_loading_database.value = false;
  });
}

// 从服务器中获取当前包的文件列表
async function get_ms_file_list() {
  axios.post("database/" + p_current_package.value).then((res) => {
    for (let i = 0; i < res.data.ms_file.length; i++) {
      p_ms_file_list.value[i].disabled = !res.data.ms_file[i];
    }
  });
}

// 获取 id 列表
async function get_id_list() {
  axios
    .post("database/" + p_current_package.value + "/" + p_current_ms_file.value)
    .then((res) => {
      console.log(res.data);
      p_ms_id_list.value = res.data.id_list.map((item) => {
        return { label: item.id, value: item.id };
      });
    });
  p_ms_id_list.value = [];
  p_current_id.value = [];
}

// 获取数据
async function get_data() {
  axios
    .post(
      "database/" +
        p_current_package.value +
        "/" +
        p_current_ms_file.value +
        "/" +
        p_current_id.value
    )
    .then((res) => {
      console.log(res.data);
      handleData(p_current_ms_file.value, res.data);
    });
}

async function get_data_by_select(val) {
  if (val.length != 3) {
    message.error("参数不满足（文件+包+id）格式");
  } else {
    axios
      .post("database/" + val[0] + "/" + val[1] + "/" + val[2])
      .then((res) => {
        console.log(res.data);
        handleData(val[1], res.data);
      });
  }
}

function handleData(file_type, value) {
  if (file_type == "rms") {
    columns.value = [
      {
        title: "Index",
        dataIndex: "index",
        width: 70,
        sorter: (row1, row2) => row1.index - row2.index,
        align: "center",
      },
      {
        title: "M/z",
        dataIndex: "mass",
        width: 100,
        sorter: (row1, row2) => row1.mass - row2.mass,
      },
      {
        title: "Intensity",
        dataIndex: "itensity",
        width: 100,
        sorter: (row1, row2) => row1.itensity - row2.itensity,
      },
    ];

    data.value = value.peaks.map((item, index) => {
      return {
        index: index,
        mass: item[0],
        itensity: item[1],
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

  if (file_type == "dms") {
    columns.value = [
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
        width: 100,
        sorter: (row1, row2) => row1.mass - row2.mass,
      },
      {
        title: "Intensity",
        dataIndex: "itensity",
        width: 100,
        sorter: (row1, row2) => row1.itensity - row2.itensity,
      },
      {
        title: "Charge",
        dataIndex: "charge",
        sorter: (row1, row2) => row1.charge - row2.charge,
        width: 70,
      },
    ];

    data.value = value.peaks.map((item, index) => {
      return {
        index: index,
        mass: item[0],
        itensity: item[1],
        charge: item[2],
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
    columns.value = [
      {
        title: "Index",
        dataIndex: "index",
        align: "center",
        width: 70,
      },
      {
        title: "属性",
        dataIndex: "attribute",
        width: 100,
      },
      {
        title: "值",
        dataIndex: "value",
        width: 100,
      },
    ];

    data.value = [];
    header_source.value = [];
    let index = 0;
    for (let key in value) {
      data.value.push({
        index: index++,
        attribute: key,
        value: value[key],
      });
    }
  }
}
</script>

<style scoped>
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
  width: 90%;
  height: 90%;
}

.visualize-button {
  position: absolute;
}
</style>
