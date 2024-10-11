<template>
    <div style="margin-bottom: 5px;">
        <div v-for="item in peptideTableList"
            :style="{ display: 'inline-block', position: 'relative', margin: '2px', backgroundColor: item.type ? DESCTYPEINFO[item.type].color : '' }">

            <span v-if="item.label.includes('Y')"
                style="color: red; z-index: 999; display: inline-block; font-weight: bold; cursor: pointer; position: absolute; top: 2px"
                @click="handleSplitClick(item, 'Y')" @mouseover="showLabelY(item)" @mouseout="showLabelY(item)">
                ｢
                <div style="position: absolute; top: -15px; left: 0px;" v-if="item.show === 'Y'">
                    {{ item.label.match(/Y\d+/)[0] }}
                </div>
            </span>

            <!-- 修饰符位置 -->
            <span v-if="item.descString" style="
                min-width: 30px; 
                min-height: 30px; 
                width: fit-content; 
                line-height: 30px; 
                text-align: center; 
                position: absolute; 
                bottom: 12px; 
                font-weight: bold; 
                left: 0px; "> 
                {{ DESCTYPEINFO[item.type].label + item.descString }} 
            </span> 

            <span v-else @mouseenter="showDescString(item)" @mouseleave="showDescString(item)"
                style="display: inline-block; min-width: 30px; min-height: 30px; width: fit-content; line-height: 30px; text-align: center; ">
                {{ item.value }}
            </span>

            <span v-if="item.label.includes('B')"
                style="color: blue; z-index: 999; display: inline-block; font-weight: bold; cursor: pointer; position: absolute; right: 1px; bottom: 2px;"
                @click="handleSplitClick(item, 'B')" @mouseover="showLabelB(item)" @mouseout="showLabelB(item)">
                ｣
                <div style="position: absolute;  top: -16px; left: -5px;" v-if="item.show === 'B'">
                    {{ item.label.match(/B\d+/)[0] }}
                </div>
            </span>

        </div>
    </div>
</template>

<script setup>

/**
 * 实现思路：
 * 1， 利用echarts的format函数，捕获Y和B的位置
 * 2， 使用正则从 变体序列 中获得 肽段序列
 */

import { ref } from 'vue';
const DESCTYPEINFO = ref({
    'Acetyl': { color: '#C7EDCC', label: '' },
    'Methyl': { color: '#FFD700', babel: '' },
    'Dimethyl': { color: '#FDE6E0', label: '' },
    'Trimethyl': { color: '#DCE2F1', label: '' },
    'Phospho': { color: '#E3EDCD', label: '' },
    'Default': { color: 'orange', label: '' },
    'Multiple': { color: '#C0C0C0', label: '' },
    '--': 'Unknown'
})


const emits = defineEmits(['handleSplitClick'])

const peptideTableList = ref([]);

// 序列的初始化
const displayPetideList = (proteform) => {
  // 从变体序列中获取肽段序列
  const match = proteform.match(/\.(.*)\./);
  const regex = /(?:\[.*?\])|./g;
  const proteformPura = match[1].match(regex).filter(item => ((/[a-zA-Z]/.test(item)) && item.length == 1));
  peptideTableList.value = proteformPura.map(val => {
    return {
      value: val,
      label: '',
      type: '',
      show: false,
      isHighlightBg: false,
      yData: null,
      bData: null,
    }
  })
}
const showLabelB = (item) => {
    if (item.show === 'B') {
        item.show = ''
    } else {
        item.show = 'B'
    }
}
const showLabelY = (item) => {
    if (item.show === 'Y') {
        item.show = ''
    } else {
        item.show = 'Y'
    }
}
const showDescString = (item) => {
    if (item.descStringIndex || item.descStringIndex === 0) {
        peptideTableList.value[item.descStringIndex].showDescString = !peptideTableList.value[item.descStringIndex].showDescString
    }
}
const handleSplitClick = (item, type) => {
    emits('handleSplitClick', item, type)
}
const insertValue = (pos, data) => {
    if (!pos) return;
    const label = pos[0];
    const posNum = Number(pos.slice(1));

    if (label == 'B') {
        if (peptideTableList.value[posNum - 1] && peptideTableList.value[posNum - 1].label.includes(label)) return;
        peptideTableList.value[posNum - 1].label = peptideTableList.value[posNum - 1].label + pos;
        peptideTableList.value[posNum - 1].bData = data;
    } else {
        if (peptideTableList.value[peptideTableList.value.length - posNum] && peptideTableList.value[peptideTableList.value.length - posNum].label.includes(label)) return;
        peptideTableList.value[peptideTableList.value.length - posNum].label = peptideTableList.value[peptideTableList.value.length - posNum].label + pos;
        peptideTableList.value[peptideTableList.value.length - posNum].yData = data;
    }
}
const getDescType = (descString) => {
    const typeList = Object.keys(DESCTYPEINFO.value);
    let finalType = 'Default';
    const matchedTypes = typeList.filter(type => descString.includes(type));
    // console.log("matchedTypes", matchedTypes)

    if (matchedTypes.length >= 2) {
        finalType = 'Multiple';
    } else if (matchedTypes.length === 1) {
        finalType = matchedTypes[0];
    }

    return finalType;
}
// 增加修饰高亮和修饰类型
const addDataDesc = (proteform) => {

    // 截取句号中间的字符串
    const match = proteform.match(/\.(.*)\./);
    const regex = /(?:\[.*?\])|./g;
    const proteformList = match[1].match(regex);

    let isLabelList = false;
    let isLabelValue = false;
    let descStartPos = null;
    let modIndex = 0;
    // console.log("proteformList", proteformList);
    proteformList.map((str, index) => {
        if (str == '(') {
            isLabelList = true;
            descStartPos = index - modIndex * 2;
            modIndex++;
            // peptideTableList.value.splice(index, 0, {
            //   value: '',
            //   label: '',
            //   show: false,
            //   isHighlightBg: true,
            //   data: null,
            // })
        } else if (str == ')') {
            isLabelList = false;
            // peptideTableList.value.splice(index, 0, {
            //   value: '',
            //   label: '',
            //   show: false,
            //   isHighlightBg: true,
            //   data: null,
            // })
        } else if (str.length > 1) {
            // console.log("peptideTableList", peptideTableList.value);
            // 将修饰符插入到第一个括号位置，需要注意 proteformList 往 peptideTableList映射时，（）的偏移
            peptideTableList.value.splice(descStartPos, 0, {
                value: str,
                label: '',
                show: false,
                isHighlightBg: true,
                data: null,
                type: '-',
                descString: str,
                showDescString: false
            })
            // 标注修饰类型
            const type = getDescType(str);
            // console.log("type", descStartPos, index, str, peptideTableList.value[descStartPos + 1])
            for (let i = descStartPos; i < index - modIndex; i++) {
                if (peptideTableList.value[i]) {
                    peptideTableList.value[i].type = type;
                    peptideTableList.value[i].descStringIndex = descStartPos;
                }
            }
        } else {
            if (isLabelList) {
                // debugger;
                // peptideTableList.value[index].isHighlightBg = true;
            }
        }
    })
}

defineExpose({
    insertValue,
    addDataDesc,
    displayPetideList,
})
</script>