dirs=("bioclimate" "chemicals" "components" "crop" "daisy" "gnuplot" "litter" "lower_boundary" "manager" "object_model" "organic_matter" "output" "parameter_types" "programs" "root" "soil" "surface" "transport" "ui" "util" "vegetation" "weather")

source_dir=$( dirname -- ${BASH_SOURCE[0]})
details=${source_dir}/dai_system_test_coverage_details.sh
for dir in ${dirs[*]}
do
    bash ${details} ${dir}
done
