# Summarize coverage for specific directory
[ -z "$1" ] && echo "Missing required argument <directory>" && exit 1
dir=${1}
src_dir=../..
build_dir=.
out_dir=${src_dir}/test/coverage/dai-system-tests/${dir}
out_file=${out_dir}/coverage.html
rm -rf ${out_dir}
mkdir -p ${out_dir}
params=("-r" "${src_dir}" "${build_dir}" "--html-details" "${out_file}" "--sort-percentage" "--exclude" "(.+/)ut_.+\.C$")
params+=("-f" "(.+/)?${dir}/.+\.C$")

echo "Summarizing test coverage for ${dir}"
gcovr ${params[*]} && echo "Coverage report in file://$(realpath ${out_file})"
