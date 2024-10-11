import amio_mass from "./amio_mass";

const ProtonMass = 1.00727647;
const HMass = 1.007825035;
const H2OMass = 18.0105647;
const PeptideMass = 18.0105647;

// aminoAcids.set('A', 71.037114);
// aminoAcids.set('R', 156.101111);
// aminoAcids.set('N', 114.042927);
// aminoAcids.set('D', 115.026943);
// aminoAcids.set('C', 103.00918);
// aminoAcids.set('Q', 128.058578);
// aminoAcids.set('E', 129.042593);
// aminoAcids.set('G', 57.021464);
// aminoAcids.set('H', 137.058912);
// aminoAcids.set('I', 113.084064);
// aminoAcids.set('L', 113.084064);
// aminoAcids.set('K', 128.094963);
// aminoAcids.set('M', 131.040485);
// aminoAcids.set('F', 147.068414);
// aminoAcids.set('P', 97.052764);
// aminoAcids.set('S', 87.032028);
// aminoAcids.set('T', 101.047679);
// aminoAcids.set('W', 186.079313);
// aminoAcids.set('Y', 163.063332);
// aminoAcids.set('V', 99.068414);
const aminoAcids = new Map(Object.entries(amio_mass));

function calculatePeptide(peptide, isProteform = false) {

    // 记录修饰的位置
    var mod_position = new Array();
    var mod_stack = new Array();
    var peptide_arr = new Array();
    var amino_acid_cnt = 0;
    var left = 0, right = peptide.length - 1;

    // 处理变体，跳过 . 符号左右的氨基酸
    if (isProteform) {
        while (left <= right && peptide[left] != '.') left++;
        while (left <= right && peptide[right] != '.') right--;
        left++;
        right--;
    }

    // 获取所有未知修饰的位置
    while (left <= right) {
        // 处理包含修饰的部分
        if (peptide[left] == '(') {
            // 这是第几个氨基酸
            mod_stack.push(amino_acid_cnt + 1);
        } else if (peptide[left] == ')') {
            // 修饰的开始位置
            let mod_start = left + 2;
            // left 到达修饰的结束位置
            while (peptide[left] != ']') left++;
            // 如果是修饰
            let mod_mass = 0;
            if (peptide[mod_start] == '+' || peptide[mod_start] == '-') {
                mod_mass = parseFloat(peptide.substring(mod_start, left));
                // 纪录修饰的质量，修饰的起始位置和结束位置
            } else {

                // console.log(peptide.substring(mod_start, left));
                const modArray = peptide.substring(mod_start, left) ? peptide.substring(mod_start, left).split(';') : [];
                for(let modIndex = 0;modIndex<modArray.length;modIndex++){
                    mod_mass += aminoAcids.get(modArray[modIndex], 0);
                }
                // console.log(peptide.substring(mod_start, left), mod_mass);
            }
            mod_position.push([mod_mass, mod_stack.pop(), amino_acid_cnt]);
        } else {
            // 如果是氨基酸，将氨基酸的质量加入到质量数组中
            peptide_arr.push(aminoAcids.get(peptide[left]));
            // 氨基酸个数加一
            amino_acid_cnt++;
        }
        left++;
    }

    var b_ion_arr = new Array(amino_acid_cnt).fill(0);
    var y_ion_arr = new Array(amino_acid_cnt).fill(0);

    for (let i = 0; i < mod_position.length; i++) {
        b_ion_arr[mod_position[i][1] - 1] = mod_position[i][0];
        y_ion_arr[amino_acid_cnt - mod_position[i][2]] = mod_position[i][0];
    }

    b_ion_arr[0] += peptide_arr[0];
    y_ion_arr[0] += peptide_arr[amino_acid_cnt - 1] + H2OMass;

    for (let i = 1; i < amino_acid_cnt; i++) {
        b_ion_arr[i] = b_ion_arr[i] + b_ion_arr[i - 1] + peptide_arr[i];
        y_ion_arr[i] = y_ion_arr[i] + y_ion_arr[i - 1] + peptide_arr[amino_acid_cnt - 1 - i];
    }


    console.log("b_ion:"+b_ion_arr);
    console.log("y_ion:"+y_ion_arr);
    
    return {
        "peptide_mass": y_ion_arr[amino_acid_cnt - 1],
        "peptide_len": amino_acid_cnt,
        "b_ion_arr": b_ion_arr,
        "y_ion_arr": y_ion_arr
    }
}

export {
    calculatePeptide,

    ProtonMass,
    HMass,
    H2OMass,
    PeptideMass,
}