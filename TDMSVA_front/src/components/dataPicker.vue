<template>
    <div>
        <a-space>
            <a-select show-search v-model:value="p_current_package" placeholder="Package" style="width: 80px"
                :options="p_package_list" @change="get_ms_file_list"></a-select>
            <a-select v-model:value="p_current_ms_file" style="width: 120px" placeholder="FileType"
                :options="p_ms_file_list" @change="get_id_list"></a-select>
            <a-button type="primary" @click="showModSelectModal = true" v-if="showSearch" >PrSM Search</a-button>
            <a-button type="info" @click="showModSelectModal = true">{{ listCount }} Results</a-button>
            <a-select show-search v-model:value="p_current_id" style="width: 80px" placeholder="Id/Scan"
                :options="p_ms_id_list" @change="get_data"></a-select>
            <a-button type="primary" @click="visualize(index)">Display</a-button>
        </a-space>

        <a-modal v-model:visible="showModSelectModal" width="800px" title="PrSM Search" @ok="handleOk">
            <a-radio-group v-model:value="selectedGroup" :button-style="null">
                <div style="margin: 10px;">
                    <a-radio value="group1">
                        <a-checkbox-group v-model:value="mod"
                            :options="['Acetyl', 'Methyl', 'Dimethyl', 'Trimethyl', 'Phospho', 'Unknown']" />
                    </a-radio>
                </div>

                <div style="margin: 10px;">
                    <a-radio value="group2">
                        <a-checkbox-group v-model:value="mod" :options="['NoPtm']" />
                    </a-radio>
                </div>

                <div style="margin: 10px;">
                    <a-radio value="group3">
                        <a-checkbox-group v-model:value="mod" :options="[{ label: 'Default（Null）', value: 'NoSelect' }]" />
                    </a-radio>
                </div>

                <div style="margin: 10px;">
                    <a-radio value="group4">
                        <a-form style="display: flex;">
                            <a-form-item label="Protein ID Search" name="Protein ID Search" v-if="!proteinSeq">
                                <a-input v-model:value="proteinName" />
                            </a-form-item>

                            <a-form-item style="margin-left: 10px;" label="Peptide Search" name="Peptide Search"
                                v-if="!proteinName">
                                <a-input v-model:value="proteinSeq" />
                            </a-form-item>
                        </a-form>
                    </a-radio>
                </div>
                
            </a-radio-group>
        </a-modal>
    </div>
</template>

<script setup>
import axios from "axios";
import { ref, watch } from 'vue';

const proteinName = ref('');
const proteinSeq = ref('');

const p_current_package = ref([]);
const p_current_ms_file = ref([]);
const p_ms_file_list = ref([
    {
        value: "rms",
        label: "RawFile",
        disabled: true,
    },
    {
        value: "dms",
        label: "MsalignFile",
        disabled: true,
    },
    {
        value: "ims",
        label: "PrsmFile",
        disabled: true,
    },
    {
        value: "dm1",
        label: "dm1File",
        disabled: true,
    },
]);

const listCount = ref(0);
const showModSelectModal = ref(false);
const p_current_id = ref([]);
const p_ms_id_list = ref([]);
const mod = ref('');
const props = defineProps([ 'index', "handleData", "p_package_list", "visualize", "showSearch"]);

watch(() => props.p_package_list, (newVal, oldVal) => {
    console.log("packageList", newVal, oldVal);
});

// 从服务器中获取当前包的文件列表
async function get_ms_file_list() {
    axios.post("database/" + p_current_package.value).then((res) => {
        for (let i = 0; i < res.data.ms_file.length; i++) {
            p_ms_file_list.value[i].disabled = !res.data.ms_file[i];
        }
    });
}

const handleOk = () => {
    if (proteinName.value && !proteinSeq.value) {
        getListByProteinName()
        proteinName.value = ''
    }
    else if (proteinSeq.value && !proteinName.value) {
        getListByProteinSeq()
        proteinSeq.value = ''
    } else{
        get_id_listByMod()
    }
    
}

// 蛋白质名称搜索
async function getListByProteinName() {
    axios.post("database/proteinName/" + p_current_package.value + "/" + p_current_ms_file.value + "/" + proteinName.value).then(
        res => {
            listCount.value = res.data.id_list.length;
            p_ms_id_list.value = res.data.id_list.map((item) => {
                return { label: item.id, value: item.id };
            });
            showModSelectModal.value = false;
        }
    )
    p_ms_id_list.value = [];
    p_current_id.value = [];
}

// 蛋白质序列搜索
async function getListByProteinSeq() {
    axios.post("database/proteinSeq/" + p_current_package.value + "/" + p_current_ms_file.value + "/" + proteinSeq.value).then(
        res => {
            listCount.value = res.data.id_list.length;
            p_ms_id_list.value = res.data.id_list.map((item) => {
                return { label: item.id, value: item.id };
            });
            showModSelectModal.value = false;
        }
    )
    p_ms_id_list.value = [];
    p_current_id.value = [];
}


// 通过修饰 获取 id 列表
async function get_id_listByMod() {
    axios
        .post("database/selectMod/" + p_current_package.value + "/" + p_current_ms_file.value + "/" + mod.value)
        .then((res) => {
            listCount.value = res.data.id_list.length;
            p_ms_id_list.value = res.data.id_list.map((item) => {
                return { label: item.id, value: item.id };
            });
            showModSelectModal.value = false;
        });
    p_ms_id_list.value = [];
    p_current_id.value = [];
}

// 获取 id 列表
async function get_id_list() {
    axios
        .post("database/" + p_current_package.value + "/" + p_current_ms_file.value)
        .then((res) => {
            listCount.value = res.data.id_list.length;
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
            props.handleData( props.index, p_current_ms_file.value, res.data, p_current_package.value);
        });
}
</script>