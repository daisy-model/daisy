if [ $# -ne 4 ]
then
    echo "Usage:
bash generate_baseline.sh <path-to-daisy-binary> <test-name> <test-directory> <rm-daisy-log>"
    exit 1
fi
daisy_path=${1}
test_name=${2}
test_dir=${3}
base_dir=$(realpath $( dirname -- ${BASH_SOURCE[0]}))
daisy_home=$base_dir/../..
common_dir=$base_dir/tests/common
baseline_dir=$base_dir/baseline/${test_dir}/${test_name}
test_file=${base_dir}/tests/${test_dir}/test-${test_name}.dai

mkdir -p ${baseline_dir}
DAISYHOME=$daisy_home $daisy_path -d $baseline_dir -D $common_dir $test_file
if [ "$4" = "1" ]
then
    rm $baseline_dir/daisy.log
fi
